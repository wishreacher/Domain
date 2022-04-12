// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Components/ActorComponents/ExplosionComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

void UExplosionComponent::Explode(AController* Controller)
{
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		MaxDamage,
		MinDamage,
		GetComponentLocation(),
		InnerRadius,
		OuterRadius,
		DamageFallOff,
		DamageTypeClass,
		ActorsToIgnore,
		GetOwner(),
		Controller,
		ECC_Visibility // TODO maybe create another trace channel. Now all of the object that we see can prevent damage behind, if staying in the line of it. So if you need some objects to let damage trough, you need to specify it in the trace channel
		);
	
	if(OnExplosion.IsBound())
	{
		OnExplosion.Broadcast();
	}
	if(IsValid(ExplosionVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetComponentLocation());
	}
}
