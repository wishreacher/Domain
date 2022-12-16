// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Types.h"
#include "Domain/Character/BaseCharacter.h"
#include "GameFramework/Actor.h"
#include "EquippableItem.generated.h"

class UBoxComponent;
class UAnimMontage;

UCLASS()
class DOMAIN_API AEquippableItem : public AActor
{
	GENERATED_BODY()
	
public:
	AEquippableItem();
	EEquipableItemType GetItemType() const;
	FName GetEquippedSocketName() const;
	FName GetUnEquippedSocketName() const;
	UAnimMontage* GetCharacterEquipAnimMontage() const;
	UAnimMontage* GetCharacterUnEquipAnimMontage() const;
	virtual void SetOwner(AActor* NewOwner) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquipableItemType EquippedItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterUnEquipAnimMontage;

	ABaseCharacter* GetCharacterOwner() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* InteractCollision;

private:
	ABaseCharacter* CachedCharacterOwner;
};