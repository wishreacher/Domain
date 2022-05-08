// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

private:
	void SearchingMovement(float DeltaTime);
	void FiringMovement(float DeltaTime);
	FORCEINLINE void SetCurrentTurretState(ETurretState NewState){CurrentState = NewState;}
	
	ETurretState CurrentState = ETurretState::Searching;

	AActor* CurrentTarget = nullptr;
	
};
