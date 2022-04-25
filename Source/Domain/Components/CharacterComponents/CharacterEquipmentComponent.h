// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Domain/Actors/Equipable/Weapons/RangeWeapon.h"
#include "Components/ActorComponent.h"
#include "Domain/Types.h"
#include "Domain/Actors/Equipable/Weapons/MeleeWeapon.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<int32, TInlineAllocator<(uint32)EAmmunitionType::MAX>> TAmmunitionArray;
typedef TArray<class AEquippableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChangedEvent, int32, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquippableItem*)

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
	FOnEquippedItemChanged OnEquippedItemChanged;
	
	UFUNCTION(BlueprintCallable)
	EEquipableItemType GetCurrentEquippedItem() const;

	UFUNCTION(BlueprintCallable)
	ARangeWeapon* GetCurrentRangeWeapon() const;
	
	void UnEquipCurrentItem();

	UFUNCTION(BlueprintCallable)
	void AttachCurrentItemToEquippedSocket();

	UFUNCTION(BlueprintCallable)
	void EquipItemInSlot(EEquipmentSlots Slot);

	void EquipNextItem();
	void EquipPreviousItem();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex);

	UFUNCTION(BlueprintCallable)
	bool GetIsEquipping() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AMeleeWeapon* GetCurrentMeleeWeapon() const {return CurrentMeleeWeapon;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TMap<EEquipmentSlots, TSubclassOf<class AEquippableItem>> GetCurrentLoadout() const {return ItemsLoadout;}
	
	FORCEINLINE EEquipmentSlots GetCurrentEquippedSlot() const {return CurrentEquippedSlot;}
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmmunitionType, int32> MaxAmmunitionAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquippableItem>> ItemsLoadout;

private:
	bool bIsEquipping = false;
	
	void CreateLoadout();

	UFUNCTION()
	void CurrentWeaponAmmoChanged(int32 Ammo);

	int32 GetAvailableAmmunitionForCurrentWeapon();
	
	TAmmunitionArray AmmunitionArray;
	TItemsArray ItemsArray;

	void EquipAnimationFinished();

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadedHandle;

	FTimerHandle EquipTimer;

	EEquipmentSlots CurrentEquippedSlot;
	AEquippableItem* CurrentEquippedItem;
	ARangeWeapon* CurrentRangeWeapon;
	AMeleeWeapon* CurrentMeleeWeapon;
	TWeakObjectPtr<class ABaseCharacter> CachedBaseCharacter;
};
