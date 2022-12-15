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

	UFUNCTION(BlueprintCallable)
	bool GetIsEquipping() const;
protected:
	virtual void BeginPlay() override;

private:
	bool bIsEquipping = false;
	

	TWeakObjectPtr<ABaseCharacter> CachedBaseCharacter;
};
