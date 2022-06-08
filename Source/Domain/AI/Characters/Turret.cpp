// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/AI/Characters/Turret.h"

#include "AIController.h"
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
		default:
			break;
	}
}

void ATurret::SetCurrentTarget(AActor* NewTarget)
{
	CurrentTarget = NewTarget;
	ETurretState NewState = IsValid(CurrentTarget) ? ETurretState::Firing : ETurretState::Searching;
	SetCurrentTurretState(NewState);
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarrelComponent->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarrelComponent->GetComponentRotation();
}

void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if(IsValid(AIController))
	{
		FGenericTeamId TeamId((uint8)Fraction);
		AIController->SetGenericTeamId(TeamId);
	}
}

void ATurret::MakeShot()
{
	FVector ShotLocation = WeaponBarrelComponent->GetComponentLocation();
	FVector ShotDirection = WeaponBarrelComponent->GetComponentRotation().RotateVector(FVector::ForwardVector);
	WeaponBarrelComponent->Shot(ShotLocation, ShotDirection, FMath::DegreesToRadians(BulletSpreadAngle));
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
	if(!IsValid(CurrentTarget))
	{
		SetCurrentTurretState(ETurretState::Searching);
	}
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseFiringInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarrelLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarrelComponent->GetComponentLocation()).GetSafeNormal();
	float BarrelLookAtPitchAngle = BarrelLookAtDirection.ToOrientationRotator().Pitch;
	
	FRotator BarrelLocalRotation = TurretBarrelComponent->GetRelativeRotation();
	BarrelLocalRotation.Pitch = FMath::FInterpTo(BarrelLocalRotation.Pitch, BarrelLookAtPitchAngle, DeltaTime, BarrelPitchRotationRate);
	TurretBarrelComponent->SetRelativeRotation(BarrelLocalRotation);
}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	bool bIsStateChanged = NewState != CurrentState;
	CurrentState = NewState;
	if(!bIsStateChanged)
	{
		return;
	}
	switch (CurrentState)
	{
		case ETurretState::Firing:
			GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot, GetFireInterval(), true, FireDelay);
			break;

		case ETurretState::Searching:
			GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
			break;
	}
	
}

float ATurret::GetFireInterval() const
{
	return 60.f / RateOfFire;
}
