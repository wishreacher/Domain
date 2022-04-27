// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/BaseCharacterMovementComponent.h"
#include "Domain/Actors/Equipable/Weapons/RangeWeapon.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"

void UBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	CachedBaseCharacter = StaticCast<ABaseCharacter*>(TryGetPawnOwner());
	checkf(TryGetPawnOwner()->IsA<ABaseCharacter>(), TEXT("UBaseCharacterAnimInstance::NativeBeginPlay() can be used with base character only"));
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if(!CachedBaseCharacter.IsValid())
	{
		return;
	}

	UBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovement->Velocity.Size();
	Direction = CalculateDirection(CharacterMovement->Velocity, CachedBaseCharacter->GetActorRotation());
	AimRotation = CachedBaseCharacter->GetBaseAimRotation();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsOutOfStamina = CharacterMovement->IsOutOfStamina();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsAiming = CachedBaseCharacter->GetIsAiming();

	const UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurrentEquippedItem = CharacterEquipment->GetCurrentEquippedItem();

	ARangeWeapon* CurrentRangeWeapon = CharacterEquipment->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		ForeGripSocketTransform = CurrentRangeWeapon->GetForeGripTransform();
	} else
	{
		ForeGripSocketTransform = FTransform::Identity;
	}
}

void UBaseCharacterAnimInstance::PlayComboMontage(EMeleeAttackType AttackType)
{
	if(!IsValid(ComboAttackMontage))
	{
		return;
	}
	
	switch(AttackType)
	{
	case (uint8)EMeleeAttackType::PrimaryAttack:
		{
			CachedBaseCharacter->PlayAnimMontage(ComboAttackMontage, 1, LightComboAttack);
			break;
		}
	case (uint8)EMeleeAttackType::SecondaryAttack:
		{
			CachedBaseCharacter->PlayAnimMontage(ComboAttackMontage, 1, HeavyComboAttack);
			break;
		}
	default:
		break;
	}
}

void UBaseCharacterAnimInstance::SetDefaultCombos()
{
	LightComboAttack = DefaultLightComboAttack;
	HeavyComboAttack = DefaultHeavyComboAttack;
}

void UBaseCharacterAnimInstance::SetNextComboSegment(FName LightCombo, FName HeavyCombo)
{
	LightComboAttack = LightCombo;
	HeavyComboAttack = HeavyCombo;
}