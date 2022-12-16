// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/BaseCharacterMovementComponent.h"

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
}

void UBaseCharacterAnimInstance::PlayComboMontage(EMeleeAttackType AttackType)
{
	// UAnimMontage* ComboMontage = CachedBaseCharacter->GetCharacterEquipmentComponent()->GetCurrentMeleeWeapon()->GetCurrentAttack()->AttackMontage;
	// if(!IsValid(ComboMontage))
	// {
	// 	return;
	// }
	//
	// switch(AttackType)
	// {
	// case (uint8)EMeleeAttackType::PrimaryAttack:
	// 	{
	// 		CachedBaseCharacter->PlayAnimMontage(ComboMontage,1, LightComboAttack);
	// 		break;
	// 	}
	// case (uint8)EMeleeAttackType::SecondaryAttack:
	// 	{
	// 		CachedBaseCharacter->PlayAnimMontage(ComboMontage, 1, HeavyComboAttack);
	// 		break;
	// 	}
	// default:
	// 	break;
	// }
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