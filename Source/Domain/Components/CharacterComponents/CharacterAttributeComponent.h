// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Character/BaseCharacter.h"
#include "CharacterAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaSignature, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOMAIN_API UCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsAlive() const { return Health > 0.0f;}

	void UpdateStaminaValue(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;
	
	UFUNCTION(BlueprintCallable)
	float GetStaminaPercent() const;

	FOnDeathEventSignature OnDeathEvent;
	FOutOfStaminaSignature OnOutOfStaminaEvent;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.0f))
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Movement")
	float MaxStamina = 20.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Movement")
	float StaminaRestoreVelocity = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Movement")
	float StaminaConsumptionVelocity = 3.0f;
	
private:
	float Health = 0.0f;
	float Stamina = 0.0f;
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TWeakObjectPtr<class ABaseCharacter> CachedBaseCharacterOwner;
};
