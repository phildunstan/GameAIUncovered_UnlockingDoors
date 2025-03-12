#include "DoorWithNavLink.h"

#include "DoorNavLinkComponent.h"
#include "DoorManager.h"
#include "NavigationSystem.h"
#include "NavLinkCustomComponent.h"
#include "NavLinkRenderingComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "VisualLogger/VisualLogger.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DoorWithNavLink)

ADoorWithNavLink::ADoorWithNavLink(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	NavLinkComponent = CreateDefaultSubobject<UDoorNavLinkComponent>(TEXT("NavLinkComponent"));
	NavLinkComponent->SetMoveReachedLink(this, &ADoorWithNavLink::OnSmartLinkReached);

#if WITH_EDITORONLY_DATA
	NavLinkRenderingComponent = CreateDefaultSubobject<UNavLinkRenderingComponent>(TEXT("NavLinkRenderingComponent"));
	NavLinkRenderingComponent->SetupAttachment(RootComponent);
#endif // WITH_EDITORONLY_DATA
}

void ADoorWithNavLink::PostLoad()
{
	Super::PostLoad();
	
	// if (NavLinkComponent && NavLinkComponent->IsA<UDoorNavLinkComponent>())
	// {
	// 	NavLinkComponent->RemoveFromRoot();
	// 	NavLinkComponent = nullptr;
	// }
}

void ADoorWithNavLink::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	if (RootComponent)
	{
		RootComponent->TransformUpdated.AddLambda([this](USceneComponent*, EUpdateTransformFlags, ETeleportType) { FNavigationSystem::UpdateActorData(*this); });
	}
}



void ADoorWithNavLink::BeginPlay()
{
#if ENABLE_VISUAL_LOG
	// used to set up redirection of log owner redirection to the visual logger
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		REDIRECT_OBJECT_TO_VLOG(this, NavSys);
	}
#endif // ENABLE_VISUAL_LOG

	Super::BeginPlay();
	
	if (UDoorManager* DoorsManager = GetWorld()->GetSubsystem<UDoorManager>())
		DoorsManager->RegisterDoor(this);
}

void ADoorWithNavLink::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (UDoorManager* DoorsManager = GetWorld()->GetSubsystem<UDoorManager>())
		DoorsManager->RegisterDoor(this);
}

bool ADoorWithNavLink::GetNavigationLinksClasses(TArray<TSubclassOf<UNavLinkDefinition>>& OutClasses) const
{
	return false;
}

bool ADoorWithNavLink::GetNavigationLinksArray(TArray<FNavigationLink>& OutLink, TArray<FNavigationSegmentLink>& OutSegments) const
{
	if (NavLinkComponent && NavLinkComponent->IsNavigationRelevant())
	{
		OutLink.Add(NavLinkComponent->GetLinkModifier());
		return true;
	}
	return false;
}

FBox ADoorWithNavLink::GetComponentsBoundingBox(bool bNonColliding, bool bIncludeFromChildActors) const
{
	FBox DoorBox = Super::GetComponentsBoundingBox(bNonColliding, bIncludeFromChildActors);

	if (NavLinkComponent && NavLinkComponent->IsNavigationRelevant())
	{
		DoorBox += NavLinkComponent->GetNavigationBounds();
	}

	return DoorBox;
}

FBox ADoorWithNavLink::GetNavigationBounds() const
{
	if (NavLinkComponent)
	{
		return NavLinkComponent->GetNavigationBounds();
	}
	return FBox();
}

void ADoorWithNavLink::OnSmartLinkReached(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint)
{
	if (UPathFollowingComponent* PathComp = Cast<UPathFollowingComponent>(PathingAgent))
	{
		AActor* PathOwner = PathComp->GetOwner();
		if (AController* ControllerOwner = Cast<AController>(PathOwner))
		{
			PathOwner = ControllerOwner->GetPawn();
		}
		OnDoorNavLinkReached(PathOwner, DestPoint);
	}
}

void ADoorWithNavLink::OnDoorNavLinkReached_Implementation(AActor* Agent, const FVector& Destination)
{
}

void ADoorWithNavLink::ResumePathFollowing(AActor* Agent)
{
	if (Agent)
	{
		UPathFollowingComponent* PathComp = Agent->FindComponentByClass<UPathFollowingComponent>();
		if (!PathComp)
		{
			APawn* PawnOwner = Cast<APawn>(Agent);
			if (PawnOwner && PawnOwner->GetController())
			{
				PathComp = PawnOwner->GetController()->FindComponentByClass<UPathFollowingComponent>();
			}
		}

		if (PathComp)
		{
			PathComp->FinishUsingCustomLink(NavLinkComponent);
		}
	}
}

