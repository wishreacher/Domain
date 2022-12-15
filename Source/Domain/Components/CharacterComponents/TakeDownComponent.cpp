// Fill out your copyright notice in the Description page of Project Settings.

#include "Domain/Components/CharacterComponents/TakeDownComponent.h"
#include "CharacterEquipmentComponent.h"
#include "Domain/Components/BaseCharacterMovementComponent.h"

UTakeDownComponent::UTakeDownComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTakeDownComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedBaseCharacter = StaticCast<ABaseCharacter*>(GetOwner());
}

void UTakeDownComponent::BeginTakeDown()
{
	if(!CanExecuteTakeDown()) //checking if we can fire a takedown or not
	{
		return;
	}
	
	UCharacterEquipmentComponent* EquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent();
	if(!IsValid(EquipmentComponent))
	{
		return;
	}
	
	// if(EquipmentComponent->GetCurrentEquippedSlot() != EEquipmentSlots::MeleeWeapon) //equipping the melee weapon if it's not
	// {
	// 	EquipmentComponent->EquipItemInSlot(EEquipmentSlots::MeleeWeapon);
	// }

	if(!CachedBaseCharacter->GetBaseCharacterMovementComponent()->IsCrouching()) //crouching if we aren't
	{
		CachedBaseCharacter->Crouch(true);
	}
	
	if(GetShouldExecuteBehindTakeDown()) //executing takedown
	{
		SetupTarget(); //TODO move the trace to c++
		if(IsValid(BehindTakeDownTarget))
		{
			ExecuteTakeDown(ETakeDownType::Behind, BehindTakeDownTarget);
		}
	}
	if(GetShouldExecuteTopTakeDown())
	{
		if(IsValid(TopTakeDownTarget))
		{
			ExecuteTakeDown(ETakeDownType::Top, TopTakeDownTarget);
		}
	}
}

bool UTakeDownComponent::CanExecuteTakeDown() const
{
	//TODO check if the weapon has corresponding takedowns
	// AMeleeWeapon* WeaponRef = Cast<AMeleeWeapon>(GetCharacterEquipmentComponent()->GetCurrentLoadout().Find(EEquipmentSlots::MeleeWeapon));
	if(!bShouldExecuteBehindTakeDown && !bShouldExecuteTopTakeDown)
	{
		return false;
	}
	return !CachedBaseCharacter->GetCharacterEquipmentComponent()->GetIsEquipping() && !bIsExecutingTakeDown;
}

void UTakeDownComponent::ExecuteTakeDown(ETakeDownType Type, AActor* Target)
{
	// UCharacterEquipmentComponent* EquipmentComponent = CachedBaseCharacter->GetCharacterEquipmentComponent();
	// AMeleeWeapon* WeaponHardRef = EquipmentComponent->GetCurrentMeleeWeapon();
	//
	// TArray<FTakeDownDescription> AllAvailableTakeDowns = WeaponHardRef->GetTakeDowns(); //Gathering the array of possible takedowns
	// CachedBaseCharacter->ToggleControls(false); //disabling movement
	//
	// bIsExecutingTakeDown = true;
	// CachedBaseCharacter->SetCanAttack(false);
	//
	// TArray<FTakeDownDescription> ChosenTakeDowns;
	// for(FTakeDownDescription TakeDownDescription : WeaponHardRef->GetTakeDowns()) //chosing what animations should we play based on takedown type
	// 	{
	// 	if(TakeDownDescription.TakeDownType == Type)
	// 	{
	// 		//TODO AddUnique doesn't work for some reason
	// 		ChosenTakeDowns.Add(TakeDownDescription);
	// 	}
	// 	}
	//
	// FTakeDownDescription RequestedTakeDown = ChosenTakeDowns[FMath::RandRange(0, ChosenTakeDowns.Num() - 1)]; //choosing one particular takedown
	// MoveToTakeDownTarget(Target, RequestedTakeDown.Offset); //moving behind the target with offset
	// PlayTakeDownMontages(RequestedTakeDown); //playing animation montages on player
	// EnemyTakeDownInterfaceCall(Target, RequestedTakeDown); //calling the enemy takedown interface
	// TakeDownCameraRigActivation(RequestedTakeDown); //camera actions
	//
	// GetWorld()->GetTimerManager().ClearTimer(TakeDownTimerHandle); //setting up the timer event that fires off when takedown is close to an end
	// GetWorld()->GetTimerManager().SetTimer(TakeDownTimerHandle, this, &UTakeDownComponent::TakeDownEnd,
	// 	RequestedTakeDown.PlayerTakeDownMontage->GetPlayLength() - 0.2, false);
}

void UTakeDownComponent::TakeDownEnd_Implementation()
{
	CachedBaseCharacter->ToggleControls(true);
	bIsExecutingTakeDown = false;
	CachedBaseCharacter->SetCanAttack(true);
}