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
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ABasePlayerController::StopFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartAim);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ABasePlayerController::StopAim);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ABasePlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &ABasePlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &ABasePlayerController::PreviousItem);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABasePlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABasePlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction("EquipSideArmWeapon", EInputEvent::IE_Pressed, this, &ABasePlayerController::EquipSideArmWeapon);
	InputComponent->BindAction("EquipPrimaryWeapon", EInputEvent::IE_Pressed, this, &ABasePlayerController::EquipPrimaryWeapon);
	InputComponent->BindAction("EquipSecondaryWeapon", EInputEvent::IE_Pressed, this, &ABasePlayerController::EquipSecondaryWeapon);
	InputComponent->BindAction("EquipMeleeWeapon", EInputEvent::IE_Pressed, this, &ABasePlayerController::EquipMeleeWeapon);
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

void ABasePlayerController::NextItem()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void ABasePlayerController::PreviousItem()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void ABasePlayerController::PrimaryMeleeAttack()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void ABasePlayerController::SecondaryMeleeAttack()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void ABasePlayerController::EquipSideArmWeapon()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipSideArmWeapon();
	}
}

void ABasePlayerController::EquipPrimaryWeapon()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryWeapon();
	}
}

void ABasePlayerController::EquipSecondaryWeapon()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipSecondaryWeapon();
	}
}

void ABasePlayerController::EquipMeleeWeapon()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipMeleeWeapon();
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
	if(IsValid(PlayerHUDWidget) && CachedBaseCharacter.IsValid() )
	{
		UCrosshairWidget* CrosshairWidget = PlayerHUDWidget->GetCrosshairWidget();
		if(IsValid(CrosshairWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
			CachedBaseCharacter->OnAimingStateChanged.AddUFunction(CrosshairWidget, FName("OnAimingStateChanged"));
			CharacterEquipment->OnEquippedItemChanged.AddUFunction(CrosshairWidget, FName("OnEquippedItemChanged"));
		}

		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
		if(IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
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