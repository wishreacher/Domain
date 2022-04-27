// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#pragma once

#include "AIOptimizer.h"
#include "Components/ActorComponent.h"
#include "AIOptimizerTypes.h"
#include "AIOSubjectComponent.generated.h"


class UAIOptimizerSubsystem;
class UAIOData_Base;
class ISpawnerInterface;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FOnOptimizationUpdate, bool, bIsBeyondLastLayer, int32, LayerIndex, bool, bIsSeen );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnPreDespawn, UAIOData_Base*, Data );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnPostSpawned, UAIOData_Base*, Data );


/**
 * Component which should be added to actors that we want to optimize, like AI Character.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIOPTIMIZER_API UAIOSubjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIOSubjectComponent();

	friend UAIOptimizerSubsystem;



	/************************************************************************/
	/*                     PARENT FUNCTION OVERRIDES                        */
	/************************************************************************/
public:
	virtual void OnRep_IsActive() override;
	virtual void OnComponentDestroyed( bool bDestroyingHierarchy ) override;
	virtual void Activate( bool bReset = false ) override;
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const override;
	/************************************************************************/






	

	/************************************************************************/
	/*                    SUBSYSTEM COMMUCIATION LOGIC                      */
	/************************************************************************/
public:
	/** Called by subsystem to update info to invokers. Distance to closest one, is owner of this comp seen by invokers etc. */
	void UpdateDataToInvokers( UAIOptimizerSubsystem* Subsystem );
	void UpdateOptimizationLayer();

	/**
	 * Returns true if subsystem should despawn this component
	 * For example if this subject is out of range of all invokers
	 */
	UFUNCTION( BlueprintNativeEvent, Category = SubjectComponent )
	bool ShouldBeDespawned( UAIOptimizerSubsystem* Subsystem, bool bForceUpdateDataToInvokers = false );
	

	/**
	 * Returns optimization layer index that subject is currently in ( within its radius )
	 * Returns INDEX_NONE if subject doesn't have any optimization layers
	 * or if is currently above radius of his last layer, for example if last layer radius is 5000, but current distance to closer invoker is 6000
	 */
	UFUNCTION( BlueprintCallable, Category = SubjectComponent )
	int32 GetOptimizationLayerForCurrentDistance();

	/** Returns distance to closest Invoker */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = SubjectComponent )
	FORCEINLINE float GetDistanceToClosestInvoker() const { return FMath::Sqrt( DistanceToClosestInvokerSquared ); }

	/**
	 * Returns true only if this subject is about to be despawned ( destroyed ), in next frame
	 * This can be used to check if actor is being Destroyed by optimizer subsystem
	 */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = SubjectComponent )
	bool IsDespawning() const { return bIsDespawning; }

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = SubjectComponent )
	float GetSpawnRadiusSquared( UAIOptimizerSubsystem* Subsystem );

	UFUNCTION( BlueprintCallable, BlueprintPure, Category = SubjectComponent )
	float GetDespawnRadiusSquared( UAIOptimizerSubsystem* Subsystem );

	/** Returns true if owning actor is seen by any Invoker */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = SubjectComponent )
	FORCEINLINE float IsSeenByAnyInvoker() const { return bIsSeenByAnyInvoker; }

	/** Returns optimization layer index that this subject is currently in */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = SubjectComponent )
	FORCEINLINE int32 GetCurrentOptimizationLayer() const { return CurrentOptimizationLayer; }

	/** Returns location of closest invoker to this subject */
	UFUNCTION( BlueprintCallable, BlueprintPure, Category = SubjectComponent )
	FORCEINLINE FVector& GetClosestInvokerLocation() { return ClosestInvokerLocation; }

	void GenerateHandle() { if( !Handle.IsHandleValid() ) Handle.GenerateId(); }
	void SetHandle( const FAIOSubjectHandle& NewHandle ) { Handle = NewHandle; }
	FORCEINLINE FAIOSubjectHandle& GetHandle() { return Handle; }

	/** Registers this invoker to subsystem, called on Component activation */
	UFUNCTION( BlueprintCallable, Category = SubjectComponent )
	void RegisterSubject();

	/** Unregisters this invoker from subsystem, called on Component deactivation ( or on destroyed ) */
	UFUNCTION( BlueprintCallable, Category = SubjectComponent )
	void UnregisterSubject();

	
protected:
	/** Cached location of where closest invoker is */
	FVector ClosestInvokerLocation;

	/** optimization layer index that this subject is currently in */
	int32 CurrentOptimizationLayer;

	/** Distance to closest invoker calculated in runtime? */
	float DistanceToClosestInvokerSquared;
	float DotProductToClosestInvoker;

	/** Is this subject currently seen by any invoker? */
	uint32 bIsSeenByAnyInvoker : 1;

	/**
	 * This is an Id of this component
	 * It can be used to retrieve data of that component from subsystem when it's despawned
	 * or to force respawn by it
	 */
	UPROPERTY( BlueprintReadOnly, Category = SubjectComponent )
	FAIOSubjectHandle Handle;

	/** Set in runtime to true right before this subject is being despawned */
	uint32 bIsDespawning : 1;
public:
	/**
	 * Event called when subject changes optimization layer or 'IsSeen' state
	 * For example when Invoker gets close to it or goes away.
	 * Called on CLIENT & SERVER
	 */
	UPROPERTY( BlueprintAssignable, Category = Events )
	FOnOptimizationUpdate OnOptimizationUpdate;

	/**
	 * Event called right before this actor will be despawned by Optimizer Subsystem
	 * If Actor is replicated, called only on SERVER
	 * If Actor is not replicated, then called on SERVER and CLIENT
	 */
	UPROPERTY( BlueprintAssignable, Category = Events )
	FOnPreDespawn OnPreDespawn;

	/**
	 * Event called right after this actor was spawned by Optimized Subsystem
	 * ( Called after BEGIN PLAY of owner )
	 * If Actor is replicated, called only on SERVER
	 * If Actor is not replicated, then called on SERVER and CLIENT
	 */
	UPROPERTY( BlueprintAssignable, Category = Events )
	FOnPostSpawned OnPostSpawned;

	/**
	 * Function called by subsystem right before it will be despawned
	 * It returns data object that will be cached by subsystem to restore data after being spawned again
	 * Can return null if subject doesn't need to save anything
	 */
	UAIOData_Base* PreDespawn( UAIOptimizerSubsystem* Subsystem );

	/**
	 * Function called by subsystem right after it was spawned
	 * It takes data that was created before subject was despawned
	 */
	void PostSpawn( UAIOData_Base* Data );
	/************************************************************************/






	/************************************************************************/
	/*                         SPAWNER										*/
	/************************************************************************/
public:
	/** Spawner that created this subject, can be null if spawner was placed on level, or spawned in any other way */
	UPROPERTY( BlueprintReadOnly, Category = SubjectComponent, Replicated )
	AActor* Spawner;

	UFUNCTION( BlueprintCallable, Category = SubjectComponent )
	void SetSpawner( TScriptInterface<ISpawnerInterface> NewSpawner );
	/************************************************************************/






	/************************************************************************/
	/*								SETTINGS								*/
	/************************************************************************/
public:
	/**
	 * Defines if this Subject can be updated by optimizer subsystem, or if should be ignored
	 * This may be useful if we don't want to unregister actor from optimizer subsystem
	 * but only want to disable it temporary, for example during combat
	 */
	UPROPERTY( EditAnywhere, Category = Settings )
	uint32 bCanBeUpdatedBySubsystem : 1;

	/**
	 * Should subsystem be allowed to auto despawn this subject when it is above DespawnRadius?
	 * Disable if owning actor wants to setup optimization logic by himself using OptimizationLayers
	 * without being despawned at all, or by doing it by himself
	 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Settings )
	uint32 bAllowSubsystemToAutoDespawn : 1;

	/**
	 * When bAllowSubsystemToAutoDespawn is set to true, then subsystem is allowed to despawn this subject when it gets to far from Invokers
	 * This value overrides
	 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Settings, meta = ( ClampMin	= -1.f, EditCondition = "bAllowSubsystemToAutoDespawn" ) )
	float OverrideSubsystemDespawnRadius;

	/**
	 * Defines importance of this subject during Spawn/Despawn using Queue
	 * For example if there are 100 subjects waiting to be spawned
	 * but with each update only 10 can be spawned due to Capacity limit
	 * Then Subjects with higher priority will be spawned first
	 */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Settings )
	uint8 Priority;
	
	/**
	 * Class of data save object that will be created and stored when this subject will get despawned
	 */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Settings )
	TSubclassOf<UAIOData_Base> DataClass;

	/**
	 * Optimization layers used to customize optimization settings based on different distance radiuses
	 * For example 
	 *	when subject is far from invoker, it disables most of its features to optimize itself
	 *	when subject is in medium range from invoker, it disables only some of its features
	 *	when subject is close to invoker, it has all features enabled and is fully functional
	 * 
	 * To modify these in runtime, call ReinitializeOptimizationLayers
	 */
	UPROPERTY( EditAnywhere, BlueprintReadOnly, Category = Settings )
	TArray<FAIOptimizationLayer> OptimizationLayers;

	/**
	 * Should calculate angle to invokers to define whether this subject is currently seen by any of them?
	 * Disable if not needed for faster calculations
	 */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Settings, AdvancedDisplay )
	uint32 bShouldCalculateIsSeen : 1;

	/** Allows to reinitialize optimization layers in runtime */
	UFUNCTION( BlueprintCallable, Category = Settings )
	void ReinitializeOptimizationLayers( UPARAM(ref)TArray<FAIOptimizationLayer>& NewOptimizationLayers );

	/**
	 * Defines if this Subject can be updated by optimizer subsystem, or if should be ignored
	 * This may be useful if we don't want to unregister actor from optimizer subsystem
	 * but only want to disable it temporary, for example during combat
	 * 
	 * Default implementation only checks value of variable bCanBeUpdatedBySubsystem
	 * but Blueprint implementation can add other conditions like, is AI in combat, is doing smth important etc.
	 */
	UFUNCTION( BlueprintNativeEvent, Category = SubjectComponent )
	bool CanBeUpdatedBySubsystem();
	bool CanBeUpdatedBySubsystem_Implementation() { return bCanBeUpdatedBySubsystem; }

	/** Setter of variable bCanBeUpdatedBySubsystem */
	UFUNCTION( BlueprintCallable, Category = SubjectComponent )
	void SetCanBeUpdatedBySubsystem( bool bCanBeUpdated) { bCanBeUpdatedBySubsystem = bCanBeUpdated; }


	bool IsSubsystemAllowedToDespawn() const { return bAllowSubsystemToAutoDespawn; }
	bool ShouldCalculateIsSeen() const { return bShouldCalculateIsSeen; }

private:
	/**
	 * This value is used only when subject is despawned 'by hand' instead of being despawned by subsystem
	 * And it allows to specify radius at which subject will be spawned back
	 * It's set by subsystem, don't modify it
	 */
	float OverrideSubsystemSpawnRadiusSquared;
	/** Sets OverrideSubsystemSpawnRadiusSquared, square only if given value is > 0, otherwise don't square to keep value negative ( cuz it is most likely -1 ) */
	void SetOverrideSubsystemSpawnRadius( float SpawnRadius ) { OverrideSubsystemSpawnRadiusSquared = SpawnRadius > 0 ?  FMath::Square( SpawnRadius ) : SpawnRadius; }

	/**
	 * This flag is set by subsystem to true when it was manually set to be despawned
	 */
	uint32 bIsForcedToDespawn : 1;
	void SetIsForcedToDespawn( bool bForce ) { bIsForcedToDespawn = bForce; }
	FORCEINLINE bool IsForcedToDespawn() const { return bIsForcedToDespawn; }

	uint32 bCanBeRespawnedOnlyByHandle : 1;
	void SetCanBeRespawnedOnlyByHandle( bool bRespawnOnlyByHandle ) { bCanBeRespawnedOnlyByHandle = bRespawnOnlyByHandle; }
	FORCEINLINE bool CanBeRespawnedOnlyByHandle() const { return bCanBeRespawnedOnlyByHandle; }
	/************************************************************************/



	/************************************************************************/
	/*								UTILITIES                               */
	/************************************************************************/
public:
	/**
	 * Helper function that helps to optimize character based on given flags
	 */
	UFUNCTION( BlueprintCallable, BlueprintNativeEvent, Category = SubjectComponent )
	void SetCharacterFeatures( ACharacter* Character, UPARAM( meta = ( Bitmask, BitmaskEnum = EAIOFeaturesFlags ) )int32 FeaturesToEnable );

	/************************************************************************/


};
