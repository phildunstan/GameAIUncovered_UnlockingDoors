#pragma once

#include "NavLinkCustomComponent.h"
#include "DoorNavLinkComponent.generated.h"

// Derived from UNavLinkCustomComponent so that we can change the obstacle nav area under the nav link from blueprint.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOORS_API UDoorNavLinkComponent : public UNavLinkCustomComponent
{
	GENERATED_BODY()

public:
	UDoorNavLinkComponent();
	
	virtual bool IsNavigationRelevant() const override { return true; }
	
	UFUNCTION(BlueprintCallable)
	void EnableNavLink();
	
	UFUNCTION(BlueprintCallable)
	void DisableNavLink();
	
	UFUNCTION(BlueprintCallable)
	void SetObstacleNavArea(TSubclassOf<UNavArea> NavAreaClass);
};
