#include "DoorManager.h"
#include "DoorWithNavLink.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DoorManager)

void UDoorManager::RegisterDoor(ADoorWithNavLink *DoorActor)
{
	// expand the door navigation bounds by enough to compensate for the navmesh erosion around the obstacle box
	Doors.Add(FDoorAndBBox(DoorActor, DoorActor->GetNavigationBounds().ExpandBy(FVector(50.0f, 50.0f, 0.0f))));
}

void UDoorManager::UnregisterDoor(ADoorWithNavLink *DoorActor)
{
	Doors.RemoveAll([DoorActor] (const FDoorAndBBox& DoorAndBBox) { return DoorAndBBox.Door == DoorActor; });
}

ADoorWithNavLink* UDoorManager::FindDoor(const FVector &Location) const
{
	for (const FDoorAndBBox& DoorAndBBox : Doors)
	{
		if (DoorAndBBox.BBox.IsInside(Location))
			return DoorAndBBox.Door;
	}
	return nullptr;
}