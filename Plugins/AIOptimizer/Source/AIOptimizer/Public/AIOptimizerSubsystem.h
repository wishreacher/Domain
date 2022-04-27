// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIOptimizerSubsystem.generated.h"


class UAIOInvokerComponent;
class UAIOSubjectComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnSubsystemEnabledChanged, bool, bIsEnabled );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnSubjectDespawned, UAIOSubjectComponent*, SubjectComponent );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnSubjectSpawned, UAIOSubjectComponent*, SubjectComponent );


/**
 * AI Optimizer Subsystem helps to optimize the game by reducing cost of Actors that are away from Players.
 * Its main purpose is to be used with AI agents.
 * However it also can be used to optimize any actor in the game depending on your needs.
 * It could be simulating physics actor that you only want to persist when is near the player,
 * or a pickup actor that should only persist when player is close to it etc.
 */
UCLASS()
class AIOPTIMIZER_API UAIOptimizerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UAIOptimizerSubsystem();

	void Initialize( FSubsystemCollectionBase& Collection ) override;
	void Deinitialize() override;

protected:
	void LoadConfigSettings();





	/************************************************************************/
	/*                           CONFIG DATA                                */
	/************************************************************************/
public:
	/** Is subsystem enabled */
	static bool IS_SUBSYSTEM_ENABLED;

	/** How often there will be update checking all registered subjects */
	static float OPTIMIZATION_UPDATE_INTERVAL;

	/** Should debug info be displayed? */
	static bool DISPLAY_DEBUG_INFO;

	/** Whether subjects handled by subsystem are spawned/despawned using queue or are spawned/despawned immediately */
	static bool USE_SPAWN_QUEUE;

	/**
	 * How far to the side subjects should be treated as 'seen' from invokers viewpoint.
	 * The value represents the angle measured in relation to the forward vector, not the whole range.
	 */
	static float PERIPHERAL_VISION_HALF_ANGLE_DEGREES;

	/** same as above but calculated as cos for easier calucations with dot product */
	static float PERIPHERAL_VISION_HALF_ANGLE_COS; 

	/** At which distance subjects should be despawned ( this is default value and may be overridden for each subject ) */
	static float DESPAWN_RADIUS;

	/** How many subjects can be spawned/despawned at once - in single frame during single update */
	static int32 SPAWN_CAPACITY_PER_UPDATE;

	/** How often there is an update during which subjects from pending lists are spawned/despawned */
	static float SPAWNING_INTERVAL;

	/**
	 * This is percent of radius that is subtracted from DespawnRadius to calculate SpawnRadius
	 * for example if DespawnRadius is 1000
	 * then RespawnRadius will be set to 900
	 * This prevents situation when Invoker (Player) moves back and forth causing
	 * subjects to be despawned and spawned immediately
	 * 
	 * This is auto calculated, keep value low ~0.1
	 */
	static float CHANGE_LAYER_BONUS_RADIUS_PERCENT;
	/************************************************************************/






	/************************************************************************/
	/*                     REGISTER / UNREGISTER                            */
	/************************************************************************/
public:
	/**
	 * Registers new invoker to the system ( for example player )
	 * returns true if successfully registered, false if was already registered
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	bool RegisterInvoker( UAIOInvokerComponent* InvokerComponent );

	/**
	 * Registers new invoker to the system ( for example player )
	 * returns true if successfuly unregistered, false if it wasn't registered to the system
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	bool UnregisterInvoker( UAIOInvokerComponent* InvokerComponent );

	/**
	 * Registers new subject to the system ( for example AI )
	 * returns true if successfully registered, false if was already registered
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	bool RegisterSubject( UAIOSubjectComponent* SubjectComponent );

	/**
	 * Registers new subject to the system ( for example AI )
	 * returns true if successfuly unregistered, false if it wasn't registered to the system
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	bool UnregisterSubject( UAIOSubjectComponent* SubjectComponent );
	/************************************************************************/






	/************************************************************************/
	/*                     RUNTIME DATA & LOGIC	                            */
	/************************************************************************/
public:
	/** Event called when subject is being despawned ( at the end of this frame ) */
	UPROPERTY( BlueprintAssignable, Category = AIOptimizerSubsystem )
	FOnSubjectDespawned OnSubjectDespawned;

	/** Event called when subject was spawned */
	UPROPERTY( BlueprintAssignable, Category = AIOptimizerSubsystem )
	FOnSubjectSpawned OnSubjectSpawned;

	/** Event called when subsystem is enabled or disabled */
	UPROPERTY( BlueprintAssignable, Category = AIOptimizerSubsystem )
	FOnSubsystemEnabledChanged OnSubsystemEnabledChanged;

	/** All registered and spawned subjects */
	UPROPERTY()
	TArray<FAIOSubject> SpawnedSubjects;

	/** Subjects that were despawned, but can be spawned again if Invoker will get closer to them */
	UPROPERTY()
	TArray<FAIODespawnedSubject> DespawnedSubjects;

	/**
	 * Invokers - in most cases just players
	 * subjects are calculating distance/angle to each invoker checking if there is any nearby them
	 * if there isn't then subjects are being despawned
	 */
	UPROPERTY()
	TArray<FAIOInvoker> Invokers;


	/**
	 * Subjects that are spawned, but are no longer near any Invoker
	 * and are waiting to be despawned
	 */
	UPROPERTY()
	TArray<FAIOSubject> PendingDespawnSubjectsHeap;

	/**
	 * Subjects that are despawned, but invoker is already near them
	 * and are waiting to be spawned
	 */
	UPROPERTY()
	TArray<FAIODespawnedSubject> PendingSpawnSubjectsHeap;

protected:
	FTimerHandle TimerHandle_UpdateSubjects;
	FTimerHandle TimerHandle_UpdatePendingSubjects;
	FTimerHandle TimerHandle_ShrinkArrays;
	
	/**
	 * Checks if timers updating subjects, starts them if not yet running or stops if there is no need to update anymore
	 * This is called each time invoker is added/removed
	 */
	void CheckUpdateTimers();

	/** Function running on timer being looped */
	UFUNCTION()
	void LoopSubjects();

	/** Function called from LoopSubjects, updating all spawned subjects */
	void UpdateSpawnedSubjects();

	/** Function called from LoopSubjects, updating all despawned subjects */
	void UpdateDespawnedSubjects();


	/** Function running on timer being looped */
	UFUNCTION()
	void LoopPendingSubjects();

	/** Function called from LoopSubjects, updating all pending spawn subjects ( the ones that are despawned but will be soon spawned ) */
	void UpdatePendingSpawnSubjects(); 

	/** Function called from LoopSubjects, updating all pending despawned subjects ( the ones that are spawned but will be soon despawned ) */
	void UpdatePendingDespawnSubjects();
	

	/** Adds subject to pending spawn array - means that it will soon be spawned */
	void AddPendingSpawnSubject( FAIODespawnedSubject& DespawnedSubject );

	/** Adds subject to pending despawn array - means that it will soon be despawned */
	void AddPendingDespawnSubject( FAIOSubject& Subject );

	/** This is actual place where subject is being destroyed and its data is added to DespawnedSubjects */
	void DespawnSubject( FAIOSubject& Subject );

	/** This is actual place where subject is being created ( spawned ) */
	void SpawnSubject( FAIODespawnedSubject& DespawnedSubject );

	/** Function running on loop every X seconds, to shrink all arrays to smallest possible size */
	UFUNCTION()
	void ShrinkArrays();
	/************************************************************************/







	/************************************************************************/
	/*                                 UTILITIES                            */
	/************************************************************************/
public:
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	void SetIsSystemEnabled( bool bIsEnabled );

	/**
	 * Returns distance to closest invoker from given querier location
	 * @param QuerierLocation - Location to which closest invoker will be searched for
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	float GetDistanceToClosestInvokerSquared( const FVector& QuerierLocation );

	/**
	 * Returns closest invoker location from given querier location
	 * @param QuerierLocation - Location to which closest invoker will be searched for
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	FVector GetClosestInvokerLocation( const FVector& QuerierLocation );

	/**
	 * Returns index from Invokers array of given component
	 * INDEX_NONE ( -1 ) if didn't find
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	FORCEINLINE int32 GetInvokerIndex( UAIOInvokerComponent* Component ) { return Invokers.IndexOfByKey( Component ); }

	/**
	 * Returns index from Subjects array of given component
	 * INDEX_NONE ( -1 ) if didn't find
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	FORCEINLINE int32 GetSubjectIndex( UAIOSubjectComponent* Component ) { return SpawnedSubjects.IndexOfByKey( Component ); }

	/**
	 * Checks if given component is registered as Invoker to the system
	 */
	FORCEINLINE bool IsInvokerRegistered( UAIOInvokerComponent* Component ) { return GetInvokerIndex( Component ) != INDEX_NONE; }

	/**
	 * Checks if given component is registered as Subject to the system
	 */
	FORCEINLINE bool IsSubjectRegistered( UAIOSubjectComponent* Component ) { return GetSubjectIndex( Component ) != INDEX_NONE; }

	/**
	 * Checks if given Component is Waiting to be despawned
	 */
	FORCEINLINE bool IsPendingDespawn( UAIOSubjectComponent* Component ) { return PendingDespawnSubjectsHeap.FindByKey(Component) != nullptr; }

	/**
	 * Removes despawned system with given handle
	 * It means that this despawned subject will never be spawned again and will be erased for good
	 * @return - true if successfully removed subject, false if couldn't find it by given handle
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem" )
	bool RemoveDespawnedSubjectByHandle( const FAIOSubjectHandle& Handle );

	/**
	 * Despawns given subject, used in cases where user wants to decide when to despawn subject
	 * instead of letting subsystem decide about it
	 * 
	 * @param SubjectHandle - returned handle of despawned subject, may be used to spawn him again
	 * @param Method - how subject should be despawned? immediately or using stack ( with little delay depending on how many subjects are waiting to be desapwned )
	 * @param Component - SubjectComponent that will be despawned
	 * @param OverrideSpawnRadius - At which radius should it be automatically spawned in? leave at -1 if you want radius to be auto calculated
	 * @param bAllowRespawnOnlyByHandle - if true, then this subject will only be able to respawn by using its Handle
	 * @return - true if subject was successfully despawned
	 */
	UFUNCTION( BlueprintCallable, Category = AIOptimizerSubsystem, meta = ( DisplayName = "DespawnSubject" ) )
	bool K2_DespawnSubject( FAIOSubjectHandle& SubjectHandle, EDespawnMethod Method, UAIOSubjectComponent* Component, float OverrideSpawnRadius = -1.f, bool bAllowRespawnOnlyByHandle = false );

	/**
	 * Despawns given subject, used in cases where user wants to decide when to despawn subject
	 * instead of letting subsystem decide about it
	 *
	 * @param SubjectHandle - handle of Subject that is about to be despawned
	 * @param Method - how subject should be despawned? immediately or using stack ( with little delay depending on how many subjects are waiting to be desapwned )
	 * @param Component - SubjectComponent that will be despawned
	 * @param OverrideSpawnRadius - At which radius should it be automatically spawned in? leave at -1 if you want radius to be auto calculated
	 * @param bAllowRespawnOnlyByHandle - if true, then this subject will only be able to respawn by using its Handle
	 * @return - true if subject was successfully despawned
	 */
	UFUNCTION( BlueprintCallable, Category = AIOptimizerSubsystem, meta = ( DisplayName = "DespawnSubjectByHandle" ) )
	bool K2_DespawnSubjectByHandle( UPARAM(ref) FAIOSubjectHandle& SubjectHandle, EDespawnMethod Method, float OverrideSpawnRadius = -1.f, bool bAllowRespawnOnlyByHandle = false );


	/**
	 * 
	 * @param Method - how subject should be spawned? immediately or using stack ( with little delay depending on how many subjects are waiting to be spawned )
	 * @param SubjectHandle - Handle of subject that will be spawned
	 * @return - true if subject was successfully spawned
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizerSubsystem", meta = ( DisplayName = "SpawnSubjectByHandle" ) )
	bool K2_SpawnSubjectByHandle( EDespawnMethod Method, const FAIOSubjectHandle& SubjectHandle );

protected:
	/** Validation method that checks if there are any invalid invokers that should be removed */
	void RemoveInvalidInvokers();
	/************************************************************************/




	/************************************************************************/
	/*								  DEBUG						            */
	/************************************************************************/
public:
	/**
	 * Gets all subjects registered to the system and creates DebugSubjects array data from them
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizer|Debug" )
	TArray< FAIODebugSubjectData > GetDebugSubjects();

	/**
	 * Categorizes given debug subjects
	 * for example, how many spawned subjects there are/how many despawned subjects etc.
	 * SpawnedSubjects - 10
	 * DespawnedSubjects - 20
	 * etc.
	 */
	UFUNCTION( BlueprintCallable, Category = "AIOptimizer|Debug" )
	TMap< EAIODebugGroup, int32 > GetCategorizedDebugSubjects( const TArray< FAIODebugSubjectData >& DebugSubjects );
	/************************************************************************/




};
