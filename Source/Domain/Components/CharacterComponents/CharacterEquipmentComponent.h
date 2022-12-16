// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Domain/Actors/Equipable/Weapons/MeleeWeapon.h"
#include "CharacterEquipmentComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChangedEvent, int32, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquippableItem*)

class ARangeWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOMAIN_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	FOnCurrentWeaponAmmoChangedEvent OnCurrentWeaponAmmoChangedEvent;
	FOnEquippedItemChanged OnEquippedItemChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	AMeleeWeapon* CurrentWeapon;

	UFUNCTION(BlueprintCallable)
	void PickupWeapon(AMeleeWeapon* Weapon);

	UFUNCTION(BlueprintCallable)
	bool GetIsEquipping() const;

	UFUNCTION(BlueprintCallable)
	void StartEquipping();

	UFUNCTION(BlueprintCallable)
	void StartUnEquipping();

	void AttachWeaponToSocket();
	void DeAttachWeaponToSocket();
	void ToggleEquip();

	bool bIsEquipping = false;
	bool bIsEquipped = false;
protected:
	virtual void BeginPlay() override;

	TWeakObjectPtr<ABaseCharacter> CachedBaseCharacter;
};
