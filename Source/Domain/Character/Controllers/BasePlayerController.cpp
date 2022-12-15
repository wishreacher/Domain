// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"

void ABasePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<ABaseCharacter>(InPawn);
	CreateAndInitializeWidgets();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("Move Forward / Backward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("Move Right / Left", this, &ABasePlayerController::MoveRight);
	InputComponent->BindAxis("Turn Right / Left Mouse", this, &ABasePlayerController::Turn);
	InputComponent->BindAxis("Look Up / Down Mouse", this, &ABasePlayerController::LookUp);
	InputComponent->BindAxis("Turn Right / Left Gamepad", this, &ABasePlayerController::TurnAtRate);
	InputComponent->BindAxis("Look Up / Down Gamepad", this, &ABasePlayerController::LookUpAtRate);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &ABasePlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ABasePlayerController::Jump);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ABasePlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABasePlayerController::StopSprint);
	InputComponent->BindAction("Takedown", EInputEvent::IE_Pressed, this, &ABasePlayerController::TakeDown);
}

void ABasePlayerController::MoveForward(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		if(Value < 0)
		{
			CachedBaseCharacter->StopSprint();
		}
		CachedBaseCharacter->MoveForward(Value);
	}
}
void ABasePlayerController::MoveRight(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}
void ABasePlayerController::Turn(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}
void ABasePlayerController::LookUp(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}
void ABasePlayerController::Jump()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void ABasePlayerController::ChangeCrouchState()
{
	CachedBaseCharacter->ChangeCrouchState();
}

void ABasePlayerController::StartSprint()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void ABasePlayerController::StopSprint()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void ABasePlayerController::TakeDown()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartTakeDown();
	}
}

void ABasePlayerController::CreateAndInitializeWidgets()
{
	if(!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if(IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}
	}
}

void ABasePlayerController::TurnAtRate(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnAtRate(Value);
	}
}
void ABasePlayerController::LookUpAtRate(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}

void ABasePlayerController::Mantle()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle(false);
	}
}