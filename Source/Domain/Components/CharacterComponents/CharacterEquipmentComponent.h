// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Domain/Actors/Equipable/Weapons/RangeWeapon.h"
#include "Components/ActorComponent.h"
#include "Domain/Types.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<int32, TInlineAllocator<(uint32)EAmmunitionType::MAX>> TAmmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChangedEvent, int32, int32)

class ARangeWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOMAIN_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void ReloadCurrentWeapon();
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	UFUNCTION()
	void OnWeaponReloadComplete();

	FOnCurrentWeaponAmmoChangedEvent OnCurrentWeaponAmmoChangedEvent;
	EEquipableItemType GetCurrentEquippedItem() const;
	ARangeWeapon* GetCurrentRangeWeapon() const;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeapon> SideArmClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmmunitionType, int32> MaxAmmunitionAmount;

private:
	void CreateLoadout();

	UFUNCTION()
	void CurrentWeaponAmmoChanged(int32 Ammo);

	int32 GetAvailableAmmunitionForCurrentWeapon();
	
	TAmmunitionArray AmmunitionArray;
	ARangeWeapon* CurrentEquippedWeapon;
	TWeakObjectPtr<class ABaseCharacter> CachedBaseCharacter;
};
