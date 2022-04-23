// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeapon.h"
#include "Domain/Types.h"
#include "Domain/Actors/Equipable/WeaponBarrelComponent.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/BaseCharacterMovementComponent.h"
#include "Domain/Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TraceServices/Public/TraceServices/Containers/Timelines.h"

ARangeWeapon::ARangeWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarrel = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("WeaponBarrelComponent"));
	WeaponBarrel->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

	EquippedSocketName = SocketCharacterWeapon;
	ReticleType = EReticleType::Default;
}

EReticleType ARangeWeapon::GetReticleType() const
{
	ABaseCharacter* CharacterOwner = Cast<ABaseCharacter>(GetOwner());
	if(IsValid(CharacterOwner))
	{
		return CharacterOwner->GetIsAiming() ? AimReticleType : ReticleType;
	}
	return ReticleType;
}

void ARangeWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(MaxAmmo);
}

void ARangeWeapon::StartFire()
{
	if(WeaponFireMode == EWeaponFireMode::FullAuto)
	{
		GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
		GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeapon::MakeShot, GetShotTimerInterval(), true);
	} else if(WeaponFireMode == EWeaponFireMode::Single)
	{
		if(!bCanShot)
		{
			return;
		}
		bCanShot = false;
		GetWorld()->GetTimerManager().ClearTimer(CanShootTimer);
		GetWorld()->GetTimerManager().SetTimer(CanShootTimer, this, &ARangeWeapon::CanShotCallback, CharacterFireHipMontage->GetPlayLength(), false);
	}
	MakeShot();
}

void ARangeWeapon::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
}

void ARangeWeapon::MakeShot()
{
	if(bIsReloading)
	{
		return;
	}
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeapon::StartFire() only character can be an owner of range weapon"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	if(!CanShoot())
	{
		StopFire();
		return;
	}

	EndReload(false);

	if(IsValid(CharacterOwner->GetCharacterAttributeComponent()))
	{
		if(!CharacterOwner->GetCharacterAttributeComponent()->IsAlive())
		{
			return;
		}
	}
	
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if(!IsValid(Controller))
	{
		return;
	}
	
	if(bIsAiming)
	{
		if(IsValid(CharacterFireAimMontage))
		{
			CharacterOwner->PlayAnimMontage(CharacterFireAimMontage);
		}
		
	} else if(!bIsAiming)
	{
		if(IsValid(CharacterFireHipMontage))
		{
			CharacterOwner->PlayAnimMontage(CharacterFireHipMontage);
		}
	}
	
	PlayAnimMontage(WeaponFireMontage);
	UGameplayStatics::PlaySound2D(GetWorld(), FireSound);

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);

	SetAmmo(Ammo - 1);
	WeaponBarrel->Shot(PlayerViewPoint, ViewDirection, GetCurrentBulletSpreadAngle());

	if(Ammo == 0 && bAutoReload)
	{
		CharacterOwner->Reload();
	}
}

FTransform ARangeWeapon::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}

void ARangeWeapon::StartReload()
{
	if(Ammo == MaxAmmo)
	{
		return;
	}
	if(bIsReloading)
	{
		return;
	}
	StopAim();
	bCanAim = false;
	UGameplayStatics::PlaySound2D(GetWorld(), FullReloadSound);
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeapon::StartReload() only character can be an owner of range weapon"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	
	bIsReloading = true;
	if(IsValid(CharacterReloadMontage))
	{
		float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);

		if(ReloadMode == EReloadMode::FullClipReload)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this](){EndReload(true); },MontageDuration, false);
		}
	} else
	{
		EndReload(true);
	}
	
}

void ARangeWeapon::EndReload(bool bIsSuccess)
{
	if(!bIsReloading)
	{
		return;
	}

	bIsReloading = false;
	bCanAim = true;

	if(OnReloadBegin.IsBound())
	{
		OnReloadBegin.Broadcast();
	}
	
	if(!bIsSuccess)
	{
		ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
		if(IsValid(CharacterOwner))
		{
			if(CharacterReloadMontage)
			{
				CharacterOwner->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f, CharacterReloadMontage);
			}
			if(WeaponReloadMontage)
			{
				WeaponMesh->GetAnimInstance()->Montage_Stop(0.1f, WeaponReloadMontage);
			}
		}
	}

	if(ReloadMode == EReloadMode::ByBulletReload)
	{
		ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
		UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if(IsValid(AnimInstance))
		{
			AnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, CharacterReloadMontage);
		}
		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
		if(IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, WeaponReloadMontage);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	
	if(bIsSuccess && OnReloadComplete.IsBound())
	{
		OnReloadComplete.Broadcast();
	}
}

void ARangeWeapon::StartAim()
{
	bIsAiming = true;
}

void ARangeWeapon::StopAim()
{
	bIsAiming = false;
}

float ARangeWeapon::GetAimFOV() const
{
	return AimFOV;
}

float ARangeWeapon::GetAimMovementMaxSpeed() const
{
	return AimMovementMaxSpeed;
}

float ARangeWeapon::GetAimTurnModifier() const
{
	return AimTurnModifier;
}

float ARangeWeapon::GetAimLookUpModifier() const
{
	return AimLookUpModifier;
}

UCurveFloat* ARangeWeapon::GetScopeCurve() const
{
	return ScopeCurve;
}

int32 ARangeWeapon::GetAmmo() const
{
	return Ammo;
}

int32 ARangeWeapon::GetMaxAmmo() const
{
	return MaxAmmo;
}

void ARangeWeapon::SetAmmo(int32 NewAmmo)
{
	Ammo = NewAmmo;
	if(OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(Ammo);
	}
}

bool ARangeWeapon::CanShoot() const
{
	return Ammo > 0;
}

bool ARangeWeapon::GetIsReloading() const
{
	if(this)
	{
		return bIsReloading;
	}
	return false;
}

EAmmunitionType ARangeWeapon::GetAmmoType() const
{
	return AmmoType;
}

void ARangeWeapon::CanShotCallback()
{
	bCanShot = true;
}

float ARangeWeapon::GetCurrentBulletSpreadAngle() const
{
	float AngleInDegrees = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegrees);
}

float ARangeWeapon::PlayAnimMontage(UAnimMontage* Montage) const
{
	float Result = 0;
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if(IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(Montage);
	}
	return Result;
}

float ARangeWeapon::GetShotTimerInterval() const
{
	return 60.f / RateOfFire;
}
