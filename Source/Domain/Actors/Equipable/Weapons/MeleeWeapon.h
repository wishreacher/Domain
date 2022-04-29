// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Domain/Actors/Equipable/EquippableItem.h"
#include "Domain/Components/ActorComponents/Weapon/MeleeHitRegistrator.h"
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

USTRUCT(BlueprintType)
struct FTakeDownDescription
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Takedown")
	UAnimMontage* PlayerTakeDownMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Takedown")
	UAnimMontage* EnemyTakeDownMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Takedown")
	float BehindOffset = 170.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Takedown")
	float LeftOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Takedown")
	FName EquippedSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Takedown")
	ETakeDownType TakeDownType = ETakeDownType::Behind;
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

	void SetIsHitRegistrationEnabled(bool bIsRegistrationEnabled);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee attack")
	TMap<EMeleeAttackType, FMeleeAttackDescription> Attacks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Takedown")
	TArray<FTakeDownDescription> TakeDowns;

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& HitDirection);

	TArray<UMeleeHitRegistrator*> HitRegistrators;
	TSet<AActor*> HitActors;

	FMeleeAttackDescription* CurrentAttack;
	void OnAttackTimerElapsed();
	FTimerHandle AttackTimer;
};
