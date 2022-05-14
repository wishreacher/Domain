// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/AI/Characters/Turret.h"
#include "Domain/Actors/Equipable/WeaponBarrelComponent.h"
#include "Kismet/GameplayStatics.h"


ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);

	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBaseComponent->SetupAttachment(TurretRoot);

	TurretBarrelComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarrelComponent"));
	TurretBarrelComponent->SetupAttachment(TurretBaseComponent);

	WeaponBarrelComponent = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("WeaponBarrel"));
	WeaponBarrelComponent->SetupAttachment(TurretBarrelComponent);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	switch (CurrentState)
	{
		case ETurretState::Searching:
			SearchingMovement(DeltaTime);
			break;
		case ETurretState::Firing:
			FiringMovement(DeltaTime);
			break;
	}
	// UKismetSystemLibrary::PrintString(this, UEnum::GetDisplayValueAsText(CurrentState).ToString());
}

void ATurret::SetCurrentTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
	ETurretState NewState = IsValid(CurrentTarget) ? ETurretState::Firing : ETurretState::Searching;
	SetCurrentTurretState(NewState);
	// UKismetSystemLibrary::PrintString(this, NewTarget->GetName());
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarrelComponent->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarrelComponent->GetComponentRotation();
}

void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotation = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotation);

	FRotator TurretBarrelRotation = TurretBarrelComponent->GetRelativeRotation();
	TurretBarrelRotation.Pitch = FMath::FInterpTo(TurretBarrelRotation.Pitch, 0.f, DeltaTime, BarrelPitchRotationRate);
	TurretBarrelComponent->SetRelativeRotation(TurretBarrelRotation);
}

void ATurret::FiringMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseFiringInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarrelLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarrelComponent->GetComponentLocation()).GetSafeNormal();
	float BarrelLookAtPitchAngle = BarrelLookAtDirection.ToOrientationRotator().Pitch;
	
	FRotator BarrelLocalRotation = TurretBarrelComponent->GetRelativeRotation();
	BarrelLocalRotation.Pitch = FMath::FInterpTo(BarrelLocalRotation.Pitch, BarrelLookAtPitchAngle, DeltaTime, BarrelPitchRotationRate);
	TurretBarrelComponent->SetWorldRotation(BarrelLocalRotation);
	
}