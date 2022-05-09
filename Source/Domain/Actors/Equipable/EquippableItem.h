// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Types.h"
#include "Domain/Character/BaseCharacter.h"
#include "GameFramework/Actor.h"
#include "EquippableItem.generated.h"

class UAnimMontage;

UCLASS()
class DOMAIN_API AEquippableItem : public AActor
{
	GENERATED_BODY()
	
public:
	EEquipableItemType GetItemType() const;
	FName GetEquippedSocketName() const;
	FName GetUnEquippedSocketName() const;
	FName GetSecondHandEquippedSocketName() const;
	FName GetSecondHandUnEquippedSocketName() const;
	bool GetIsDual() const;
	UAnimMontage* GetCharacterEquipAnimMontage() const;
	virtual EReticleType GetReticleType() const;
	virtual void SetOwner(AActor* NewOwner) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquipableItemType EquippedItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	bool bIsWeaponDual = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item", meta=(EditCondition = "bIsWeaponDual == true"))
	FName SecondHandEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item", meta=(EditCondition = "bIsWeaponDual == true"))
	FName SecondHandUnEquippedSocketName = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item|Reticle")
	EReticleType ReticleType = EReticleType::None;

	ABaseCharacter* GetCharacterOwner() const;

private:
	ABaseCharacter* CachedCharacterOwner;
};