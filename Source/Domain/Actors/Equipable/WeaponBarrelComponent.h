
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WeaponBarrelComponent.generated.h"

USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal Info")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal Info")
	FVector DecalSize = FVector(5.f, 5.f, 5.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal Info")
	float DecalLifeTime = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Decal Info")
	float DecalFadeOutTime = 5.f;
};

class UNiagaraSystem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOMAIN_API UWeaponBarrelComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	void Shot(FVector ShotStart, FVector ShotDirection, AController* Controller, float SpreadAngle) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes")
	float FireRange = 5000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barrel Attributes", meta = (ClampMin = 1, UIMin = 1))
	int32 BulletsPerShot = 1;

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
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;
};
