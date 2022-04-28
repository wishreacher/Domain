// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Character/EnemyCharacter.h"
#include "Components/BoxComponent.h"


AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	TakeDownCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TakeDownCollision"));
	TakeDownCollision->InitBoxExtent(FVector(32.f, 32.f, 83.f));
	TakeDownCollision->SetupAttachment(RootComponent);

	TopTakeDownCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TopTakeDownCollision"));
	TopTakeDownCollision->InitBoxExtent(FVector(32.f, 32.f, 83.f));
	TopTakeDownCollision->SetupAttachment(RootComponent);
}

void AEnemyCharacter::OnDeath()
{
	Super::OnDeath();
	ProcessDeath();
}
