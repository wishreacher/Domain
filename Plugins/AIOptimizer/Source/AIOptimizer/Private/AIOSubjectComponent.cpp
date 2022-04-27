// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#include "AIOSubjectComponent.h"
#include "AIOptimizerSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "AIOBPLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "SpawnerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"



// Sets default values for this component's properties
UAIOSubjectComponent::UAIOSubjectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	SetAutoActivate( true ); // by default activates and is registered to the system
	SetIsReplicatedByDefault( true ); // replicated to replicate bIsActive and register both on client and server if needed
	bWantsInitializeComponent = true; // needed to get InitializeComponent

	bAllowSubsystemToAutoDespawn = true;
	bShouldCalculateIsSeen = false;
	bCanBeUpdatedBySubsystem = true;
	bCanBeRespawnedOnlyByHandle = false;
	Spawner = nullptr;

	OverrideSubsystemSpawnRadiusSquared = -1.f;
	OverrideSubsystemDespawnRadius = -1.f;
	bIsSeenByAnyInvoker = false;
	DistanceToClosestInvokerSquared = 0.f;
	CurrentOptimizationLayer = -999;
}


void UAIOSubjectComponent::OnComponentDestroyed( bool bDestroyingHierarchy )
{
	Super::OnComponentDestroyed( bDestroyingHierarchy );

	UnregisterSubject();
}

void UAIOSubjectComponent::Activate( bool bReset /*= false */ )
{
	RegisterSubject();
}


void UAIOSubjectComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// make sure it's runtime, not editor
	if( UWorld* world = GetWorld() )
	{
		if( world->IsGameWorld() )
		{
			GenerateHandle();

			// update squared values of optimization layers for ease of calculations later on
			for( FAIOptimizationLayer& layer : OptimizationLayers )
			{
				layer.UpdateLayerRadiusSquared();
			}

			if( AActor* owner = GetOwner() )
			{
				owner->Tags.Add( SUBJECT_TAG );
			}
		}
	}
}

void UAIOSubjectComponent::GetLifetimeReplicatedProps( TArray<FLifetimeProperty>& OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps( OutLifetimeProps );

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST( UAIOSubjectComponent, Spawner, SharedParams );
}

void UAIOSubjectComponent::OnRep_IsActive()
{
	if( IsActive() ) // for CLIENTS
	{
		RegisterSubject();
	}
}


void UAIOSubjectComponent::RegisterSubject()
{
	// sort before registering to make sure layers are in correct order ( from smallest to biggest )
	OptimizationLayers.Sort();
	GenerateHandle();

	if( UWorld* world = GetWorld() )
	{
		if( world->IsGameWorld() ) // make sure it is runtime, not editor
		{
			if( UAIOptimizerSubsystem* subsystem = world->GetSubsystem<UAIOptimizerSubsystem>() )
			{
				subsystem->RegisterSubject( this );
			}
		}
	}
}

void UAIOSubjectComponent::UnregisterSubject()
{
	if( UWorld* world = GetWorld() )
	{
		if( world->IsGameWorld() ) // make sure it is runtime, not editor
		{
			if( UAIOptimizerSubsystem* subsystem = world->GetSubsystem<UAIOptimizerSubsystem>() )
			{
				subsystem->RegisterSubject( this );
			}
		}
	}
}




bool UAIOSubjectComponent::ShouldBeDespawned_Implementation( UAIOptimizerSubsystem* Subsystem, bool bForceUpdateDataToInvokers /*= false */ )
{
	if( bIsForcedToDespawn )
	{
		return true;
	}

	// update data to invokers if needed ( distance, is seen etc. )
	if( bForceUpdateDataToInvokers )
	{
		UpdateDataToInvokers( Subsystem );
	}

	// check if this subject allows subsystem to optimize it
	if( IsSubsystemAllowedToDespawn() == false )
	{
		return false;
	}

	if( !CanBeUpdatedBySubsystem() )
	{
		return false;
	}

	float despawnDistance = GetDespawnRadiusSquared( Subsystem );
	if( DistanceToClosestInvokerSquared > despawnDistance )
	{
		return true;
	}

	return false;
}



UAIOData_Base* UAIOSubjectComponent::PreDespawn( UAIOptimizerSubsystem* Subsystem )
{
	// mark this actor as despawning
	bIsDespawning = true;

	UAIOData_Base* dataSaveObject = nullptr;

	if( DataClass )
	{
		dataSaveObject = NewObject<UAIOData_Base>( Subsystem, DataClass );
	}

	OnPreDespawn.Broadcast( dataSaveObject );



	// if subject has spawner, notify it about despawn
	if( IsValid( Spawner ) )
	{
		ISpawnerInterface::Execute_OnSubjectDespawnedByOptimizerSubsystem( Spawner, this );
	}

	return dataSaveObject;
}

void UAIOSubjectComponent::PostSpawn( UAIOData_Base* Data )
{
	OnPostSpawned.Broadcast( Data );

	// if subject has spawner, notify it about spawn
	if( IsValid( Spawner ) )
	{
		ISpawnerInterface::Execute_OnSubjectSpawnedByOptimizerSubsystem( Spawner, this );
	}
}










void UAIOSubjectComponent::SetSpawner( TScriptInterface<ISpawnerInterface> NewSpawner )
{
	if( UObject* spawnerObject = NewSpawner.GetObject() )
	{
		MARK_PROPERTY_DIRTY_FROM_NAME( UAIOSubjectComponent, Spawner, this );
		Spawner = Cast<AActor>( spawnerObject );
	}

}


void UAIOSubjectComponent::ReinitializeOptimizationLayers( TArray<FAIOptimizationLayer>& NewOptimizationLayers )
{
	OptimizationLayers = NewOptimizationLayers;

	// update squared values of optimization layers for ease of calculations later on
	for( FAIOptimizationLayer& layer : OptimizationLayers )
	{
		layer.UpdateLayerRadiusSquared();
	}
}

float UAIOSubjectComponent::GetSpawnRadiusSquared( UAIOptimizerSubsystem* Subsystem )
{
	if( OverrideSubsystemSpawnRadiusSquared > -1.f )
	{
		return OverrideSubsystemSpawnRadiusSquared;
	}

	// get despawn radius
	float spawnRadius = GetDespawnRadiusSquared( Subsystem );

	// subtract some percent value from it to get spawn radius ( usually subtract 10% )
	// this is to prevent spawn/despawn when player moves a little back and forward
	spawnRadius -= spawnRadius * UAIOptimizerSubsystem::CHANGE_LAYER_BONUS_RADIUS_PERCENT;


	return spawnRadius;
}

float UAIOSubjectComponent::GetDespawnRadiusSquared( UAIOptimizerSubsystem* Subsystem )
{
	// if system is handling spawning/despawning, then radius specified in it ( unless it is overridden by component )
	if( IsSubsystemAllowedToDespawn() )
	{
		if( OverrideSubsystemDespawnRadius > 0.f )
		{
			return FMath::Square( OverrideSubsystemDespawnRadius );
		}
		else
		{
			return FMath::Square( Subsystem->DESPAWN_RADIUS );
		}
	}
	// if subject is handling optimization by itself, then use radius of last optimization layer
	else
	{
		if( OptimizationLayers.Num() > 0 )
		{
			// get radius of last layer, for example 1000
			float despawnRadius = OptimizationLayers[ OptimizationLayers.Num() - 1 ].GetSquaredLayerRadius();

			// increase that radius by percent specified in subsystem ( usualy 10% )
			despawnRadius += despawnRadius * UAIOptimizerSubsystem::CHANGE_LAYER_BONUS_RADIUS_PERCENT;
			return despawnRadius;
		}
		else
		{
			return -1.f;
		}
	}
}

void UAIOSubjectComponent::UpdateDataToInvokers( UAIOptimizerSubsystem* Subsystem )
{
	// cache location of owner for ease of use
	FVector ownerLocation = GetOwner()->GetActorLocation();

	float smallestDotProductToInvoker = -2.f;
	float closestSquaredDistanceToInvoker = FLT_MAX;

	for( int32 i = Subsystem->Invokers.Num() - 1; i >= 0; i-- )
	{
		// cache invoker location for ease of use
		FVector invokerLocation = Subsystem->Invokers[ i ].GetActorLocation();

		// Get distance to current invoker and check if it is closer than what we have
		float currentInvokerDistanceSquared = UKismetMathLibrary::Vector_DistanceSquared( ownerLocation, invokerLocation );
		if( currentInvokerDistanceSquared < closestSquaredDistanceToInvoker )
		{
			closestSquaredDistanceToInvoker = currentInvokerDistanceSquared;
			ClosestInvokerLocation = invokerLocation;
		}

		// Get angle to current invoker and check if it is smaller than what we have
		if( ShouldCalculateIsSeen() )
		{
			FRotator invokerViewRotation = FRotator::ZeroRotator;
			FVector invokerViewLocation = FVector::ZeroVector;
			Subsystem->Invokers[ i ].GetActor()->GetActorEyesViewPoint( invokerViewLocation, invokerViewRotation );

			FVector invokerViewDirection = UKismetMathLibrary::Conv_RotatorToVector( invokerViewRotation );
			FVector directionToSubject = ( ownerLocation - invokerLocation ).GetUnsafeNormal();

			float currentDot = UKismetMathLibrary::Dot_VectorVector( invokerViewDirection, directionToSubject );
			if( currentDot > smallestDotProductToInvoker )
			{
				smallestDotProductToInvoker = currentDot;
			}
		}
	}

	// cache distance to closest invoker, to be able to use it elsewhere
	DistanceToClosestInvokerSquared = closestSquaredDistanceToInvoker;
	DotProductToClosestInvoker = smallestDotProductToInvoker;
}



void UAIOSubjectComponent::UpdateOptimizationLayer()
{
	bool bNewIsSeen = !ShouldCalculateIsSeen() || DotProductToClosestInvoker >= UAIOptimizerSubsystem::PERIPHERAL_VISION_HALF_ANGLE_COS;
	int32 newLayer = GetOptimizationLayerForCurrentDistance();

	// check if anything has changed and if there is need to call an update
	if( newLayer != CurrentOptimizationLayer ||
		bNewIsSeen != bIsSeenByAnyInvoker )
	{
		// update cached values
		bIsSeenByAnyInvoker = bNewIsSeen;
		CurrentOptimizationLayer = newLayer;

		// broadcast event to notify that there is need for optimization update, so subject can react
		bool bIsBeyondLastLayer = CurrentOptimizationLayer == INDEX_NONE;
		OnOptimizationUpdate.Broadcast( bIsBeyondLastLayer, CurrentOptimizationLayer, bIsSeenByAnyInvoker );
	}
}

int32 UAIOSubjectComponent::GetOptimizationLayerForCurrentDistance()
{
	// at first assume that subject is not in any layer ( INDEX_NONE )
	int32 newLayer = INDEX_NONE;

	// loop through all available layers from end to start because they are sorted from lowest to highest
	for( int32 i = OptimizationLayers.Num() - 1; i >= 0; i-- )
	{
		// cache squared radius of checked layer
		float comparedDistance = OptimizationLayers[ i ].GetSquaredLayerRadius();

		// if this is layer that subject is currently IN, then add bonus radius ( by default 10% )
		if( CurrentOptimizationLayer == i )
		{
			comparedDistance += comparedDistance * UAIOptimizerSubsystem::CHANGE_LAYER_BONUS_RADIUS_PERCENT; // bonus percent radius is specified on subsystem
		}

		// compare with distance to closest invoker and if subject is within radius, update new layer
		if( DistanceToClosestInvokerSquared < comparedDistance )
		{
			newLayer = i;
		}
	}

	return newLayer;
}






void UAIOSubjectComponent::SetCharacterFeatures_Implementation( ACharacter* Character, int32 FeaturesToEnable )
{
	check( Character );

	bool bHasAuthority = Character->HasAuthority();

	// AI BRAIN ( BEHAVIOR TREE )
	if( TEST_BIT( FeaturesToEnable, EAIOFeaturesFlags::AIBrain ) )
	{
		if( bHasAuthority )
			UAIOBPLibrary::SetAILogicEnabled( Character, true );
	}
	else
	{
		if( bHasAuthority )
			UAIOBPLibrary::SetAILogicEnabled( Character, false );
	}
	//////////////////////////////////////////////////////////////////////////

	// ANIMATIONS
	if( TEST_BIT( FeaturesToEnable, EAIOFeaturesFlags::Animations ) )
	{
		Character->GetMesh()->bPauseAnims = false;
		Character->GetMesh()->SetComponentTickInterval( 0.f );
	}
	else
	{
		Character->GetMesh()->bPauseAnims = true;
		Character->GetMesh()->SetComponentTickInterval( 0.5f );
	}
	//////////////////////////////////////////////////////////////////////////

	// COLLISION
	if( TEST_BIT( FeaturesToEnable, EAIOFeaturesFlags::Collision ) )
	{
		Character->SetActorEnableCollision( true );
	}
	else
	{
		Character->SetActorEnableCollision( false );
	}
	//////////////////////////////////////////////////////////////////////////

	// MOVEMENT COMPONENT
	if( TEST_BIT( FeaturesToEnable, EAIOFeaturesFlags::MovementComponent ) )
	{
		UAIOBPLibrary::SetCharacterMovementEnabled( Character, true );
	}
	else
	{
		UAIOBPLibrary::SetCharacterMovementEnabled( Character, false );
	}
	//////////////////////////////////////////////////////////////////////////

	// SHADOWS
	if( TEST_BIT( FeaturesToEnable, EAIOFeaturesFlags::Shadows ) )
	{
		Character->GetMesh()->SetCastShadow( true );
	}
	else
	{
		Character->GetMesh()->SetCastShadow( false );
	}
	//////////////////////////////////////////////////////////////////////////

	// ACTOR TICK
	if( TEST_BIT( FeaturesToEnable, EAIOFeaturesFlags::ActorTick ) )
	{
		Character->SetActorTickEnabled( true );
	}
	else
	{
		Character->SetActorTickEnabled( false );
	}
	//////////////////////////////////////////////////////////////////////////

	// VISIBILITY
	if( TEST_BIT( FeaturesToEnable, EAIOFeaturesFlags::Visibility ) )
	{
		Character->SetActorHiddenInGame( false );
	}
	else
	{
		Character->SetActorHiddenInGame( true );
	}
	//////////////////////////////////////////////////////////////////////////


	// CLOTH SIMULATION
	if( TEST_BIT( FeaturesToEnable, EAIOFeaturesFlags::Visibility ) )
	{
		Character->GetMesh()->ResumeClothingSimulation();
	}
	else
	{
		Character->GetMesh()->SuspendClothingSimulation();
	}
	//////////////////////////////////////////////////////////////////////////
}






