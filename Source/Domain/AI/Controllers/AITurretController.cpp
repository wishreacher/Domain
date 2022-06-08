// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/AI/Controllers/AITurretController.h"
#include "Domain/AI/Characters/Turret.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"

void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if(IsValid(InPawn))
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn AITurretController can possess only turrets"))
		CachedTurret = StaticCast<ATurret*>(InPawn);
	} else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if(!CachedTurret.IsValid())
	{
		return;
	}
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	CachedTurret->SetCurrentTarget(ClosestActor);
}

float AAITurretController::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
