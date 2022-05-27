// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"
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
	void ExecuteTakeDown(ETakeDownType Type, AActor* Target);

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
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetShouldExecuteBehindTakeDown() const {return bShouldExecuteBehindTakeDown;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetShouldExecuteTopTakeDown() const {return bShouldExecuteTopTakeDown;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetShouldExecuteBehindCooldown(bool bNewShouldExecute) {bShouldExecuteBehindTakeDown = bNewShouldExecute;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetShouldExecuteTopCooldown(bool bNewShouldExecute) {bShouldExecuteTopTakeDown = bNewShouldExecute;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetBehindTakeDownTarget(AActor* NewBehindTakeDownTarget) {BehindTakeDownTarget = NewBehindTakeDownTarget;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTopTakeDownTarget(AActor* NewTopTakeDownTarget) {TopTakeDownTarget = NewTopTakeDownTarget;}

	bool CanExecuteTakeDown() const;

	UFUNCTION(BlueprintCallable)
	void ToggleControls(bool bShouldEnableControl);

	UFUNCTION(BlueprintImplementableEvent)
	void SetupTarget();
	
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToTakeDownTarget(AActor* TargetActor, FVector Offset);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayTakeDownMontages(FTakeDownDescription TakeDownDescription);

	UFUNCTION(BlueprintImplementableEvent)
	void EnemyTakeDownInterfaceCall(AActor* Target, FTakeDownDescription TakeDown);

	UFUNCTION(BlueprintImplementableEvent)
	void TakeDownCameraRigActivation(FTakeDownDescription TakeDown);

	UFUNCTION(BlueprintNativeEvent)
	void TakeDownEnd();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class UCameraComponent* CameraComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class USpringArmComponent* SpringArmComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	UCameraShakeSourceComponent* CameraShakeComponent = nullptr;

private:
	TSubclassOf<UCameraShakeBase> CurrentCameraShakeClass;

	bool bIsExecutingTakeDown;
	bool bShouldExecuteBehindTakeDown;
	bool bShouldExecuteTopTakeDown;

	FTimerHandle TakeDownTimerHandle;
	
	AActor* BehindTakeDownTarget;
	AActor* TopTakeDownTarget;
	// FTimeline ScopeTimeLine;
};
