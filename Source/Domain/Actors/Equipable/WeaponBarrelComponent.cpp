#include "WeaponBarrelComponent.h"
#include "DrawDebugHelpers.h"
#include "Domain/Types.h"
#include "Kismet/GameplayStatics.h"
#include "Domain/Subsystems/DebugSubsystem.h"
#include "Components/DecalComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Weapons/RangeWeapon.h"

bool UWeaponBarrelComponent::HitScan(FVector ShotStart, FVector ShotDirection,
                                     OUT FVector& ShotEnd, FHitResult ShotResult) 
{
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
	if(bHasHit)
	{
		ShotEnd = ShotResult.ImpactPoint;
		if(IsValid(DamageFallCurve))
		{
			FinalDamage = BaseDamageAmount * DamageFallCurve->GetFloatValue(FVector::Distance(ShotStart, ShotEnd) / 100.f); // multiplying by 100 to get curve values in meters
		} else
		{
			FinalDamage = BaseDamageAmount;
		}
		
		ProcessHit(ShotResult, ShotDirection);
	}
	return bHasHit;
}

void UWeaponBarrelComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection)
{
	FinalDamage = BaseDamageAmount;
	ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, LaunchStart, LaunchDirection.ToOrientationRotator());
	if(IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwningPawn());
		Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarrelComponent::ProcessHit);
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}
}

void UWeaponBarrelComponent::ProcessHit(const FHitResult& Hit, const FVector& Direction)
{
	AActor* HitActor = Hit.GetActor();

	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = Hit;
	DamageEvent.ShotDirection = Direction;
	DamageEvent.DamageTypeClass = DamageTypeClass;
	HitActor->TakeDamage(FinalDamage, DamageEvent, GetController(), GetOwner());

	UDecalComponent* DecalComponent =  UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize,
		Hit.ImpactPoint,Hit.ImpactNormal.ToOrientationRotator());
	if(IsValid(DecalComponent))
	{
		DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
		DecalComponent->SetFadeScreenSize(0.0001f);
	}

	ARangeWeapon* Weapon = Cast<ARangeWeapon>(GetOwner());
	if(IsValid(Weapon))
	{
		if(IsValid(Weapon->ImpactSound))
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), Weapon->ImpactSound, Hit.Location, FRotator::ZeroRotator, 2.f);
		}
	}

	if(IsValid(TraceFX))
	{
		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX,  GetComponentLocation(), GetComponentRotation());
		TraceFXComponent->SetVectorParameter(FXParamTraceEnd, Hit.ImpactPoint);
	}
	
	if(IsValid(HitActor))
	{
		GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Magenta, FString::Printf(TEXT("%f damage done"), FinalDamage));
	}
}

void UWeaponBarrelComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	FHitResult ShotResult;
	
	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());
	
	for(int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		FVector ShotEnd = ShotStart + FireRange * ShotDirection;

	#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubSystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
	#else
		bIsDebugEnabled = false;
	#endif
		
		switch (HitRegistration)
		{
		case EHitRegistrationType::Projectile:
			{
				LaunchProjectile(ShotStart, ShotDirection);
				break;
			}
		case EHitRegistrationType::HitScan:
			{
				bool bHasHit = HitScan(ShotStart, ShotDirection, ShotEnd, ShotResult);
				if(bIsDebugEnabled && bHasHit)
				{
					DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
				}
				break;
			}
		}
		
		if(bIsDebugEnabled)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
		}
	}
}

APawn* UWeaponBarrelComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if(!IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}
	return PawnOwner;
}

AController* UWeaponBarrelComponent::GetController() const
{
	APawn* PawnOwner = GetOwningPawn();
	if(IsValid(PawnOwner))
	{
		return PawnOwner->GetController();
	}
	return nullptr;
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
