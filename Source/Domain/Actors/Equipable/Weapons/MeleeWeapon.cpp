// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Actors/Equipable/Weapons/MeleeWeapon.h"

AMeleeWeapon::AMeleeWeapon()
{
	EquippedSocketName = SocketCharacterWeapon;
}

void AMeleeWeapon::StartAttack(EMeleeAttackType AttackType)
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	CurrentAttack =  Attacks.Find(AttackType);
	if(CurrentAttack && IsValid(CurrentAttack->AttackMontage))
	{
		UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if(IsValid(AnimInstance))
		{
			float Duration = AnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1.f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeapon::OnAttackTimerElapsed, Duration, false);
		}
	} else
	{
		OnAttackTimerElapsed();
	}
}

void AMeleeWeapon::OnAttackTimerElapsed()
{
	CurrentAttack = nullptr;
}
