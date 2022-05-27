// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "Domain/Actors/Equipable/Weapons/RangeWeapon.h"
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
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()) && GetIsAiming())
	{
		AimTurnModifier = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimTurnModifier();
	}
	if(GetIsSprinting())
	{
		SprintTurnModifier = GetBaseCharacterMovementComponent()->GetSprintTurnRate();
	}
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds() * AimTurnModifier * SprintTurnModifier);
}

void APlayerCharacter::LookUp(float Value)
{
	float AimLookUpModifier = 1.f;
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()) && GetIsAiming())
	{
		AimLookUpModifier = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimLookUpModifier();
	}
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * AimLookUpModifier);
}

void APlayerCharacter::TurnAtRate(float Value)
{
	float AimTurnModifier = 1.f;
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()) && GetIsAiming())
	{
		AimTurnModifier = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimTurnModifier();
	}
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds() * AimTurnModifier);
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	float AimLookUpModifier = 1.f;
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()) && GetIsAiming())
	{
		AimLookUpModifier = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimLookUpModifier();
	}
	
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

bool APlayerCharacter::CanExecuteTakeDown() const
{
	//TODO check if the weapon has corresponding takedowns
	// AMeleeWeapon* WeaponRef = Cast<AMeleeWeapon>(GetCharacterEquipmentComponent()->GetCurrentLoadout().Find(EEquipmentSlots::MeleeWeapon));
	return !GetCharacterEquipmentComponent()->GetIsEquipping() && !bIsExecutingTakeDown;
}

void APlayerCharacter::ToggleControls(bool bShouldEnableControl)
{
	GetBaseCharacterMovementComponent()->SetActive(bShouldEnableControl);
	CameraComponent->bUsePawnControlRotation = bShouldEnableControl;
	SpringArmComponent->bUsePawnControlRotation = bShouldEnableControl;
	bUseControllerRotationYaw = bShouldEnableControl;
}

void APlayerCharacter::StartTakeDown()
{
	if(!CanExecuteTakeDown()) //checking if we can fire a takedown or not
	{
		return;
	}
	
	UCharacterEquipmentComponent* EquipmentComponent = GetCharacterEquipmentComponent();
	if(!IsValid(EquipmentComponent))
	{
		return;
	}
	
	if(EquipmentComponent->GetCurrentEquippedSlot() != EEquipmentSlots::MeleeWeapon) //equipping the melee weapon if it's not
	{
		EquipmentComponent->EquipItemInSlot(EEquipmentSlots::MeleeWeapon);
	}

	if(!GetBaseCharacterMovementComponent()->IsCrouching()) //crouching if we aren't
	{
		Crouch(true);
	}

	
	if(GetShouldExecuteBehindTakeDown()) //executing takedown
	{
		SetupTarget(); //TODO move the trace to c++
		if(IsValid(BehindTakeDownTarget))
		{
			ExecuteTakeDown(ETakeDownType::Behind, BehindTakeDownTarget);
		}
	}
	if(GetShouldExecuteTopTakeDown())
	{
		if(IsValid(BehindTakeDownTarget))
		{
			ExecuteTakeDown(ETakeDownType::Top, TopTakeDownTarget);
		}
	}
	
}

void APlayerCharacter::ExecuteTakeDown(ETakeDownType Type, AActor* Target)
{
	UCharacterEquipmentComponent* EquipmentComponent = GetCharacterEquipmentComponent();
	AMeleeWeapon* WeaponHardRef = EquipmentComponent->GetCurrentMeleeWeapon();
	
	TArray<FTakeDownDescription> AllAvailableTakeDowns = WeaponHardRef->GetTakeDowns(); //Gathering the array of possible takedowns
	ToggleControls(false); //disabling movement
	
	bIsExecutingTakeDown = true;
	SetCanAttack(false);

	TArray<FTakeDownDescription> ChosenTakeDowns;
	for(FTakeDownDescription TakeDownDescription : WeaponHardRef->GetTakeDowns()) //chosing what animations should we play based on takedown type
	{
		if(TakeDownDescription.TakeDownType == Type)
		{
			//TODO AddUnique doesn't work for some reason
			ChosenTakeDowns.Add(TakeDownDescription);
		}
	}

	FTakeDownDescription RequestedTakeDown = ChosenTakeDowns[FMath::RandRange(0, ChosenTakeDowns.Num() - 1)]; //choosing one particular takedown
	MoveToTakeDownTarget(Target, RequestedTakeDown.Offset); //moving behind the target with offset
	PlayTakeDownMontages(RequestedTakeDown); //playing animation montages on player
	EnemyTakeDownInterfaceCall(Target, RequestedTakeDown); //calling the enemy takedown interface
	TakeDownCameraRigActivation(RequestedTakeDown); //camera actions

	GetWorld()->GetTimerManager().ClearTimer(TakeDownTimerHandle); //setting up the timer event that fires off when takedown is close to an end
	GetWorld()->GetTimerManager().SetTimer(TakeDownTimerHandle, this, &APlayerCharacter::TakeDownEnd,
		RequestedTakeDown.PlayerTakeDownMontage->GetPlayLength() - 0.2, false);
}

void APlayerCharacter::TakeDownEnd_Implementation()
{
	ToggleControls(true);
	bIsExecutingTakeDown = false;
	SetCanAttack(true);
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

float APlayerCharacter::GetAimTurnModifier() const
{
	ARangeWeapon* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	return RangeWeapon->GetAimTurnModifier();
}

float APlayerCharacter::GetAimLookUpModifier() const
{
	ARangeWeapon* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	return RangeWeapon->GetAimLookUpModifier();
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

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();
	APlayerController* PlayerController = GetController<APlayerController>();
	// ARangeWeapon* RangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	
	if(!IsValid(PlayerController))
	{
		return;
	}
	if(GetBaseCharacterMovementComponent()->IsSprinting() || GetBaseCharacterMovementComponent()->IsOutOfStamina() || GetBaseCharacterMovementComponent()->IsSwimming())
	{
		return;
	}
	// if(IsValid(RangeWeapon->GetScopeCurve()))
	// {
	// 	ScopeTimeLine.PlayFromStart();
	// } else
	// {
	// 	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	// 	if(IsValid(CameraManager))
	// 	{
	// 		CameraManager->SetFOV(RangeWeapon->GetAimFOV());
	// 	}
	// }
	SetAimCameraFOV();
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	SetDefaultCameraFOV();
	APlayerController* PlayerController = GetController<APlayerController>();
	
	if(!IsValid(PlayerController))
	{
		return;
	}

	// APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	// if(IsValid(CameraManager))
	// {
	// 	CameraManager->UnlockFOV();
	// }
	// ScopeTimeLine.Stop();
	
}

void APlayerCharacter::OnWeaponReloadBegin()
{
	OnStopAimingInternal();
}

void APlayerCharacter::SetDefaultCameraFOV_Implementation()
{
	
}

void APlayerCharacter::SetAimCameraFOV_Implementation()
{
	
}
