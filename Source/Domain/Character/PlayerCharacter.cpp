 // Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "Domain/Components/BaseCharacterMovementComponent.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;
	
	CameraShakeComponent = CreateDefaultSubobject<UCameraShakeSourceComponent>(TEXT("CameraShake"));
	CameraShakeComponent->SetupAttachment(CameraComponent);

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	Fraction = EFractions::UNO;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	// ARangeWeapon* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	// if(IsValid(RangeWeapon))
	// {
	// 	FOnTimelineFloat InterpFunction;
	// 	InterpFunction.BindUFunction(this, FName("TimelineFloatReturn"));
	// 	ScopeTimeLine.AddInterpFloat(RangeWeapon->GetScopeCurve(), InterpFunction, FName("Alpha"));
	// 	ScopeTimeLine.SetLooping(false);
	// }

	// TArray<UActorComponent*> TakeDownComponents = GetComponentsByClass(UTakeDownComponent::StaticClass());
	// for(UActorComponent* ActorComponent : TakeDownComponents)
	// {
	// 	if(ActorComponent->IsA<UTakeDownComponent>())
	// 	{
	// 		TakeDownComponent = Cast<UTakeDownComponent>(ActorComponent);
	// 	}
	// }
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// ScopeTimeLine.TickTimeline(DeltaSeconds);
}

void APlayerCharacter::MoveForward(float Value)
{
	if(GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f );
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if(GetIsSprinting())
	{
		return;
	}
	if(GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f );
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	float AimTurnModifier = 1.f;
	float SprintTurnModifier = 1.f;
	if(GetIsSprinting())
	{
		SprintTurnModifier = GetBaseCharacterMovementComponent()->GetSprintTurnRate();
	}
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds() * AimTurnModifier * SprintTurnModifier);
}

void APlayerCharacter::LookUp(float Value)
{
	float AimLookUpModifier = 1.f;
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * AimLookUpModifier);
}

void APlayerCharacter::TurnAtRate(float Value)
{
	float AimTurnModifier = 1.f;
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds() * AimTurnModifier);
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	float AimLookUpModifier = 1.f;
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * AimLookUpModifier);
}

void APlayerCharacter::StartSprint()
{
	Super::StartSprint();
	CurrentCameraShakeClass = GetBaseCharacterMovementComponent()->GetSprintCameraShake();
	GetCameraShakeComponent()->CameraShake = CurrentCameraShakeClass;
	CameraShakeComponent->Start();
}

void APlayerCharacter::StopSprint()
{
	Super::StopSprint();
	GetCameraShakeComponent()->StopAllCameraShakesOfType(CurrentCameraShakeClass, true);
}

void APlayerCharacter::StartTakeDown()
{
	if(IsValid(TakeDownComponent))
	{
		TakeDownComponent->BeginTakeDown();
	}
}

void APlayerCharacter::OnDeath()
{
	Super::OnDeath();
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset+= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset-= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return bIsCrouched || Super::CanJumpInternal_Implementation();
}

void APlayerCharacter::OnJumped_Implementation()
{
	if(bIsCrouched)
	{
		UnCrouch(true);
	}
}

// void APlayerCharacter::TimelineFloatReturn(float Value) const
// {
// 	APlayerController* PlayerController = GetController<APlayerController>();
// 	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
// 	if(IsValid(CameraManager))
// 	{
// 		ARangeWeapon* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
// 		CameraManager->SetFOV(FMath::Lerp(CameraComponent->FieldOfView, RangeWeapon->GetAimFOV(), Value));
// 	}
// }

void APlayerCharacter::ToggleControls(bool bShouldEnableControl)
{
	Super::ToggleControls(bShouldEnableControl);
	CameraComponent->bUsePawnControlRotation = bShouldEnableControl;
	SpringArmComponent->bUsePawnControlRotation = bShouldEnableControl;
}

void APlayerCharacter::SetDefaultCameraFOV_Implementation()
{
	
}

void APlayerCharacter::SetAimCameraFOV_Implementation()
{
	
}
