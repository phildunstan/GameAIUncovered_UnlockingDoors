#pragma once

#include "Components/ActorComponent.h"
#include "UObject/WeakObjectPtr.h"

#include "DoorOpenerComponent.generated.h"

class INavMovementInterface;
class UPathFollowingComponent;

// This component looks ahead on the path that an AI character is following to determine if it is approaching a door.
// If so, it will try to trigger that door to open.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DOORS_API UDoorOpenerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDoorOpenerComponent();

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/** The distance ahead along the path that this component will check for a nav link path segment.
	 *  If this is <= 0, no checks will be performed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Doors")
	float DistanceAheadOnPath = 0.0f;

private:
	UPROPERTY()
	TWeakObjectPtr<UPathFollowingComponent> PathFollowingComponent;
	
	INavMovementInterface* NavMovementInterface = nullptr;

	AActor* IsApproachingDoor() const;
};