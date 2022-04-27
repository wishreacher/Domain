// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved


#include "AIODeveloperSettings.h"

UAIODeveloperSettings::UAIODeveloperSettings()
{
	bDisplayDebugInfo = false;
	OptimizationUpdateInterval = 0.1f;
	PeripheralVisionHalfAngleDegrees = 75.f;
	SpawnCapacityPerUpdate = 5;
	SpawnInterval = 0.1f;

	DespawnRadius = 3000.f;
	bIsSubsystemEnabled = true;
	HandleSpawnDespawnMethod = EDespawnMethod::Immediately;
}
