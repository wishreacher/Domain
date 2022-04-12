// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "Domain/Character/BaseCharacter.h"

void ABasePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<ABaseCharacter>(InPawn);
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABasePlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &ABasePlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ABasePlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &ABasePlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &ABasePlayerController::LookUpAtRate);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &ABasePlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ABasePlayerController::Jump);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ABasePlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABasePlayerController::StopSprint);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ABasePlayerController::StopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartAim);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ABasePlayerController::StopAim);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ABasePlayerController::Reload);
}

void ABasePlayerController::MoveForward(float Value)
{
	if(CachedBaseCharacter.IsValid())
	{
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

void ABasePlayerController::StartFire()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void ABasePlayerController::StopFire()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void ABasePlayerController::StartAim()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAim();
	}
}

void ABasePlayerController::StopAim()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAim();
	}
}

void ABasePlayerController::Reload()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
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
