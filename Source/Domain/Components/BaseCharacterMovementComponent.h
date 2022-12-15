// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Domain/Character/BaseCharacter.h"
#include "BaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;
	
	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation  = FVector::ZeroVector;
	
	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_Max UMETA(Hidden)
};

class ABaseCharacter;
UCLASS()
class DOMAIN_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual void PhysicsRotation(float DeltaTime) override;
	virtual float GetMaxSpeed() const override;
	
	//-----------------------------------------Sprint and Stamina------------------------------------------
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);
	void StartSprint();
	void EndSprint();
	bool CanJumpAndSprint();
	bool CannotJumpAndSprint();
	virtual bool CanCrouchInCurrentState() const override;
	
	//-----------------------------------------Mantle------------------------------------------
	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;
	void PhysMantling(float DeltaTime, int32 Iterations);
	
	bool bWantsToProne = false;
	bool bIsProne = false;
	bool bProneMaintainsBaseLocation = true;
	bool bIsOutOfStamina = false;
	bool bIsSprinting = false;
	bool bIsMantling = false;

	FORCEINLINE bool IsSprinting() const {return bIsSprinting;}
	FORCEINLINE bool IsOutOfStamina() const { return bIsOutOfStamina;}
	FORCEINLINE float GetSprintTurnRate() const { return SprintTurnRate;}
	FORCEINLINE TSubclassOf<UCameraShakeBase> GetSprintCameraShake() const {return SprintCameraShake;}
protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chatacter|Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 1200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chatacter|Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f, ClampMax = 1.f, UIMax = 1.f))
	float SprintTurnRate = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chatacter|Movement")
	TSubclassOf<UCameraShakeBase> SprintCameraShake;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chatacter|Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float OutOfStaminaSpeed = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chatacter|Movement", meta = (ClampMin = 0.0f, UIMin = 0.0f))
    float CrouchSpeed = 400.0f;

	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY()
	FVector BaseTranslationOffset;
	
	ABaseCharacter* BaseCharacter = nullptr;
	ABaseCharacter* GetBaseCharacterOwner() const;

private:
	FMantlingMovementParameters CurrentMantlingParameters;
	FTimerHandle MantlingTimer;
	FRotator ForcedTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;
};