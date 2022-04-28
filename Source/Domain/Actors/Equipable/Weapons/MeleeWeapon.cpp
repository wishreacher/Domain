// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Actors/Equipable/Weapons/MeleeWeapon.h"
#include "Domain/Character/Animations/BaseCharacterAnimInstance.h"

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
	CharacterOwner->SetIsAttacking(true);

	HitActors.Empty();
	CurrentAttack = Attacks.Find(AttackType);
	if (CurrentAttack && IsValid(CurrentAttack->AttackMontage))
	{
		UBaseCharacterAnimInstance* CharacterAnimInstance = Cast<UBaseCharacterAnimInstance>(CharacterOwner->GetMesh()->GetAnimInstance());
		if (IsValid(CharacterAnimInstance))
		{
			CharacterAnimInstance->PlayComboMontage(AttackType);
			// GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeapon::OnAttackTimerElapsed, Duration, false);
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

	DrawDebugSphere(GetWorld(), HitResult.Location, 10.f, 6.f, FColor::Emerald, false, 2.f);
}

void AMeleeWeapon::OnAttackTimerElapsed()
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	CurrentAttack = nullptr;
	CharacterOwner->SetIsAttacking(false);
	SetIsHitRegistrationEnabled(false);
}
