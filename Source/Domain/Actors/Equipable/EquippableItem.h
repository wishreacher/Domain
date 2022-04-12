// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Types.h"
#include "GameFramework/Actor.h"
#include "EquippableItem.generated.h"

UCLASS()
class DOMAIN_API AEquippableItem : public AActor
{
	GENERATED_BODY()
	
public:
	EEquipableItemType GetItemType() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquipableItemType EquippedItemType = EEquipableItemType::None;
};
