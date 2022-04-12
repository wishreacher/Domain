// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Domain/Types.h"
#include "Animation/AnimInstance.h"
#include "BaseCharacterAnimInstance.generated.h"

UCLASS()
class DOMAIN_API UBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Animation")
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Animation")
	float Direction = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character|Animation")
	bool bIsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character|Controls")
	bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character|Controls")
	bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Animation")
	bool bIsOutOfStamina = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Animation")
	bool bIsSwimming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character|Animation")
	FRotator AimRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Animation")
	EEquipableItemType CurrentEquippedItem = EEquipableItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character|Animation")
	FTransform ForeGripSocketTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Character|Animation")
	bool bIsAiming; 
private:
	TWeakObjectPtr<class ABaseCharacter> CachedBaseCharacter;
};
