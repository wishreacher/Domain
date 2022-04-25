// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Components/ActorComponents/Weapon/MeleeHitRegistrator.h"
#include "Domain/Types.h"
#include "Domain/Actors/Equipable/Weapons/MeleeWeapon.h"
#include "Domain/Utils/TraceUtils.h"

UMeleeHitRegistrator::UMeleeHitRegistrator()
{
	PrimaryComponentTick.bCanEverTick = true;
	SphereRadius = 5.f;
	SetCollisionProfileName(CollisionProfileNoCollision);
}

void UMeleeHitRegistrator::TickComponent(float DeltaTime, ELevelTick Tick,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, Tick, ThisTickFunction);
	PreviousComponentLocation = GetComponentLocation();
	if(bIsHitRegistrationEnabled)
	{
		ProcessHit();
	}
}

void UMeleeHitRegistrator::ProcessHit()
{
	FVector CurrentLocation = GetComponentLocation();
	FHitResult HitResult;

	bool bHasHit = TraceUtils::SweepSphereSingleByChanel(GetWorld(),
		HitResult,
		PreviousComponentLocation,
		CurrentLocation,
		GetScaledSphereRadius(),
		ECC_Melee,
		FCollisionQueryParams::DefaultQueryParam,
		FCollisionResponseParams::DefaultResponseParam,
		true,
		5.f,
		FColor::Black,
		FColor::Red
		);
	if(bHasHit)
	{
		FVector Direction = (CurrentLocation - PreviousComponentLocation).GetSafeNormal();
		if(OnMeleeHitRegistered.IsBound())
		{
			OnMeleeHitRegistered.Broadcast(HitResult, Direction);
		}
	}
}

void UMeleeHitRegistrator::SetIsHitRegistrationEnabled(bool bIsEnabled_In)
{
	bIsHitRegistrationEnabled = bIsEnabled_In;
}
