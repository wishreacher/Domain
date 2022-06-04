
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Domain/Actors/Projectiles/BaseProjectile.h"
#include "WeaponBarrelComponent.generated.h"

USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal Info")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal Info", meta = (EditCondition = "DecalMaterial != nullptr"))
	FVector DecalSize = FVector(5.f, 5.f, 5.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal Info", meta = (EditCondition = "DecalMaterial != nullptr"))
	float DecalLifeTime = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal Info", meta = (EditCondition = "DecalMaterial != nullptr"))
	float DecalFadeOutTime = 5.f;
};

UENUM()
enum class EHitRegistrationType : uint8
{
	HitScan,
	Projectile
};

class UNiagaraSystem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOMAIN_API UWeaponBarrelComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	void Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes")
	float FireRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes", meta = (ClampMin = 1, UIMin = 1))
	int32 BulletsPerShot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes")
	EHitRegistrationType HitRegistration = EHitRegistrationType::HitScan;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes", meta = (EditCondition = "HitRegistration == EHitRegistrationType::Projectile"))
	TSubclassOf<ABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes")
	float BaseDamageAmount = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes")
	UCurveFloat* DamageFallCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes|Decals")
	FDecalInfo DefaultDecalInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes|Decals")
	TSubclassOf<class UDamageType> DamageTypeClass;

private:
	float FinalDamage;
	
	bool HitScan(FVector ShotStart, FVector ShotDirection, FVector& ShotEnd, FHitResult ShotResult);
	void LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection);

	UFUNCTION()
	void ProcessHit(const FHitResult& Hit, const FVector& Direction);
	
	APawn* GetOwningPawn() const;
	AController* GetController() const;
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;
};
