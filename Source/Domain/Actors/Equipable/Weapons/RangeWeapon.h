// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../Actors/Equipable/EquippableItem.h"
#include "RangeWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32)
DECLARE_MULTICAST_DELEGATE(FOnReloadBegin)
DECLARE_MULTICAST_DELEGATE(FOnReloadComplete)

UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	Single,
	FullAuto
};

UENUM(BlueprintType)
enum class EReloadMode : uint8
{
	FullClipReload,
	ByBulletReload
};

class UAnimMontage;
UCLASS(Blueprintable)
class DOMAIN_API ARangeWeapon : public AEquippableItem
{
	GENERATED_BODY()

public:
	ARangeWeapon();
	
	void StartFire();
	void StopFire();
	int32 GetAmmo() const;
	int32 GetMaxAmmo() const;
	void SetAmmo(int32 NewAmmo);
	bool CanShoot() const;
	
	void StartAim();
	void StopAim();

	UFUNCTION(BlueprintCallable)
	float GetAimFOV() const;

	UFUNCTION(BlueprintCallable)
	float GetAimMovementMaxSpeed() const;
	
	UFUNCTION(BlueprintCallable)
	float GetAimTurnModifier() const;

	UFUNCTION(BlueprintCallable)
	float GetAimLookUpModifier() const;

	UFUNCTION(BlueprintCallable)
	UCurveFloat* GetScopeCurve() const;
	
	FORCEINLINE bool CanAim() const {return bCanAim;}
	
	void StartReload();
	void EndReload(bool bIsSuccess);
	FOnReloadComplete OnReloadComplete;
	FOnReloadBegin OnReloadBegin;
	bool GetIsReloading() const;
	FOnAmmoChanged OnAmmoChanged;
	EAmmunitionType GetAmmoType() const;
	
	FTransform GetForeGripTransform() const;

	void CanShotCallback();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Parameters|Sound")
	USoundBase* ImpactSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Parameters|Sound")
	USoundBase* FireSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Parameters|Sound")
	USoundBase* FullReloadSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Parameters|Sound")
	USoundBase* BulletReloadSound = nullptr;

	virtual EReticleType GetReticleType() const override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWeaponBarrelComponent* WeaponBarrel;
	
	UPROPERTY(EditAnywhere, Category = "Animations|Weapon")
	UAnimMontage* WeaponFireMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animations|Weapon")
	UAnimMontage* WeaponReloadMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animations|Character")
	UAnimMontage* CharacterFireHipMontage;

	UPROPERTY(EditAnywhere, Category = "Animations|Character")
	UAnimMontage* CharacterFireAimMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animations|Character")
	UAnimMontage* CharacterReloadMontage;

	//rounds per minute
	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters", meta = (ClampMin = 10.f, UIMin = 10.f))
	float RateOfFire = 600.f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters")
	EAmmunitionType AmmoType = EAmmunitionType::Pistol;

	// FullClip reload mode adds all ammo only when the whole reload animation is played successfully
	// ByBullet reload requires section "EndReload" in character reload animation
	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters")
	EReloadMode ReloadMode = EReloadMode::FullClipReload;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters")
	bool bAutoReload = true;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float SpreadAngle = 1.f; // bullet spread half angle in degrees

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters|Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float AimSpreadAngle = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters|Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 1000.f, UIMax = 1000.f))
	float AimMovementMaxSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters|Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 120.f, UIMax = 120.f))
	float AimFOV = 60.f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters|Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 1.f, UIMax = 1.f))
	float AimTurnModifier = 1.f;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters|Aiming", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 1.f, UIMax = 1.f))
	float AimLookUpModifier = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Parameters|Aiming")
	UCurveFloat* ScopeCurve = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon|Parameters|Aiming", meta = (ClampMin = 1, UIMin = 1))
	int32 MaxAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Parameters|Aiming")
	EReticleType AimReticleType;
private:
	float GetCurrentBulletSpreadAngle() const;
	float PlayAnimMontage(UAnimMontage* Montage) const;
	float GetShotTimerInterval() const;
	void MakeShot();
	
	int32 Ammo = 0;
	bool bIsAiming;
	bool bCanShot = true;
	bool bIsReloading = false;
	bool bCanAim = true;
	
	FTimerHandle ShotTimer;
	FTimerHandle ReloadTimer;
	FTimerHandle CanShootTimer;
};
