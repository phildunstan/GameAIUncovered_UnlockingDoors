#pragma once

#include "DoorInterface.h"
#include "NavLinkCustomComponent.h"
#include "NavLinkHostInterface.h"
#include "GameFramework/Actor.h"
#include "DoorWithNavLink.generated.h"

class UDoorNavLinkComponent;
class UNavLinkRenderingComponent;
class USceneComponent;

UCLASS(Blueprintable, autoCollapseCategories=(SmartLink, Actor), hideCategories=(Input))
class DOORS_API ADoorWithNavLink : public AActor, public INavLinkHostInterface, public IDoorInterface
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, Category=Components)
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category=Navigation)
	TObjectPtr<UDoorNavLinkComponent> NavLinkComponent;

#if WITH_EDITORONLY_DATA
private:
	/** Editor Preview */
	UPROPERTY()
	TObjectPtr<UNavLinkRenderingComponent> NavLinkRenderingComponent;
public:
#endif // WITH_EDITORONLY_DATA

	virtual void PostLoad() override;
	
	virtual void PostRegisterAllComponents() override;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// BEGIN INavLinkHostInterface - used for rendering the navlink in the editor
	virtual bool GetNavigationLinksClasses(TArray<TSubclassOf<UNavLinkDefinition> >& OutClasses) const override;
	virtual bool GetNavigationLinksArray(TArray<FNavigationLink>& OutLink, TArray<FNavigationSegmentLink>& OutSegments) const override;
	// END INavLinkHostInterface

	virtual FBox GetComponentsBoundingBox(bool bNonColliding = false, bool bIncludeFromChildActors = false) const override;

	FBox GetNavigationBounds() const;

	UFUNCTION()
	void OnSmartLinkReached(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint);

	UFUNCTION(BlueprintNativeEvent)
	void OnDoorNavLinkReached(AActor* Agent, const FVector& Destination);
	
	UFUNCTION(BlueprintCallable)
	void ResumePathFollowing(AActor *Agent);
};
