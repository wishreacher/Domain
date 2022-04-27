// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIOData_Base.generated.h"

/**
 * Base Object used as 'save data' of despawned subjects
 * For example AI may want to save its health/state on being despawned
 * to be able to restore them on being spawned again
 * 
 * In such case it would create child blueprint of this class and use it in AIOSubjectComponent
 */
UCLASS( BlueprintType, Blueprintable, Abstract )
class AIOPTIMIZER_API UAIOData_Base : public UObject
{
	GENERATED_BODY()
	
	// Empty by default, add new variables in child classes depending on your needs
};
