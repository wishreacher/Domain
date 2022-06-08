// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/AI/Controllers/BaseAIController.h"

#include "Perception/AIPerceptionComponent.h"

ABaseAIController::ABaseAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

AActor* ABaseAIController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const
{
	if(!IsValid(GetPawn()))
	{
		return nullptr;
	}
	
	TArray<AActor*> SensedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SensedActors);
	
	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;
	FVector PawnLocation = GetPawn()->GetActorLocation();
	
	for(AActor* SensedActor : SensedActors)
	{
		float CurrentSquaredDistance = FVector::DistSquared(PawnLocation, SensedActor->GetActorLocation());
		if(CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = SensedActor;
		}
	}
	return ClosestActor;
}


