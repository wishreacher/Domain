// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "AIController.h"
#include "Curves/CurveVector.h"
#include "Domain/Components/BaseCharacterMovementComponent.h"
#include "Domain/Components/LedgeDetectorComponent.h"
#include "Domain/Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Engine/DamageEvents.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	BaseCharacterMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());
	
	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
	
	CharacterAttributeComponent = CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("CharacterAttributes"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(CharacterAttributeComponent))
	{
		CharacterAttributeComponent->OnOutOfStaminaEvent.AddUObject(this, &ABaseCharacter::OnOutOfStamina);
		CharacterAttributeComponent->OnDeathEvent.AddUObject(this, &ABaseCharacter::OnDeath);
	}
}

//Movement
void ABaseCharacter::Falling()
{
	GetBaseCharacterMovementComponent()->bNotifyApex = true;
}

void ABaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01f;
	if(IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.GetActor());
	}
}

bool ABaseCharacter::CanMantle() const
{
	if(GetBaseCharacterMovementComponent()->IsMantling())
	{
		return false;
	}
	if(GetBaseCharacterMovementComponent()->IsFalling())
	{
		return false;
	}
	return true;
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TryChangeSprintState();
}

void ABaseCharacter::Jump()
{
	Super::Jump();
}

void ABaseCharacter::ChangeCrouchState()
{
	if(BaseCharacterMovementComponent->CanCrouchInCurrentState() && !GetCharacterMovement()->IsCrouching() && !BaseCharacterMovementComponent->bIsOutOfStamina)
	{
		
		Crouch(true);
	} else if(GetCharacterMovement()->IsCrouching())
	{
		
		UnCrouch(true);
	}
}

void ABaseCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
	bIsCrouching = true;
}

void ABaseCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
	bIsCrouching = false;
}

void ABaseCharacter::StartTakeDown()
{
	
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if(IsValid(AIController))
	{
		FGenericTeamId TeamId((uint8)Fraction);
		AIController->SetGenericTeamId(TeamId);
	}
}

void ABaseCharacter::Mantle(bool bForce)
{
	if(!(CanMantle() || bForce))
	{
		return;
	}
	if(BaseCharacterMovementComponent->bIsMantling == true)
	{
		return;
	}
	if(BaseCharacterMovementComponent->IsCrouching())
	{
		return;
	}
	FLedgeDescription LedgeDescription;
	if(LedgeDetectorComponent->DetectLedge(LedgeDescription))
	{

		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location ;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;

		float MantlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);
		
		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;

		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;

		FVector2D SourceRange (MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange (MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;

		GetBaseCharacterMovementComponent()->StartMantle(MantlingParameters);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		
	}
}

bool ABaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}

void ABaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch(true);
	}
}

void ABaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

bool ABaseCharacter::CanSprint()
{
	if(BaseCharacterMovementComponent->IsCrouching())
	{
		return true;
	}
	return true;
}

void ABaseCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();
	float Duration = PlayAnimMontage(OnDeathAnimMontage);
	if(Duration == 0)
	{
		// GetWorld()->GetTimerManager().SetTimer(DeathMontageTimer, this, &ABaseCharacter::EnableRagdoll, Duration, false);
	}
}

void ABaseCharacter::OnOutOfStamina(bool IsOutOfStamina)
{
	if(IsOutOfStamina == true)
	{
		BaseCharacterMovementComponent->bIsOutOfStamina = true;
		bCanCrouch = false;
	} else if(IsOutOfStamina == false)
	{
		BaseCharacterMovementComponent->bIsOutOfStamina = false;
		bCanCrouch = true;
	}
}

void ABaseCharacter::TryChangeSprintState()
{
	if (bIsSprintRequested && !BaseCharacterMovementComponent->IsSprinting() && CanSprint() && !bIsCrouched)
	{
		BaseCharacterMovementComponent->StartSprint();
		OnSprintStart();
	}

	if (!bIsSprintRequested && BaseCharacterMovementComponent->IsSprinting())
	{
		BaseCharacterMovementComponent->EndSprint();
		OnSprintEnd();
	}
}

void ABaseCharacter::ToggleControls(bool bShouldEnableControl)
{
	GetBaseCharacterMovementComponent()->SetActive(bShouldEnableControl);
	bUseControllerRotationYaw = bShouldEnableControl;
	GetBaseCharacterMovementComponent()->SetJumpAllowed(bShouldEnableControl);
}

FGenericTeamId ABaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)Fraction);
	
}

//Getters

UCharacterAttributeComponent* ABaseCharacter::GetCharacterAttributeComponent() const
{
	return CharacterAttributeComponent;
}

