// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved


#include "AIOInvokerComponent.h"
#include "AIOptimizerSubsystem.h"
#include "AIODeveloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "AIOptimizerTypes.h"

// Sets default values for this component's properties
UAIOInvokerComponent::UAIOInvokerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bWantsInitializeComponent = true; // needed to get InitializeComponent
	SetAutoActivate( true );
	SetIsReplicatedByDefault( true );
}

void UAIOInvokerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	if( UWorld* world = GetWorld() )
	{
		if( world->IsGameWorld() )
		{
			if( AActor* owner = GetOwner() )
			{
				owner->Tags.Add( INVOKER_TAG );
			}
		}
	}
}

void UAIOInvokerComponent::OnComponentDestroyed( bool bDestroyingHierarchy )
{
	Super::OnComponentDestroyed( bDestroyingHierarchy );

	UnregisterInvoker();
}

void UAIOInvokerComponent::Activate( bool bReset /*= false */ )
{
	RegisterInvoker();
}


void UAIOInvokerComponent::OnRep_IsActive()
{
	if( IsActive() ) // for CLIENTS
	{
		RegisterInvoker();
	}
}


void UAIOInvokerComponent::RegisterInvoker()
{
	if( UWorld* world = GetWorld() )
	{
		if( world->IsGameWorld() ) // make sure it is runtime, not editor
		{
			if( UAIOptimizerSubsystem* subsystem = world->GetSubsystem<UAIOptimizerSubsystem>() )
			{
				subsystem->RegisterInvoker( this );
			}
		}
	}
}

void UAIOInvokerComponent::UnregisterInvoker()
{
	if( UWorld* world = GetWorld() )
	{
		if( world->IsGameWorld() ) // make sure it is runtime, not editor
		{
			if( UAIOptimizerSubsystem* subsystem = world->GetSubsystem<UAIOptimizerSubsystem>() )
			{
				subsystem->UnregisterInvoker( this );
			}
		}
	}
}


void UAIOInvokerComponent::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay( EndPlayReason );

	DebugAIOptimizer( false );
}

void UAIOInvokerComponent::DebugAIOptimizer( bool bDebug )
{
	if( bDebug )
	{
		if( !DebugWidget ) // check if wasn't already created
		{
			if( APlayerController* PC = UGameplayStatics::GetPlayerController( this, 0 ) )
			{
				DebugWidget = UWidgetBlueprintLibrary::Create( this, UAIODeveloperSettings::Get()->DebugWidgetClass, PC );
				if( DebugWidget )
				{
					DebugWidget->AddToViewport();
				}
			}
		}
	}
	else if( DebugWidget )
	{
		DebugWidget->RemoveFromParent();
		DebugWidget = nullptr;
	}
}



