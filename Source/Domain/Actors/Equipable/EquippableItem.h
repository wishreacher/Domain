// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Types.h"
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
	UAnimMontage* GetCharacterEquipAnimMontage() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquipableItemType EquippedItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;
};