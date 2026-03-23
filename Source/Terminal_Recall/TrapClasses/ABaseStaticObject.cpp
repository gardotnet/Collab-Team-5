#include "ABaseStaticObject.h"

ABaseStaticObject::ABaseStaticObject()
{
	bProvidesCover = true;
	bBlocksMovement = true;
}

void ABaseStaticObject::InitializeStaticObject_Implementation()
{
	// Setup collision or grid logic to prevent characters from being on the same tile
}
