// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Domain/Types.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* MantlingMontage;

	/** Adding curve slot.
	 * This curve defines duration of an animation states. If you need to change delay after mantling, you should modify this one
	 */ 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* MantlingCurve;
	
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
class DOMAIN_API ABaseCharacter : public ACharacter, public IGenericTeamAgentInterface
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
	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;
	virtual void StartTakeDown();
	virtual void PossessedBy(AController* NewController) override;
	
	UFUNCTION(BlueprintCallable)
	virtual void ToggleEquip();

	/** Activates mantling
	 * if ULedgeDetectorComponent() detects a climbable ledge:
	 *	1) Fills mantling params.
	 *	2) Activates BaseCharacterMovementComponent::StartMantle().
	 *	3) Plays selected montage.
	 */
	virtual void Mantle(bool bForce);

	FORCEINLINE void SetTimeDilation(float NewDilation){UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewDilation);}
	
	//-----------------------------------------Getters------------------------------------------
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return BaseCharacterMovementComponent; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const { return CharacterEquipmentComponent; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsSprinting() const {return bIsSprintRequested;}
	FORCEINLINE bool GetIsCrouching() const {return bIsCrouching;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetIsAttacking(bool NewAttacking) {bIsAttacking = NewAttacking;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetCanAttack(bool NewCanAttack) {bCanAttack = NewCanAttack;}
	
	UFUNCTION(BlueprintCallable)
	UCharacterAttributeComponent* GetCharacterAttributeComponent() const;
	
	UFUNCTION(BlueprintCallable)
	virtual void ToggleControls(bool bShouldEnableControl);

	//-----------------------------------------TeamInterface------------------------------------------
	virtual FGenericTeamId GetGenericTeamId() const override;
	
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
	UAnimMontage* OnDeathAnimMontage;

	// Damage depends on fall height in meters
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Attributes")
	UCurveFloat* FallDamageCurve;
	
	virtual void OnDeath();

	virtual void OnOutOfStamina(bool IsOutOfStamina);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBaseCharacterMovementComponent* BaseCharacterMovementComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCharacterAttributeComponent* CharacterAttributeComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCharacterEquipmentComponent* CharacterEquipmentComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EFractions Fraction = EFractions::Bolshevik;
	
private:
	void TryChangeSprintState();
	
	float DefaultMaxMovementSpeed = 0.0f;
	float CurrentAimingMovementSpeed = 0.f;
	bool bIsSprintRequested = false;
	bool bCanCrouch = true;
	bool bIsCrouching = false;
	bool bIsAttacking = false;
	bool bCanAttack = true;
	
	FTimerHandle DeathMontageTimer;
	FVector CurrentFallApex;

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;
};

inline const FMantlingSettings& ABaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}