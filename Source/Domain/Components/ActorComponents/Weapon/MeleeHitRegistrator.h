// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MeleeHitRegistrator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMeleeHitRegistered, const FHitResult&, HitResult, const FVector&, Direction);
/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class DOMAIN_API UMeleeHitRegistrator : public USphereComponent
{
	GENERATED_BODY()

public:
	UMeleeHitRegistrator();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ProcessHit();

	void SetIsHitRegistrationEnabled(bool bIsEnabled_In);

	FOnMeleeHitRegistered OnMeleeHitRegistered;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee Hit Registration")
	bool bIsHitRegistrationEnabled = false;
private:
	FVector PreviousComponentLocation = FVector::ZeroVector;
	
};
