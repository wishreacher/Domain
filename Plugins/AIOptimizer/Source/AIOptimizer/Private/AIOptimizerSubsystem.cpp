// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved


#include "AIOptimizerSubsystem.h"
#include "AIOInvokerComponent.h"
#include "AIOSubjectComponent.h"
#include "AIODeveloperSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIOData_Base.h"
#include "TimerManager.h"
#include "Templates/SubclassOf.h"
#include "SpawnerInterface.h"



float UAIOptimizerSubsystem::CHANGE_LAYER_BONUS_RADIUS_PERCENT = 0.1f;
bool UAIOptimizerSubsystem::IS_SUBSYSTEM_ENABLED = true;
float UAIOptimizerSubsystem::OPTIMIZATION_UPDATE_INTERVAL = 0.1f;
bool UAIOptimizerSubsystem::DISPLAY_DEBUG_INFO = false;
bool UAIOptimizerSubsystem::USE_SPAWN_QUEUE = false;
float UAIOptimizerSubsystem::PERIPHERAL_VISION_HALF_ANGLE_DEGREES = 75.f;
float UAIOptimizerSubsystem::PERIPHERAL_VISION_HALF_ANGLE_COS; // auto calculated in runtime
float UAIOptimizerSubsystem::DESPAWN_RADIUS = 3000.f;
float UAIOptimizerSubsystem::SPAWNING_INTERVAL = 0.1f;
int32 UAIOptimizerSubsystem::SPAWN_CAPACITY_PER_UPDATE = 5;




DECLARE_STATS_GROUP( TEXT( "AIOptimizerSubsystem" ), STATGROUP_AIOptimizerSubsystem, STATCAT_Advanced );
DECLARE_CYCLE_STAT( TEXT( "UpdateSpawnedSubjects" ), STAT_UpdateSpawnedSubjects, STATGROUP_AIOptimizerSubsystem );
DECLARE_CYCLE_STAT( TEXT( "UpdateDespawnedSubjects" ), STAT_UpdateDespawnedSubjects, STATGROUP_AIOptimizerSubsystem );
DECLARE_CYCLE_STAT( TEXT( "SpawnSubject" ), STAT_SpawnSubject, STATGROUP_AIOptimizerSubsystem );
DECLARE_CYCLE_STAT( TEXT( "DespawnSubject" ), STAT_DespawnSubject, STATGROUP_AIOptimizerSubsystem );
DECLARE_CYCLE_STAT( TEXT( "UpdatePendingSpawnSubjects" ), STAT_UpdatePendingSpawnSubjects, STATGROUP_AIOptimizerSubsystem );
DECLARE_CYCLE_STAT( TEXT( "UpdatePendingDespawnSubjects" ), STAT_UpdatePendingDespawnSubjects, STATGROUP_AIOptimizerSubsystem );



UAIOptimizerSubsystem::UAIOptimizerSubsystem()
{
	OPTIMIZATION_UPDATE_INTERVAL = 0.15f;
	DISPLAY_DEBUG_INFO = true;

	DESPAWN_RADIUS = 5000.f;
	PERIPHERAL_VISION_HALF_ANGLE_DEGREES = 45.f;
	PERIPHERAL_VISION_HALF_ANGLE_COS = FMath::Cos( FMath::Clamp( FMath::DegreesToRadians( PERIPHERAL_VISION_HALF_ANGLE_DEGREES ), 0.f, PI ) );
}

void UAIOptimizerSubsystem::Initialize( FSubsystemCollectionBase& Collection )
{
	Super::Initialize( Collection );
	LoadConfigSettings();
	UE_LOG( LogAIOptimizer, Log, TEXT( "AI Optimizer Subsystem initialized" ) );
}

void UAIOptimizerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG( LogAIOptimizer, Log, TEXT( "AI Optimizer Subsystem deinitialized" ) );
}

void UAIOptimizerSubsystem::LoadConfigSettings()
{
	if( const UAIODeveloperSettings* settings = UAIODeveloperSettings::Get() )
	{
		USE_SPAWN_QUEUE = settings->HandleSpawnDespawnMethod == EDespawnMethod::UseQueue;
		DISPLAY_DEBUG_INFO = settings->bDisplayDebugInfo;
		OPTIMIZATION_UPDATE_INTERVAL = settings->OptimizationUpdateInterval;
		SPAWNING_INTERVAL = settings->SpawnInterval;
		SPAWN_CAPACITY_PER_UPDATE = settings->SpawnCapacityPerUpdate;
		DESPAWN_RADIUS = settings->DespawnRadius;
		IS_SUBSYSTEM_ENABLED = settings->bIsSubsystemEnabled;

		PERIPHERAL_VISION_HALF_ANGLE_DEGREES = settings->PeripheralVisionHalfAngleDegrees;
		PERIPHERAL_VISION_HALF_ANGLE_COS = FMath::Cos( FMath::Clamp( FMath::DegreesToRadians( PERIPHERAL_VISION_HALF_ANGLE_DEGREES ), 0.f, PI ) );
	}
	else
	{
		UE_LOG( LogAIOptimizer, Warning, TEXT( "Couldn't read values from config file!" ) );
	}
}















bool UAIOptimizerSubsystem::RegisterInvoker( UAIOInvokerComponent* InvokerComponent )
{
	if( IsValid( InvokerComponent ) )
	{
		if( !IsInvokerRegistered( InvokerComponent ) )
		{
			UE_CLOG( DISPLAY_DEBUG_INFO, LogAIOptimizer, Log, TEXT( "Added Invoker: %s" ), *InvokerComponent->GetOwner()->GetName() );
			Invokers.Emplace( InvokerComponent );

			CheckUpdateTimers();
			return true;
		}
	}

	return false;
}

bool UAIOptimizerSubsystem::UnregisterInvoker( UAIOInvokerComponent* InvokerComponent )
{
	if( IsValid( InvokerComponent ) )
	{
		int32 foundIndex = Invokers.IndexOfByKey( InvokerComponent );
		if( foundIndex != INDEX_NONE )
		{
			UE_CLOG( DISPLAY_DEBUG_INFO, LogAIOptimizer, Log, TEXT( "Removed Invoker: %s" ), *InvokerComponent->GetOwner()->GetName() );
			Invokers.RemoveAt( foundIndex, 1, false );

			CheckUpdateTimers();
			return true;
		}
	}

	return false;
}

bool UAIOptimizerSubsystem::RegisterSubject( UAIOSubjectComponent* SubjectComponent )
{
	if( IsValid( SubjectComponent ) )
	{
		if( !IsSubjectRegistered( SubjectComponent ) )
		{
			UE_CLOG( DISPLAY_DEBUG_INFO, LogAIOptimizer, Log, TEXT( "Added Subject: %s" ), *SubjectComponent->GetOwner()->GetName() );
			SpawnedSubjects.Emplace( SubjectComponent );
			return true;
		}
	}

	return false;

}

bool UAIOptimizerSubsystem::UnregisterSubject( UAIOSubjectComponent* SubjectComponent )
{
	if( IsValid( SubjectComponent ) )
	{
		int32 foundIndex = SpawnedSubjects.IndexOfByKey( SubjectComponent );
		if( foundIndex != INDEX_NONE )
		{
			UE_CLOG( DISPLAY_DEBUG_INFO, LogAIOptimizer, Log, TEXT( "Removed Subject: %s" ), *SubjectComponent->GetOwner()->GetName() );
			SpawnedSubjects.RemoveAt( foundIndex, 1, false );
			return true;
		}

		// given component could already be in pending despawn subjects array, check it as well
		foundIndex = PendingDespawnSubjectsHeap.IndexOfByKey( SubjectComponent );
		if( foundIndex != INDEX_NONE )
		{
			UE_CLOG( DISPLAY_DEBUG_INFO, LogAIOptimizer, Log, TEXT( "Removed Subject: %s" ), *SubjectComponent->GetOwner()->GetName() );
			PendingDespawnSubjectsHeap.RemoveAt( foundIndex, 1, false );
			return true;
		}
	}

	return false;
}














void UAIOptimizerSubsystem::CheckUpdateTimers()
{
	if( !IS_SUBSYSTEM_ENABLED )
		return;

	if( UWorld* world = GetWorld() )
	{
		FTimerManager& timerManager = world->GetTimerManager();

		bool bIsTimerActive = timerManager.IsTimerActive( TimerHandle_UpdateSubjects );

		// are there any invokers?
		if( Invokers.Num() > 0 )
		{
			// is update loop active? (replace with variable)
			if( bIsTimerActive == false )
			{
				timerManager.SetTimer( TimerHandle_UpdateSubjects, this, &UAIOptimizerSubsystem::LoopSubjects, OPTIMIZATION_UPDATE_INTERVAL, true, 0.f );
				timerManager.SetTimer( TimerHandle_UpdatePendingSubjects, this, &UAIOptimizerSubsystem::LoopPendingSubjects, SPAWNING_INTERVAL, true, 0.f );

				// shrink all arrays every 30s
				timerManager.SetTimer( TimerHandle_ShrinkArrays, this, &UAIOptimizerSubsystem::ShrinkArrays, 30.f, true, 0.f );
			}
		}
		// if there aren't any invokers, then stop all timers cuz there is no context to update subjects
		else if( bIsTimerActive )
		{
			timerManager.ClearAllTimersForObject( this );
		}
	}
}













void UAIOptimizerSubsystem::LoopSubjects()
{
	// remove invalid invokers in case there are any
	// it's called there to make sure that in updating subjects they will be valid and there will be no need to check them every iteration
	RemoveInvalidInvokers();

	UpdateSpawnedSubjects();
	UpdateDespawnedSubjects();
}

void UAIOptimizerSubsystem::UpdateSpawnedSubjects()
{
	SCOPE_CYCLE_COUNTER( STAT_UpdateSpawnedSubjects );

	// reversed loop through subjects to be able to remove invalid subjects if there are any
	for( int32 i = SpawnedSubjects.Num() - 1; i >= 0; i-- )
	{
		// check if subject is not valid, if it is -> remove it
		if( SpawnedSubjects[ i ].IsOwnerValid() == false )
		{
			UE_CLOG( DISPLAY_DEBUG_INFO, LogAIOptimizer, Warning, TEXT( "Invalid Subject at index %d" ), i );
			SpawnedSubjects.RemoveAt( i, 1, false );
			continue;
		}

		// cache component for ease of use
		UAIOSubjectComponent* subjectComponent = SpawnedSubjects[ i ].Component;

		// skip checking this subject if it can't be updated
		if( subjectComponent->CanBeUpdatedBySubsystem() == false )
		{
			continue;
		}

		// update subject's data to invokers ( distance/dot etc. )
		subjectComponent->UpdateDataToInvokers( this );

		if( subjectComponent->ShouldBeDespawned( this ) )
		{
			if( USE_SPAWN_QUEUE )
				AddPendingDespawnSubject( SpawnedSubjects[ i ] );
			else
				DespawnSubject( SpawnedSubjects[ i ] );

			SpawnedSubjects.RemoveAt( i, 1, false );
		}
		else
		{
			subjectComponent->UpdateOptimizationLayer();
		}
	}
}

void UAIOptimizerSubsystem::UpdateDespawnedSubjects()
{
	SCOPE_CYCLE_COUNTER( STAT_UpdateDespawnedSubjects );

	// reversed loop though despawned subjects
	for( int32 i = DespawnedSubjects.Num() - 1; i >= 0; i-- )
	{
		FVector subjectLocation = DespawnedSubjects[ i ].GetActorLocation();

		bool bCanBeSpawned = false;

		if( DespawnedSubjects[ i ].bIsForcedToSpawn ) // always allow to spawn subject if this flag is 'true'
			bCanBeSpawned = true;
		else if( DespawnedSubjects[ i ].bCanBeRespawnedOnlyByHandle ) // if flag above is false and this flag is 'false' then don't allow to spawn
			bCanBeSpawned = false;
		else if( GetDistanceToClosestInvokerSquared( subjectLocation ) < DespawnedSubjects[ i ].SpawnRadiusSquared ) // otherwise just check spawn distance
			bCanBeSpawned = true;

		if( bCanBeSpawned )
		{
			if( USE_SPAWN_QUEUE )
				AddPendingSpawnSubject( DespawnedSubjects[ i ] );
			else
				SpawnSubject( DespawnedSubjects[ i ] );

			DespawnedSubjects.RemoveAt( i, 1, false );
		}
	}
}

















void UAIOptimizerSubsystem::LoopPendingSubjects()
{
	// remove invalid invokers in case there are any
	// it's called there to make sure that in updating pending subjects they will be valid and there will be no need to check them every iteration
	RemoveInvalidInvokers();

	UpdatePendingSpawnSubjects();
	UpdatePendingDespawnSubjects();
}




void UAIOptimizerSubsystem::UpdatePendingSpawnSubjects()
{
	SCOPE_CYCLE_COUNTER( STAT_UpdatePendingSpawnSubjects );

	if( PendingSpawnSubjectsHeap.Num() > 0 )
	{
		int32 spawnedCounter = 0;

		for( int32 i = 0; i < PendingSpawnSubjectsHeap.Num(); i++ )
		{
			FAIODespawnedSubject despawnedSubject;
			PendingSpawnSubjectsHeap.HeapPop( despawnedSubject, false );

			FVector subjectLocation = despawnedSubject.GetActorLocation();

			bool bCanBeSpawned = false;

			if( despawnedSubject.bIsForcedToSpawn ) // always allow to spawn subject if this flag is 'true'
				bCanBeSpawned = true;
			else if( despawnedSubject.bCanBeRespawnedOnlyByHandle ) // if flag above is false and this flag is 'false' then don't allow to spawn
				bCanBeSpawned = false;
			else if( GetDistanceToClosestInvokerSquared( subjectLocation ) < despawnedSubject.SpawnRadiusSquared ) // otherwise just check spawn distance
				bCanBeSpawned = true;

			if( bCanBeSpawned )
			{
				

				SpawnSubject( despawnedSubject );
				spawnedCounter++;

				// check if limit of despawned subjects was hit, if it was then stop executing and wait for next update
				if( spawnedCounter == SPAWN_CAPACITY_PER_UPDATE )
				{
					break;
				}
			}
			// if can't be spawned, it means that it is still not in spawn radius and should get back to DespawnedSubjects and wait for any invoker to get closer
			else
			{
				DespawnedSubjects.Add( despawnedSubject );
			}
		}
	}
}

void UAIOptimizerSubsystem::UpdatePendingDespawnSubjects()
{
	SCOPE_CYCLE_COUNTER( STAT_UpdatePendingDespawnSubjects );

	// check if there are any subjects waiting to be despawned
	if( PendingDespawnSubjectsHeap.Num() > 0 )
	{
		// num of despawned subjects during this iteration
		int32 despawnedCounter = 0;

		for( int32 i = 0; i < PendingDespawnSubjectsHeap.Num(); i++ )
		{
			FAIOSubject subject;
			PendingDespawnSubjectsHeap.HeapPop( subject, false );

			// make sure subject is still valid ( could be destroyed or smth )
			if( subject.IsOwnerValid() == false )
			{
				continue;
			}

			// check if this subject still can be despawned, or if should be returned to Subjects array
			bool bShouldBeDespawned = subject.Component->IsForcedToDespawn() ||
				subject.Component->ShouldBeDespawned( this, true );

			// check if this subject still can be despawned
			if( bShouldBeDespawned )
			{
				DespawnSubject( subject );
				despawnedCounter++;

				// check if limit of despawned subjects was hit, if it was then stop executing and wait for next update
				if( despawnedCounter == SPAWN_CAPACITY_PER_UPDATE )
				{
					break;
				}
			}
			// if it can't be despawned, it means that it went back to acceptable layer radius and should be returned to SpawnedSubjects array
			else
			{
				SpawnedSubjects.Add( subject );
			}
		}
	}
}

void UAIOptimizerSubsystem::AddPendingSpawnSubject( FAIODespawnedSubject& DespawnedSubject )
{
	PendingSpawnSubjectsHeap.HeapPush( DespawnedSubject );
}


void UAIOptimizerSubsystem::AddPendingDespawnSubject( FAIOSubject& Subject )
{
	PendingDespawnSubjectsHeap.HeapPush( Subject );
}


void UAIOptimizerSubsystem::DespawnSubject( FAIOSubject& Subject )
{
	SCOPE_CYCLE_COUNTER( STAT_DespawnSubject );

	AActor* subjectActor = Subject.GetActor();

	// If actor is replicated, then allow it to be despawned only on Server side
	// for non replicated actors it can be despawned both on client and server
	bool bCanDespawn = subjectActor->GetIsReplicated() == false || subjectActor->HasAuthority();

	if( bCanDespawn )
	{
		UAIOData_Base* data = Subject.Component->PreDespawn( this );

		FAIODespawnedSubject despawnedSubject;
		despawnedSubject.Transform = subjectActor->GetTransform();
		despawnedSubject.Class = subjectActor->GetClass();
		despawnedSubject.Data = data;
		despawnedSubject.Priority = Subject.Priority;
		despawnedSubject.Handle = Subject.Component->GetHandle();
		despawnedSubject.bCanBeRespawnedOnlyByHandle = Subject.Component->CanBeRespawnedOnlyByHandle();
		despawnedSubject.SpawnRadiusSquared = Subject.Component->GetSpawnRadiusSquared( this );
		despawnedSubject.Spawner = Subject.Component->Spawner;
		DespawnedSubjects.Add( despawnedSubject );

		OnSubjectDespawned.Broadcast( Subject.Component );

		Subject.GetActor()->Destroy();
	}
}

void UAIOptimizerSubsystem::SpawnSubject( FAIODespawnedSubject& DespawnedSubject )
{
	SCOPE_CYCLE_COUNTER( STAT_SpawnSubject );

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AActor* spawnedActor = GetWorld()->SpawnActor(
		DespawnedSubject.Class,
		&DespawnedSubject.Transform,
		spawnParams );

	// just to make sure it wasn't destroyed right after being spawned
	if( IsValid( spawnedActor ) )
	{
		UAIOSubjectComponent* subjectComp = spawnedActor->FindComponentByClass<UAIOSubjectComponent>();
		check( subjectComp ); // must be valid but verify that

		subjectComp->SetHandle( DespawnedSubject.Handle ); // give it back handle that was used before despawn
		subjectComp->SetSpawner( DespawnedSubject.Spawner ); // set spawner back if there was any

		subjectComp->PostSpawn( DespawnedSubject.Data ); // notify component that it was spawned
		OnSubjectSpawned.Broadcast( subjectComp );


		// destroy data object cuz it is no longer needed
		if( IsValid( DespawnedSubject.Data ) )
		{
			DespawnedSubject.Data->MarkPendingKill();
			DespawnedSubject.Data = nullptr;
		}
	}
	else
	{
		UE_LOG( LogAIOptimizer, Error, TEXT( "Subject of class %s was destroyed right after spawn" ), *DespawnedSubject.Class->GetName() );
	}
}



















void UAIOptimizerSubsystem::ShrinkArrays()
{
	SpawnedSubjects.Shrink();
	DespawnedSubjects.Shrink();

	PendingDespawnSubjectsHeap.Shrink();
	PendingSpawnSubjectsHeap.Shrink();
}

void UAIOptimizerSubsystem::SetIsSystemEnabled( bool bIsEnabled )
{
	if( IS_SUBSYSTEM_ENABLED != bIsEnabled )
	{
		// update value
		IS_SUBSYSTEM_ENABLED = bIsEnabled;


		if( bIsEnabled )
		{
			// update timers to run them again
			CheckUpdateTimers();
		}
		else
		{
			// stop all timers
			GetWorld()->GetTimerManager().ClearAllTimersForObject( this );

			// force respawn of all subjects that were despawned
			for( FAIODespawnedSubject& despawnedSubject : DespawnedSubjects )
			{
				SpawnSubject( despawnedSubject );
			}
			DespawnedSubjects.Empty();

			for( FAIODespawnedSubject& despawnedSubject : PendingSpawnSubjectsHeap )
			{
				SpawnSubject( despawnedSubject );
			}
			DespawnedSubjects.Empty();
		}

		OnSubsystemEnabledChanged.Broadcast( bIsEnabled );
	}
}

float UAIOptimizerSubsystem::GetDistanceToClosestInvokerSquared( const FVector& QuerierLocation )
{
	float closestInvokerDistanceSquared = FLT_MAX;

	for( int32 i = Invokers.Num() - 1; i >= 0; i-- )
	{
		FVector invokerLocation = Invokers[ i ].GetActorLocation();

		// FIND BEST DISTANCE
		float currentInvokerDistanceSquared = UKismetMathLibrary::Vector_DistanceSquared( QuerierLocation, invokerLocation );
		if( currentInvokerDistanceSquared < closestInvokerDistanceSquared )
		{
			closestInvokerDistanceSquared = currentInvokerDistanceSquared;
		}
	}

	return closestInvokerDistanceSquared;
}

FVector UAIOptimizerSubsystem::GetClosestInvokerLocation( const FVector& QuerierLocation )
{
	FVector resultLoc = FVector::ZeroVector;
	float closestInvokerDistanceSquared = FLT_MAX;

	for( int32 i = Invokers.Num() - 1; i >= 0; i-- )
	{
		FVector invokerLocation = Invokers[ i ].GetActorLocation();

		// FIND BEST DISTANCE
		float currentInvokerDistanceSquared = UKismetMathLibrary::Vector_DistanceSquared( QuerierLocation, invokerLocation );
		if( currentInvokerDistanceSquared < closestInvokerDistanceSquared )
		{
			closestInvokerDistanceSquared = currentInvokerDistanceSquared;
			resultLoc = invokerLocation;
		}
	}

	return resultLoc;
}

bool UAIOptimizerSubsystem::K2_DespawnSubjectByHandle( FAIOSubjectHandle& SubjectHandle, EDespawnMethod Method, float OverrideSpawnRadius /*= -1.f*/, bool bAllowRespawnOnlyByHandle /*= false */ )
{
	// find subject by given handle
	FAIOSubject* subject = nullptr;

	// firstly check SpawnedSubjects
	subject = SpawnedSubjects.FindByKey( SubjectHandle );

	// if it failed, then check PendingDespawnSubjects
	if( !subject ) subject = PendingDespawnSubjectsHeap.FindByKey( SubjectHandle );

	// if subject was found then despawn it using K2_DespawnSubject
	if( subject )
	{
		return K2_DespawnSubject( SubjectHandle, Method, subject->Component, OverrideSpawnRadius, bAllowRespawnOnlyByHandle );
	}



	// but if at this point subject was still not found
	// then check if it is already despawned and if is DespawnedSubjects or PendingSpawnSubjects
	// cuz if it's there then we need to reset flags
	FAIODespawnedSubject* despawnedSubject = nullptr;

	// firstly check SpawnedSubjects
	despawnedSubject = DespawnedSubjects.FindByKey( SubjectHandle );

	// if it failed, then check PendingDespawnSubjects
	if( !despawnedSubject ) despawnedSubject = PendingSpawnSubjectsHeap.FindByKey( SubjectHandle );

	// if subject was found, reset flags
	if( despawnedSubject )
	{
		despawnedSubject->bIsForcedToSpawn = false;
		despawnedSubject->bCanBeRespawnedOnlyByHandle = bAllowRespawnOnlyByHandle;

		// false because subject was already despawned and we only changed its flags
		return false;
	}

	UE_LOG( LogAIOptimizer, Warning, TEXT( "Subject with handle %s couldn't be found in the system" ), *SubjectHandle.ToString() );
	return false;
}


bool UAIOptimizerSubsystem::K2_DespawnSubject( FAIOSubjectHandle& SubjectHandle, EDespawnMethod Method, UAIOSubjectComponent* Component, float OverrideSpawnRadius /*= -1.f*/, bool bAllowRespawnOnlyByHandle /*= false */ )
{
	if( IsValid( Component ) )
	{
		SubjectHandle = Component->GetHandle();
		Component->SetOverrideSubsystemSpawnRadius( OverrideSpawnRadius );
		Component->SetIsForcedToDespawn( true );
		Component->SetCanBeRespawnedOnlyByHandle( bAllowRespawnOnlyByHandle );

		// if wants to be despawned immediately then simply pass it to DespawnSubject function
		if( Method == EDespawnMethod::Immediately )
		{
			// find it in SpawnedSubjects
			int32 foundIndex = SpawnedSubjects.IndexOfByKey( Component );
			if( foundIndex != INDEX_NONE )
			{
				// despawn and remove from spawned subjects array
				DespawnSubject( SpawnedSubjects[ foundIndex ] );
				SpawnedSubjects.RemoveAt( foundIndex, 1, false );
				return true;
			}

			// if it wasn't find in SpawnedSubjects, then check PendingDespawnSubjects, it could be there
			foundIndex = PendingDespawnSubjectsHeap.IndexOfByKey( Component );
			if( foundIndex != INDEX_NONE )
			{
				// despawn and remove from pending despawn array
				DespawnSubject( PendingDespawnSubjectsHeap[ foundIndex ] );
				PendingDespawnSubjectsHeap.RemoveAt( foundIndex, 1, false );
				return true;
			}
		}
		else if( Method == EDespawnMethod::UseQueue )
		{
			// find it in SpawnedSubjects
			int32 foundIndex = SpawnedSubjects.IndexOfByKey( Component );
			if( foundIndex != INDEX_NONE )
			{
				AddPendingDespawnSubject( SpawnedSubjects[ foundIndex ] );
				SpawnedSubjects.RemoveAt( foundIndex, 1, false );
				return true;
			}

			// if it wasn't find in SpawnedSubjects, then check PendingDespawnSubjects, it could be there
			foundIndex = PendingDespawnSubjectsHeap.IndexOfByKey( Component );
			if( foundIndex != INDEX_NONE )
			{
				// despawn and remove from pending despawn array
				DespawnSubject( PendingDespawnSubjectsHeap[ foundIndex ] );
				PendingDespawnSubjectsHeap.RemoveAt( foundIndex, 1, false );
				return true;
			}

			// if is already in pending despawn then it's ok, do nothing and return true
			if( IsPendingDespawn( Component ) )
			{
				return true;
			}
		}
		UE_LOG( LogAIOptimizer, Warning, TEXT( "Trying to despawn subject %s that is not registered to the system" ), *Component->GetOwner()->GetName() );
	}

	return false;
}

bool UAIOptimizerSubsystem::K2_SpawnSubjectByHandle( EDespawnMethod Method, const FAIOSubjectHandle& SubjectHandle )
{
	if( Method == EDespawnMethod::Immediately )
	{
		// find it in DespawnedSubjects by Id
		int32 foundIndex = DespawnedSubjects.IndexOfByKey( SubjectHandle );
		if( foundIndex != INDEX_NONE )
		{
			SpawnSubject( DespawnedSubjects[ foundIndex ] );
			DespawnedSubjects.RemoveAt( foundIndex, 1, false );
			return true;
		}

		// if it wasn't find in DespawnedSubjects, then check PendingSpawnSubjects, it could be there
		foundIndex = PendingSpawnSubjectsHeap.IndexOfByKey( SubjectHandle );
		if( foundIndex != INDEX_NONE )
		{
			SpawnSubject( PendingSpawnSubjectsHeap[ foundIndex ] );
			PendingSpawnSubjectsHeap.RemoveAt( foundIndex, 1, false );
			return true;
		}
	}
	else if( Method == EDespawnMethod::UseQueue )
	{
		// check if despawned subject with given handle exists
		int32 foundIndex = DespawnedSubjects.IndexOfByKey( SubjectHandle );
		if( foundIndex != INDEX_NONE )
		{
			DespawnedSubjects[ foundIndex ].bIsForcedToSpawn = true;
			AddPendingSpawnSubject( DespawnedSubjects[ foundIndex ] );
			DespawnedSubjects.RemoveAt( foundIndex, 1, false );
			return true;
		}

		// if it wasn't find in DespawnedSubjects, then also check PendingSpawnSubjects, maybe he is there
		foundIndex = PendingSpawnSubjectsHeap.IndexOfByKey( SubjectHandle );
		if( foundIndex != INDEX_NONE )
		{
			PendingSpawnSubjectsHeap[ foundIndex ].bIsForcedToSpawn = true;
			return true;
		}
	}

	// if subject with given handle wasn't found in DespawnedSubjects & PendingSpawnSubjects
	// then check SpawnedSubjects & PendingDespawnedSubjects
	// because it could be there with flag ForcedToDespawn set to true and in such case we need to reset it

	// find subject by given handle
	FAIOSubject* subject = nullptr;

	// firstly check SpawnedSubjects
	subject = SpawnedSubjects.FindByKey( SubjectHandle );

	// if it failed, then check PendingDespawnSubjects
	if( !subject ) subject = PendingDespawnSubjectsHeap.FindByKey( SubjectHandle );

	// if subject was found, reset flags
	if( subject && subject->IsOwnerValid() )
	{
		subject->Component->SetIsForcedToDespawn( false );
		// return false because subject was already spawned and we only did reset its flags
		return false;
	}


	UE_LOG( LogAIOptimizer, Warning, TEXT( "Trying to spawn subject by handle %s that is not registered to the system" ), *SubjectHandle.ToString() );
	return false;
}

bool UAIOptimizerSubsystem::RemoveDespawnedSubjectByHandle( const FAIOSubjectHandle& Handle )
{
	// check if despawned subject with given handle exists
	int32 foundIndex = DespawnedSubjects.IndexOfByKey( Handle );
	if( foundIndex != INDEX_NONE )
	{
		DespawnedSubjects.RemoveAt( foundIndex, 1, false );
		return true;
	}

	// if it wasn't find in DespawnedSubjects, then also check PendingSpawnSubjects, maybe he is there
	foundIndex = PendingSpawnSubjectsHeap.IndexOfByKey( Handle );
	if( foundIndex != INDEX_NONE )
	{
		PendingSpawnSubjectsHeap.RemoveAt( foundIndex, 1, false );
		return true;
	}


	return false;
}


void UAIOptimizerSubsystem::RemoveInvalidInvokers()
{
	for( int32 i = Invokers.Num() - 1; i >= 0; i-- )
	{
		// check if invoker is valid
		if( Invokers[ i ].IsOwnerValid() == false )
		{
			UE_CLOG( DISPLAY_DEBUG_INFO, LogAIOptimizer, Warning, TEXT( "Invalid Invoker at index %d" ), i );
			Invokers.RemoveAt( i, 1, false );
			continue;
		}
	}
}













TArray< FAIODebugSubjectData > UAIOptimizerSubsystem::GetDebugSubjects()
{
	// prepare array that will be returned
	TArray< FAIODebugSubjectData > resultArray;

	// loop through all spawned subjects
	for( FAIOSubject& subject : SpawnedSubjects )
	{
		if( !subject.IsOwnerValid() )
			continue;

		FAIODebugSubjectData debugSubject;
		debugSubject.bIsSpawned = true;
		debugSubject.bIsPending = false;
		debugSubject.SubjectLocation = subject.GetActorLocation();
		debugSubject.InvokerLocation = subject.Component->GetClosestInvokerLocation();
		debugSubject.DespawnRadius = FMath::Sqrt( subject.Component->GetDespawnRadiusSquared( this ) );
		debugSubject.DistanceToInvoker = subject.Component->GetDistanceToClosestInvoker();
		debugSubject.bNotUpdated = !subject.Component->CanBeUpdatedBySubsystem();
		debugSubject.Layer = subject.Component->GetCurrentOptimizationLayer();
		debugSubject.bIsSeen = subject.Component->IsSeenByAnyInvoker();

		resultArray.Add( debugSubject );
	}

	// loop through all pending despawn subjects
	for( FAIOSubject& subject : PendingDespawnSubjectsHeap )
	{
		if( !subject.IsOwnerValid() )
			continue;

		FAIODebugSubjectData debugSubject;
		debugSubject.bIsSpawned = true;
		debugSubject.bIsPending = true;
		debugSubject.SubjectLocation = subject.GetActorLocation();
		debugSubject.InvokerLocation = subject.Component->GetClosestInvokerLocation();
		debugSubject.DespawnRadius = FMath::Sqrt( subject.Component->GetDespawnRadiusSquared( this ) );
		debugSubject.DistanceToInvoker = subject.Component->GetDistanceToClosestInvoker();
		debugSubject.Layer = subject.Component->GetCurrentOptimizationLayer();
		debugSubject.bIsSeen = subject.Component->IsSeenByAnyInvoker();

		resultArray.Add( debugSubject );
	}

	// loop through all despawned subjects
	for( FAIODespawnedSubject& despawnedSubject : DespawnedSubjects )
	{
		FAIODebugSubjectData debugSubject;
		debugSubject.bIsSpawned = false;
		debugSubject.bIsPending = false;
		debugSubject.SubjectLocation = despawnedSubject.GetActorLocation();
		debugSubject.InvokerLocation = GetClosestInvokerLocation( debugSubject.SubjectLocation );
		resultArray.Add( debugSubject );
	}

	// loop through all pending spawn subjects
	for( FAIODespawnedSubject& despawnedSubject : PendingSpawnSubjectsHeap )
	{
		FAIODebugSubjectData debugSubject;
		debugSubject.bIsSpawned = false;
		debugSubject.bIsPending = true;
		debugSubject.SubjectLocation = despawnedSubject.GetActorLocation();
		resultArray.Add( debugSubject );
	}

	return resultArray;
}


TMap< EAIODebugGroup, int32 > UAIOptimizerSubsystem::GetCategorizedDebugSubjects( const TArray< FAIODebugSubjectData >& DebugSubjects )
{
	// prepare map that will be filled in next step
	TMap< EAIODebugGroup, int32 > resultMap;
	resultMap.Add( EAIODebugGroup::Spawned, 0 );
	resultMap.Add( EAIODebugGroup::Despawned, 0 );
	resultMap.Add( EAIODebugGroup::PendingSpawn, 0 );
	resultMap.Add( EAIODebugGroup::PendingDespawn, 0 );
	resultMap.Add( EAIODebugGroup::SpawnedClose, 0 );
	resultMap.Add( EAIODebugGroup::SpawnedMedium, 0 );
	resultMap.Add( EAIODebugGroup::SpawnedFar, 0 );
	resultMap.Add( EAIODebugGroup::NotUpdated, 0 );


	// loop through each subject and decide to which type should it go
	for( auto& debugSubject : DebugSubjects )
	{
		// is it spawned subject?
		if( debugSubject.bIsSpawned )
		{
			// is pending despawn?
			if( debugSubject.bIsPending )
			{
				resultMap[ EAIODebugGroup::PendingDespawn ]++;
			}
			else
			{
				resultMap[ EAIODebugGroup::Spawned ]++;

				// is not updated by subsystem
				if( debugSubject.bNotUpdated )
				{
					resultMap[ EAIODebugGroup::NotUpdated ]++;
					continue;
				}

				// check distance type
				float distanceAlpha = UKismetMathLibrary::FClamp( debugSubject.DistanceToInvoker / debugSubject.DespawnRadius, 0.f, 1.f );

				if( distanceAlpha < 0.33f )
					resultMap[ EAIODebugGroup::SpawnedClose ]++;

				else if( distanceAlpha < 0.66f )
					resultMap[ EAIODebugGroup::SpawnedMedium ]++;

				else
					resultMap[ EAIODebugGroup::SpawnedFar ]++;
			}
		}
		// is it despawned subject?
		else
		{
			// is pending spawn?
			if( debugSubject.bIsPending )
				resultMap[ EAIODebugGroup::PendingSpawn ]++;

			else
				resultMap[ EAIODebugGroup::Despawned ]++;
		}
	}

	return resultMap;
}

