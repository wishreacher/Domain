// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DOMAIN_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;
	virtual void TurnAtRate(float Value) override;
	virtual void LookUpAtRate(float Value) override;

	virtual void OnDeath() override;
	
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	// UFUNCTION()
	// void TimelineFloatReturn(float Value) const;
	
	float GetAimTurnModifier() const;
	float GetAimLookUpModifier() const;

	virtual void OnStartAimingInternal() override;
	virtual void OnStopAimingInternal() override;

	UFUNCTION()
	void OnWeaponReloadBegin();

	UFUNCTION(BlueprintNativeEvent)
	void SetAimCameraFOV();

	UFUNCTION(BlueprintNativeEvent)
	void SetDefaultCameraFOV();

	FORCEINLINE UCameraShakeSourceComponent* GetCameraShakeComponent() const {return CameraShakeComponent;}

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class UCameraComponent* CameraComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class USpringArmComponent* SpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	UCameraShakeSourceComponent* CameraShakeComponent = nullptr;

	// FTimeline ScopeTimeLine;
};
