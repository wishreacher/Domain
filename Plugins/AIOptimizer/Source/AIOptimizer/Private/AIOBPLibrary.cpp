// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#include "AIOBPLibrary.h"
#include "AIOptimizer.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIOSubjectComponent.h"

int32 UAIOBPLibrary::AddUniqueHandle( TArray<FAIOSubjectHandle>& Array, const FAIOSubjectHandle& Handle )
{
	int32 index = Array.IndexOfByKey( Handle );
	if( index == INDEX_NONE )
	{
		index = Array.Add( Handle );
		return index;
	}

	// if that handle is already in given Array then return INDEX_NONE
	return INDEX_NONE;
}


bool UAIOBPLibrary::RemoveHandle( TArray<FAIOSubjectHandle>& Array, const FAIOSubjectHandle& Handle )
{
	int32 foundIndex = Array.IndexOfByKey( Handle );
	if( foundIndex != INDEX_NONE )
	{
		Array.RemoveAt( foundIndex );
		return true;
	}
	return false;
}

void UAIOBPLibrary::SetAILogicEnabled( AActor* Actor, bool bEnable )
{
	check( Actor );

	if( AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController( Actor ) )
	{
		AIController->SetActorTickEnabled( bEnable );
		AIController->GetPathFollowingComponent()->SetComponentTickEnabled( bEnable );

		if( UBrainComponent* brainComponent = AIController->GetBrainComponent() )
		{
			if( bEnable )
			{
				brainComponent->RestartLogic();
			}
			else
			{
				brainComponent->StopLogic( TEXT( "AIO Library" ) );
			}
		}
	}
}

void UAIOBPLibrary::SetCharacterMovementEnabled( ACharacter* Character, bool bEnable )
{
	check( Character );

	if( bEnable )
	{
		Character->GetCharacterMovement()->Activate();
		Character->GetCharacterMovement()->SetComponentTickEnabled( true );
	}
	else
	{
		Character->GetCharacterMovement()->Deactivate();
		Character->GetCharacterMovement()->SetComponentTickEnabled( false );
	}
}



