// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Actors/Equipable/Weapons/Throwable.h"
#include "Domain/Character/BaseCharacter.h"

void AThrowable::Throw()
{
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("AThrowable::Throw() only character can be an owner of range weapon"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if(!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);

	FVector SpawnLocation = PlayerViewPoint + ViewDirection * 100.f;

	ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);
	if(IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(ViewDirection);
	}
}
