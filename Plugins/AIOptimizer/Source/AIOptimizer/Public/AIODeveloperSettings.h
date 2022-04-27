// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AIODeveloperSettings.generated.h"

class UUserWidget;




/**
 * Configure AI Optimizer Subsystem plugin.
 */
UCLASS( config = Game, defaultconfig, meta = ( DisplayName = "AI Optimizer Subsystem" ) )
class AIOPTIMIZER_API UAIODeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UAIODeveloperSettings();

	/** Should debug info be displayed? */
	UPROPERTY( EditAnywhere, config, Category = "General" )
	uint32 bIsSubsystemEnabled : 1;

	/** Should debug info be displayed? */
	UPROPERTY( EditAnywhere, config, Category = "General" )
	uint32 bDisplayDebugInfo : 1;

	UPROPERTY( EditAnywhere, config, Category = "General" )
	TSubclassOf<UUserWidget> DebugWidgetClass;

	/** How often there will be update checking all registered subjects */
	UPROPERTY( EditAnywhere, config, Category = "General", meta = ( ClampMin = 0.01f ) )
	float OptimizationUpdateInterval;

	UPROPERTY( EditAnywhere, config, Category = "General" )
	EDespawnMethod HandleSpawnDespawnMethod;

	/** How many subjects can be spawn/despawned at once - in single frame */
	UPROPERTY( EditAnywhere, config, Category = "General", meta = ( ClampMin = 1 ) )
	int32 SpawnCapacityPerUpdate;

	/** How often subjects can be spawned/despawned in groups specified in SpawnCapacity */
	UPROPERTY( EditAnywhere, config, Category = "General", meta = ( ClampMin = 0.01f ) )
	float SpawnInterval;

	/**
	 * At which distance subjects should be despawned by the system
	 * This value can be overridden by each subject
	 */
	UPROPERTY( EditAnywhere, config, Category = "General", meta = ( ClampMin = 0.f ) )
	float DespawnRadius;

	/**
	 * How far to the side subjects should be treated as 'seen' from invokers viewpoint.
	 * The value represents the angle measured in relation to the forward vector, not the whole range.
	 */
	UPROPERTY( EditAnywhere, Category = "General", config, meta = (UIMin = 0.0, ClampMin = 0.0, UIMax = 180.0, ClampMax = 180.0 ) )
	float PeripheralVisionHalfAngleDegrees;


	/** Returns default object of this class */
	static const UAIODeveloperSettings* Get() { return GetDefault<UAIODeveloperSettings>(); }
};
