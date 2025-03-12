#pragma once

#include "NavAreas/NavArea.h"
#include "DoorNavArea_Door.generated.h"

class UObject;

/** Custom NavArea for Doors. This is used to identify doors as NPCs are pathing, in addition to adjusting navigation costs and filters. */
UCLASS(Config = Game)
class DOORS_API UDoorNavArea_Door : public UNavArea
{
	GENERATED_BODY()
	
public:
	UDoorNavArea_Door();
};
