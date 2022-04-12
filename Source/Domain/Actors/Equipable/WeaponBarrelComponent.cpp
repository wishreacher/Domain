#include "WeaponBarrelComponent.h"
#include "DrawDebugHelpers.h"
#include "Domain/Types.h"
#include "Kismet/GameplayStatics.h"
#include "Domain/Subsystems/DebugSubsystem.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UWeaponBarrelComponent::Shot(FVector ShotStart, FVector ShotDirection, AController* Controller, float SpreadAngle) const
{
	for(int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
	
	FVector MuzzleLocation = GetComponentLocation();
	FVector ShotEnd = ShotStart + FireRange * ShotDirection;

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubSystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
#else
	bIsDebugEnabled = false;
#endif

	FHitResult ShotResult;
	
	if(GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet))
	{
		ShotEnd = ShotResult.ImpactPoint;
		if(bIsDebugEnabled)
		{
			DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
		}
		AActor* HitActor = ShotResult.GetActor();
		if(IsValid(HitActor))
		{
			float FinalDamage;
			if(IsValid(DamageFallCurve))
			{
				FinalDamage = BaseDamageAmount * DamageFallCurve->GetFloatValue(FVector::Distance(ShotStart, ShotEnd) / 100.f); // multiplying by 100 to get curve values in meters
			} else
			{
				FinalDamage = BaseDamageAmount;
			}

			FPointDamageEvent DamageEvent;
			DamageEvent.HitInfo = ShotResult;
			DamageEvent.ShotDirection = ShotDirection;
			DamageEvent.DamageTypeClass = DamageTypeClass;
			HitActor->TakeDamage(FinalDamage, DamageEvent, Controller, GetOwner());

			UDecalComponent* DecalComponent =  UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize,
				ShotEnd,ShotResult.ImpactNormal.ToOrientationRotator());
			if(IsValid(DecalComponent))
			{
				DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
				DecalComponent->SetFadeScreenSize(0.0001f);
			}
			
			GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Magenta, FString::Printf(TEXT("%f damage done"), FinalDamage));
		}
	}
	if(IsValid(TraceFX))
	{
		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
	}
	
	if(bIsDebugEnabled)
	{
		DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
	}
	}
}

FVector UWeaponBarrelComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;
}
