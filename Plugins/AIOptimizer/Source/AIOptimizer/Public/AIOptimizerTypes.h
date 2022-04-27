// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#pragma once


#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "AIOptimizerTypes.generated.h"



/** Helper macros to operate on bits */
#define TEST_BIT(Bitmask, Bit) (((Bitmask) & (1 << static_cast<uint32>(Bit))) > 0)
#define SET_BIT(Bitmask, Bit) (Bitmask |= 1 << static_cast<uint32>(Bit))
#define CLEAR_BIT(Bitmask, Bit) (Bitmask &= ~(1 << static_cast<uint32>(Bit)))


static const FName INVOKER_TAG = TEXT("invoker"); // tag added to owner of invoker components
static const FName SUBJECT_TAG = TEXT("subject"); // tag added to owner of subject components


class UAIOSubjectComponent;
class UAIOInvokerComponent;
class UAIOData_Base;
class AActor;

// keeps track of last assigned ID to subject handle, to make sure each subject will have unique number
static uint32 LAST_SUBJECT_HANDLE_ID = 1;
static const uint32 INVALID_SUBJECT_HANDLE_ID = -1;

/**
 * This Enum describes methods of despawning subjects
 * whether it should happen immediately or using stack
 */
UENUM( BlueprintType )
enum class EDespawnMethod : uint8
{
	UseQueue		UMETA( ToolTip = "Subject that is about to be spawned/despawned will be added to the queue and will be removed checking spawn/despawn capacity" ),
	Immediately		UMETA( ToolTip = "Subject that is about to be spawned/despawned will be spawned/despawned immediately without any delay" )
};


/**
 * This enum is helper defining what features subject may have that can be enabled/disabled
 */
UENUM( BlueprintType, meta = ( Bitflags ) )
enum class EAIOFeaturesFlags : uint8
{
	AIBrain				UMETA( ToolTip = "Behavior Tree / Logic" ),
	MovementComponent	UMETA( ToolTip = "Character Movement Component" ),
	Visibility			UMETA( ToolTip = "Visibility of the Actor ( Hide/Show In Game )" ),
	Collision			UMETA( ToolTip = "Collision enabled on the Actor" ),
	Animations			UMETA( ToolTip = "Pause/Stop playing animation blueprint" ),
	ActorTick			UMETA( ToolTip = "Enable/Disable Tick function running on Actor" ),
	Shadows				UMETA( ToolTip = "Show/Hide shadows of Actor" ),
	ClothSimulation		UMETA( ToolTip = "Enable/Disable cloth simulation on SkeletalMesh")
};


/**
 * This struct is an Id of a subject
 * each subject component receives unique handle (Id)
 * with which it can be searched in subsystem even when its despawned ( destroyed ) to get/update its data or to force respawn
 */
USTRUCT( BlueprintType )
struct AIOPTIMIZER_API FAIOSubjectHandle
{
	GENERATED_BODY()

	FAIOSubjectHandle() :
		HandleId( INVALID_SUBJECT_HANDLE_ID )
		{}

	UPROPERTY( BlueprintReadOnly, Category = AIOptimizer )
	int32 HandleId;

	void GenerateId() { HandleId = LAST_SUBJECT_HANDLE_ID; LAST_SUBJECT_HANDLE_ID++; }
	bool IsHandleValid() const { return HandleId > INVALID_SUBJECT_HANDLE_ID; }
	FString ToString() const { return FString::FromInt( HandleId ); }

	FORCEINLINE bool operator == ( const FAIOSubjectHandle& OtherHandle ) const
	{
		return HandleId == OtherHandle.HandleId;
	}
};


/**
 * This struct represents subject that is despawned ( destroyed from the world )
 * and its data is being stored in there, so it can be recreated when needed
 * 
 * For Example AI that is registered to the system, but was despawned ( destroyed ) from the world
 */
USTRUCT( BlueprintType )
struct AIOPTIMIZER_API FAIODespawnedSubject
{
	GENERATED_BODY()

	FAIODespawnedSubject() :
		Class( nullptr ),
		SpawnRadiusSquared( 0.f ),
		Priority( 0 ),
		Data( nullptr ),
		bIsForcedToSpawn( false ),
		bCanBeRespawnedOnlyByHandle( false ),
		Spawner( nullptr )
		{}

	/** Transform with which subject will be respawned */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	FTransform Transform;

	/** Class of subject that will be respawned */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	TSubclassOf<AActor> Class;

	/** Required distance to closest invoker to respawn this subject */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	float SpawnRadiusSquared;

	/** Imporance of respawning this subject ( look at UAIOSubjectComponent Priority description ) */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	uint8 Priority;

	/** Pointer to save data object, can be null if not used by subject */
	UPROPERTY()
	UAIOData_Base* Data;

	/** Handle/Id of this subject, can be used to force respawn without checking other required conditions */
	UPROPERTY( BlueprintReadOnly, Category = AIOptimizer )
	FAIOSubjectHandle Handle;

	/** Flag that is set in runtime if this subject is forced to be respawned */
	UPROPERTY( BlueprintReadOnly, Category = AIOptimizer )
	uint32 bIsForcedToSpawn : 1;

	/** Whether this subject can only be respawned using its handle */
	UPROPERTY( BlueprintReadOnly, Category = AIOptimizer )
	uint32 bCanBeRespawnedOnlyByHandle : 1;

	/** Spawner that created this subject, can be null if spawner was placed on level, or spawned in any other way */
	UPROPERTY( BlueprintReadOnly, Category = AIOptimizer )
	AActor* Spawner;


	/** Returns location of subject ( in moment of despawn ) */
	FORCEINLINE FVector GetActorLocation() const { return Transform.GetLocation(); }

	/** used to sort by priorities in ascending order (lowest values first) */
	FORCEINLINE bool operator < ( const FAIODespawnedSubject& Other ) const
	{
		return Priority < Other.Priority;
	}

	/** Used to find by subject ( using handle ) */
	FORCEINLINE bool operator == ( const FAIODespawnedSubject& Other ) const
	{
		return Handle == Other.Handle;
	}

	/** Used to find by handle */
	FORCEINLINE bool operator == ( const FAIOSubjectHandle& OtherHandle ) const
	{
		return Handle == OtherHandle;
	}
};


/**
 * This struct represents subject that is spawned
 * For Example AI Agent that is registered to the system but still spawned in the world
 */
USTRUCT( BlueprintType )
struct AIOPTIMIZER_API FAIOSubject
{
	GENERATED_BODY()

	FAIOSubject() :
		Component( nullptr ),
		Priority( 0 )
		{}

	FAIOSubject( UAIOSubjectComponent* InComponent );

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	UAIOSubjectComponent* Component;

	/** Imporance of respawning this subject ( look at UAIOSubjectComponent Priority description ) */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	int32 Priority;

	/** Returns true if owning actor is Valid */
	bool IsOwnerValid();
	FORCEINLINE FVector GetActorLocation() const;
	FORCEINLINE AActor* GetActor() const;

	/** used to find by component */
	FORCEINLINE bool operator == ( const UAIOSubjectComponent* const OtherComponent ) const
	{
		return Component == OtherComponent;
	}

	/** Used to find by handle */
	FORCEINLINE bool operator == ( const FAIOSubjectHandle& OtherHandle ) const;

	/** used to sort by priorities in ascending order (lowest values first) */
	FORCEINLINE bool operator < ( const FAIOSubject& Other ) const
	{
		return Priority < Other.Priority;
	}
};


/**
 * This struct represents invoker
 * Most likely Players that moves around the map and from whom distance is calculated to subjects
 * to decide whether subjects should be optimized/despawned/spawned
 */
USTRUCT( BlueprintType )
struct AIOPTIMIZER_API FAIOInvoker
{
	GENERATED_BODY()

	FAIOInvoker() :
		Component( nullptr )
		{}

	FAIOInvoker( UAIOInvokerComponent* InComponent ) :
		Component( InComponent )
		{}

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	UAIOInvokerComponent* Component;

	/** Returns true if owning actor is Valid */
	bool IsOwnerValid();
	FORCEINLINE FVector GetActorLocation() const;
	FORCEINLINE AActor* GetActor() const;

	// used to find by component
	FORCEINLINE bool operator == ( const UAIOInvokerComponent* const OtherComponent ) const
	{
		return Component == OtherComponent;
	}
};


/**
 * This struct represents optimization layer
 * that is used by subjects who wants to have some custom optimization settings
 * 
 * For Example subject may have 2 optimization layers
 * 1 with 1000 radius
 * 2 with 2000 radius
 * and then when invoker is within radius 1000 to subject, it may be fully functional
 * but when it invoker will be in 2nd layer, then subject may want to disable some of its features, like shadows/tick etc. to optimize itself
 */
USTRUCT( BlueprintType )
struct AIOPTIMIZER_API FAIOptimizationLayer
{
	GENERATED_BODY()

public:
	FAIOptimizationLayer() :
		LayerRadius( 1000.f )
		{}

	/** Max radius from subject of this layer, in which must be any invoker to make this layer active */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	float LayerRadius;

	/** Layer Radius Squared cached in param to faster calculations */
	float LayerRadiusSquared;

	FORCEINLINE float GetSquaredLayerRadius() const { return LayerRadiusSquared; }

	/** Updates value of LayerRadiusSquared */
	void UpdateLayerRadiusSquared() { LayerRadiusSquared = FMath::Square( LayerRadius ); }

	FORCEINLINE bool operator < ( const FAIOptimizationLayer& OtherLayer ) const
	{
		return LayerRadius < OtherLayer.LayerRadius;
	}
};








/************************************************************************/
/*							DEBUG ONLY DATA STRUCT                      */
/************************************************************************/


/**
 * This struct is used to represent subject in debug mode
 */
USTRUCT( BlueprintType )
struct AIOPTIMIZER_API FAIODebugSubjectData
{
	GENERATED_BODY()

	FAIODebugSubjectData() :
		Layer( -1 ),
		SubjectLocation( FVector::ZeroVector ),
		InvokerLocation( FVector:: ZeroVector ),
		bIsSpawned( false ),
		bIsPending( false ),
		bNotUpdated( false ),
		bIsSeen( false ),
		DistanceToInvoker( 0.f ),
		DespawnRadius( 0.f )
		{}

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	int32 Layer;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	FVector SubjectLocation;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	FVector InvokerLocation;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	uint32 bIsSpawned : 1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	uint32 bIsPending : 1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	uint32 bNotUpdated : 1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	uint32 bIsSeen : 1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	float DistanceToInvoker;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	float DespawnRadius;
};


/**
 * This enum helps to categorize subjects in debug mode
 */
 UENUM( BlueprintType )
 enum class EAIODebugGroup : uint8
 {
	Undefined,
	Spawned,
	Despawned,
	PendingSpawn,
	PendingDespawn,
	SpawnedClose,
	SpawnedMedium,
	SpawnedFar,
	NotUpdated
 };








/************************************************************************/
/*							AI SPAWNER TYPES		                    */
/************************************************************************/

/**
 * This enum defines method of choosing spawn points for spawning Actors
 */
UENUM( BlueprintType )
enum class EAIOSelectSpawnPointsMethod : uint8
{
	UseRandomPoints,
	UseSpecifiedSpawnPoints
};



/**
 * This enum defines if and how spawned Actors should be respawned after being killed
 */
UENUM( BlueprintType )
enum class EAIORespawnMethod : uint8
{
	Undefined			UMETA( ToolTip = "Don't respawn at all"),
	AllAtOnce			UMETA( ToolTip = "Respawn entire group when all Actors of this spawned are killed" ),
	EachIndividually	UMETA( ToolTip = "Respawn each Actor individualy after it's killed" )
};



/**
 * This enum defines how spawn points should be projected on the ground
 */
UENUM( BlueprintType )
enum class EAIOSpawnPointsProjectionMethod : uint8
{
	None		UMETA( ToolTip = "Don't project spawn points at all" ),
	Geometry	UMETA( ToolTip = "Project points on geometry using trace checks" ),
	Navigation	UMETA( ToolTip = "Project points on navmesh" )
};



/**
 * This enum defines when and how Spawner should start spawning Actors
 */
UENUM( BlueprintType )
enum class EAIOStartSpawningMethod : uint8
{
	None				UMETA( ToolTip = "Don't spawn at all" ),
	SpawnOnGameStart	UMETA( ToolTip = "Spawn right after game starts ( On Begin Play )" ),
	SpawnOnRadius		UMETA( ToolTip = "Spawn when Invoker(Player) enters specified SpawnRadius" ),
	SpawnOnRegion		UMETA( ToolTip = "Spawn when Invoker(Player) enters specified Region" )
};


/**
 * This struct defines group of Actors that should be spawned by Spawner
 */
USTRUCT( BlueprintType )
struct FAIOPendingSpawnGroup
{
	GENERATED_BODY()

	/** Number of Actors already spawned */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	int32 SpawnedAmount = 0;

	/** Total number of Actors to spawn */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	int32 TotalAmountToSpawn = 0;
};


/**
 * This struct defines group of Actors that should be respawned by Spawner
 */
USTRUCT( BlueprintType )
struct FAIOPendingRespawnGroup
{
	GENERATED_BODY()

	/** World time when this group should be respawned */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	float SpawnGameTime = 0.f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer )
	FAIOPendingSpawnGroup SpawnGroup;
};


/**
 * This struct defines point where Actors should be spawned by Spawner
 */
USTRUCT( BlueprintType )
struct FAIOSpawnPoint
{
	GENERATED_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = AIOptimizer,  meta = ( MakeEditWidget ) )
	FTransform Transform;
};