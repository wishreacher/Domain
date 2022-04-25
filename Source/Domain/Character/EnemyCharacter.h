// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Domain/Character/BaseCharacter.h"
#include "EnemyCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DOMAIN_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	
public:
	AEnemyCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void OnDeath() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ProcessDeath();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* TakeDownCollision;
};
