// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Actors/Equipable/Weapons/MeleeWeapon.h"

AMeleeWeapon::AMeleeWeapon()
{
	EquippedSocketName = SocketCharacterWeapon;
}

void AMeleeWeapon::StartAttack(EMeleeAttackType AttackType)
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	if(!bCanAttack)
	{
		return;
	}
	bCanAttack = false;

	HitActors.Empty();
	CurrentAttack = Attacks.Find(AttackType);
	if (CurrentAttack && IsValid(CurrentAttack->AttackMontage))
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if (IsValid(CharacterAnimInstance))
		{
			float Duration = CharacterAnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1.0f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeapon::OnAttackTimerElapsed, Duration, false);
		}
		else
		{
			OnAttackTimerElapsed();
		}
	}
}

void AMeleeWeapon::SetIsHitRegistrationEnabled(bool bIsRegistrationEnabled)
{
	HitActors.Empty();
	for (UMeleeHitRegistrator* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->SetIsHitRegistrationEnabled(bIsRegistrationEnabled);
	}
}

void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UMeleeHitRegistrator>(HitRegistrators);
	for (UMeleeHitRegistrator* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->OnMeleeHitRegistered.AddDynamic(this, &AMeleeWeapon::ProcessHit);
	}
}

void AMeleeWeapon::ProcessHit(const FHitResult& HitResult, const FVector& HitDirection)
{
	if (CurrentAttack == nullptr)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!IsValid(HitActor))
	{
		return;
	}

	if (HitActors.Contains(HitActor))
	{
		return;
	}

	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = HitResult;
	DamageEvent.ShotDirection = HitDirection;
	DamageEvent.DamageTypeClass = CurrentAttack->DamageType;

	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	AController* Controller = IsValid(CharacterOwner) ? CharacterOwner->GetController<AController>() : nullptr;
	HitActor->TakeDamage(CurrentAttack->DamageAmount, DamageEvent, Controller, GetOwner());

	HitActors.Add(HitActor);
}

void AMeleeWeapon::OnAttackTimerElapsed()
{
	bCanAttack = true;
	CurrentAttack = nullptr;
	SetIsHitRegistrationEnabled(false);
}
