// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	/** Adding curve slot.
	 * This curve defines duration of an animation states. If you need to change delay after mantling, you should modify this one
	 */ 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeightStartTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionXY = 65.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionZ = 200.0f;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool)

class UBaseCharacterMovementComponent;
class UCharacterEquipmentComponent;
class UCharacterAttributeComponent;
class ULedgeDetectorComponent;
UCLASS(Abstract, NotBlueprintable)
class DOMAIN_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()
	
public:
	//-----------------------------------------MOVEMENT------------------------------------------
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	virtual void Jump() override;
	virtual void Falling() override;
	virtual void NotifyJumpApex() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual bool CanJumpInternal_Implementation() const override;
	
	virtual void MoveForward(float Value){}
	virtual void MoveRight(float Value){}
	virtual void Turn(float Value){}
	virtual void LookUp(float Value){}
	virtual void TurnAtRate(float Value){}
	virtual void LookUpAtRate(float Value){}
	virtual void StartSprint();
	virtual void StopSprint();

	virtual bool CanSprint();
	
	virtual void ChangeCrouchState();

	/** Activates mantling
	 * if ULedgeDetectorComponent() detects a climbable ledge:
	 *	1) Fills mantling params.
	 *	2) Activates BaseCharacterMovementComponent::StartMantle().
	 *	3) Plays selected montage.
	 */
	virtual void Mantle(bool bForce);
	
	//-----------------------------------------Range Weapon------------------------------------------
	void StartFire();
	void StopFire();

	void StartAim();
	void StopAim();

	FOnAimingStateChanged OnAimingStateChanged;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	void Reload() const;
	
	//-----------------------------------------Getters------------------------------------------
	FORCEINLINE UBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return BaseCharacterMovementComponent; }
	FORCEINLINE bool GetIsSprinting() const {return bIsSprinting;}

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;
	bool GetIsAiming() const;
	float GetAimingMovementSpeed() const;
	
	UFUNCTION(BlueprintCallable)
	UCharacterAttributeComponent* GetCharacterAttributeComponent() const;

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();
protected:
	//-----------------------------------------MOVEMENT------------------------------------------
	bool CanMantle() const;

	UFUNCTION(BlueprintImplementableEvent, Category="Character|Movement")
	void OnSprintStart();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Character|Movement")
	void OnSprintEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character|Movement")
	float SprintSpeed = 800.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character|Controls")
	float BaseTurnRate = 45.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character|Controls")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings HighMantleSettings;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings LowMantleSettings;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LowMantleMaxHeight = 125.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Animation")
	class UAnimMontage* OnDeathAnimMontage;

	// Damage depends on fall height in meters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Attributes")
	class UCurveFloat* FallDamageCurve;
	
	virtual void OnDeath();

	virtual void OnOutOfStamina(bool IsOutOfStamina);
	
	UBaseCharacterMovementComponent* BaseCharacterMovementComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCharacterAttributeComponent* CharacterAttributeComponent = nullptr;
	UPROPERTY(EditAnywhere)
	UCharacterEquipmentComponent* CharacterEquipmentComponent = nullptr;
	ULedgeDetectorComponent* LedgeDetectorComponent = nullptr;
private:
	void EnableRagdoll();
	void TryChangeSprintState();
	
	float DefaultMaxMovementSpeed = 0.0f;
	float CurrentAimingMovementSpeed = 0.f;
	bool bIsSprintRequested = false;
	bool bIsSprinting = false;
	bool bCanCrouch = true;
	bool bIsAiming;
	
	FTimerHandle DeathMontageTimer;
	FVector CurrentFallApex;

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;
};

inline const FMantlingSettings& ABaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}