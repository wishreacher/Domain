// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Domain/Actors/Equipable/EquippableItem.h"
#include "MeleeWeapon.generated.h"

USTRUCT(BlueprintType)
struct FMeleeAttackDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack")
	TSubclassOf<class UDamageType> DamageType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Attack", meta = (ClampMin = 0.f, UIMin = 0.f))
	float DamageAmount = 50.f;
	
};

/**
 * 
 */
UCLASS(Blueprintable)
class DOMAIN_API AMeleeWeapon : public AEquippableItem
{
	GENERATED_BODY()

public:
	AMeleeWeapon();

	void StartAttack(EMeleeAttackType AttackType);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
	TMap<EMeleeAttackType, FMeleeAttackDescription> Attacks;

private:
	FMeleeAttackDescription* CurrentAttack;
	
	void OnAttackTimerElapsed();
	FTimerHandle AttackTimer;
};
