// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Domain/Components/CharacterComponents/TakeDownComponent.h"
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
	virtual void StartSprint() override;
	virtual void StopSprint() override;
	virtual void StartTakeDown() override;

	virtual void OnDeath() override;
	
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	void SetAimCameraFOV();

	UFUNCTION(BlueprintNativeEvent)
	void SetDefaultCameraFOV();

	FORCEINLINE UCameraShakeSourceComponent* GetCameraShakeComponent() const {return CameraShakeComponent;}

	FORCEINLINE UTakeDownComponent* GetTakeDownComponent() const {return TakeDownComponent;}
	
	virtual void ToggleControls(bool bShouldEnableControl) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class UCameraComponent* CameraComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class USpringArmComponent* SpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	UCameraShakeSourceComponent* CameraShakeComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	UTakeDownComponent* TakeDownComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TSubclassOf<UTakeDownComponent> TakeDownComponentClass;

private:
	TSubclassOf<UCameraShakeBase> CurrentCameraShakeClass;
	// FTimeline ScopeTimeLine;
};
