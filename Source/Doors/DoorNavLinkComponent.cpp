#include "DoorNavLinkComponent.h"

#include "NavigationSystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DoorNavLinkComponent)

void RebuildNavMeshCommand()
{
	UWorld* World = GEngine->GetWorldContexts()[0].World();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid world context!"));
		return;
	}

	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
	{
		NavSys->Build();
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("NavMesh regeneration triggered!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to find Navigation System!"));
	}
}
FAutoConsoleCommand RebuildNavMeshCmd(TEXT("RebuildNavMesh"), TEXT("Forces NavMesh regeneration."), FConsoleCommandDelegate::CreateStatic(RebuildNavMeshCommand));


UDoorNavLinkComponent::UDoorNavLinkComponent()
{
	SetNavigationRelevancy(true);
}

void UDoorNavLinkComponent::EnableNavLink()
{
	SetEnabled(true);
}

void UDoorNavLinkComponent::DisableNavLink()
{
	SetEnabled(false);
}

void UDoorNavLinkComponent::SetObstacleNavArea(TSubclassOf<UNavArea> NavAreaClass)
{
	ObstacleAreaClass = NavAreaClass;
	RefreshNavigationModifiers();
}
