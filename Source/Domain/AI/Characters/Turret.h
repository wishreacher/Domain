// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Domain/Types.h"
#include "Domain/Components/CharacterComponents/CharacterAttributeComponent.h"
#include "GameFramework/Pawn.h"
#include "Turret.generated.h"

class UWeaponBarrelComponent;
UENUM(BlueprintType)
enum class ETurretState : uint8
{
	Searching,
	Firing
};

UCLASS()
class DOMAIN_API ATurret : public APawn
{
	GENERATED_BODY()

public:
	ATurret();
	virtual void Tick(float DeltaTime) override;
	void SetCurrentTarget(AActor* NewTarget);
	virtual FVector GetPawnViewLocation() const override;
	virtual FRotator GetViewRotation() const override;
	virtual void PossessedBy(AController* NewController) override;
	void MakeShot();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBaseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBarrelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponBarrelComponent* WeaponBarrelComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters", meta = (ClampMin = 0.f, UIMin = 0.f))
	float BaseSearchingRotationRate = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters", meta = (ClampMin = 0.f, UIMin = 0.f))
	float BarrelPitchRotationRate = 60.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters")
	float MaxBarrelPitchRotationRate = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters")
	float MinBarrelPitchRotationRate = -30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters")
	float BaseFiringInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters|Fire", meta = (ClampMin = 1.f, UIMin = 1.f))
	float RateOfFire = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters|Fire", meta = (ClampMin = 0.f, UIMin = 0.f))
	float BulletSpreadAngle = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters|Fire", meta = (ClampMin = 0.f, UIMin = 0.f))
	float FireDelay = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EFractions Fraction = EFractions::Bolshevik;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCharacterAttributeComponent* CharacterAttributeComponent;

private:
	void SearchingMovement(float DeltaTime);
	void FiringMovement(float DeltaTime);
	void SetCurrentTurretState(ETurretState NewState);	
	ETurretState CurrentState = ETurretState::Searching;

	AActor* CurrentTarget = nullptr;

	FTimerHandle ShotTimer;
	float GetFireInterval() const;
};
