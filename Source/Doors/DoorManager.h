#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "DoorManager.generated.h"

class ADoorWithNavLink;

UCLASS()
class DOORS_API UDoorManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterDoor(ADoorWithNavLink* DoorActor);
	void UnregisterDoor(ADoorWithNavLink* DoorActor);

	ADoorWithNavLink* FindDoor(const FVector& Location) const;


private:
	struct FDoorAndBBox
	{
		ADoorWithNavLink* Door;
		FBox BBox;
	};
	TArray<FDoorAndBBox> Doors;
};
