#include "DoorOpenerComponent.h"

#include "DoorInterface.h"
#include "DoorWithNavLink.h"
#include "DoorManager.h"
#include "DoorNavArea_Door.h"
#include "NavLinkCustomInterface.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "NavMesh/NavMeshPath.h"
#include "NavMesh/RecastNavMesh.h"
#include "Navigation/PathFollowingComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DoorOpenerComponent)

UDoorOpenerComponent::UDoorOpenerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UDoorOpenerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	PathFollowingComponent = Cast<UPathFollowingComponent>(GetOwner()->GetComponentByClass(UPathFollowingComponent::StaticClass()));
	
	const APawn* MyPawn = Cast<APawn>(GetOwner());
	if (!MyPawn)
	{
		if (AController* MyController = Cast<AController>(GetOwner()))
		{
			MyPawn = MyController->GetPawn();
		}
	}
	if (MyPawn)
	{
		NavMovementInterface = MyPawn->FindComponentByInterface<INavMovementInterface>();
	}
}


// This function uses a very simple model of movement to project ahead on the path by DistanceAheadOnPath
// This function is very tightly coupled to the implementation of the PathFollowingComponent
void UDoorOpenerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PathFollowingComponent.IsValid())
		return;
	if (!NavMovementInterface)
		return;
	
	if (PathFollowingComponent->HasValidPath())
	{
		if (AActor* DoorActor = IsApproachingDoor())
		{
			check(DoorActor->Implements<UDoorInterface>());
			IDoorInterface::Execute_OpenDoor(DoorActor);
		}
	}
}


AActor* UDoorOpenerComponent::IsApproachingDoor() const
{
	if (DistanceAheadOnPath <= 0.0f)
		return nullptr;
	
	const FNavPathSharedPtr Path = PathFollowingComponent->GetPath();
	const TArray<FNavPathPoint>& PathPoints = Path->GetPathPoints();

	FVector CharacterLocationOnSegment = NavMovementInterface->GetFeetLocation();
	uint32 MoveSegmentStartIndex = PathFollowingComponent->GetCurrentPathIndex();
	uint32 MoveSegmentEndIndex = PathFollowingComponent->GetNextPathIndex();
	float RemainingDistanceAheadOnPath = DistanceAheadOnPath;

	// Walk along the path segments until we reach DistanceAheadOnPath ahead.
	while (RemainingDistanceAheadOnPath > 0.0f)
	{
		if (!PathPoints.IsValidIndex(MoveSegmentStartIndex))
			// There is no more path to follow
			return nullptr;

		// Check to see if the next path segment contains a door.
		// A door could either be implemented as either a nav link segment or a normal navmesh segment
		// using the UDoorNavArea_Door area type.
		const FNavPathPoint& PathPoint = PathPoints[MoveSegmentStartIndex];
		const FNavMeshNodeFlags SegmentFlags = FNavMeshNodeFlags(PathPoint.Flags);
		if (const ANavigationData* NavData = Cast<ANavigationData>(FNavigationSystem::GetNavDataForActor(*PathFollowingComponent->GetOwner())))
		{
			if (NavData->GetAreaClass(SegmentFlags.Area) == UDoorNavArea_Door::StaticClass())
			{
				if (SegmentFlags.IsNavLink())
				{
					// We've found a nav link segment
					const UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
					if (const INavLinkCustomInterface* MoveSegmentCustomLink = NavSys->GetCustomLink(PathPoint.CustomNavLinkId))
					{
						// Find the door actor by using the CustomNavLinkId to get the NavLinkComponent
						if (const UActorComponent* NavLinkComponent = Cast<UActorComponent>(MoveSegmentCustomLink))
						{
							if (AActor* NavLinkOwner = NavLinkComponent->GetOwner())
							{
								if (NavLinkOwner->Implements<UDoorInterface>())
								{
									return NavLinkOwner;
								}
							}
						}
					}
				}
				else
				{
					// This could also be used to replace the logic above used when the segement is a nav link segment.
					// I've included it separately to show that the doors manager is not needed
					// if doors only ever use the implementation using the nav link segment when closed.
					if (const UDoorManager* DoorsManager = GetWorld()->GetSubsystem<UDoorManager>())
					{
						if (ADoorWithNavLink* Door = DoorsManager->FindDoor(PathPoint.Location))
						{
							return Door;
						}
					}
				}
			}
		}

		const FVector SegmentEndLocation = *Path->GetPathPointLocation(MoveSegmentEndIndex);
		const float DistanceToSegmentEnd = FVector::Dist(CharacterLocationOnSegment, SegmentEndLocation);
		RemainingDistanceAheadOnPath -= DistanceToSegmentEnd;

		MoveSegmentStartIndex = MoveSegmentEndIndex;
		MoveSegmentEndIndex = MoveSegmentEndIndex + 1;
		CharacterLocationOnSegment = SegmentEndLocation;
	}
	
	return nullptr;
}
