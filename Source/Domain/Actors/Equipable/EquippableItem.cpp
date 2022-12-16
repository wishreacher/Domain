// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableItem.h"

#include "Components/BoxComponent.h"

AEquippableItem::AEquippableItem()
{
	InteractCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision"));
}

EEquipableItemType AEquippableItem::GetItemType() const
{
	return EquippedItemType;
}

FName AEquippableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}

FName AEquippableItem::GetUnEquippedSocketName() const
{
	return UnEquippedSocketName;
}

UAnimMontage* AEquippableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}

UAnimMontage* AEquippableItem::GetCharacterUnEquipAnimMontage() const
{
	return CharacterUnEquipAnimMontage;
}

void AEquippableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if(IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("AEquippableItem::SetOwner only BaseCharacter can own equipable item"))
		CachedCharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	} else
	{
		CachedCharacterOwner = nullptr;
	}
}

ABaseCharacter* AEquippableItem::GetCharacterOwner() const
{
	return IsValid(CachedCharacterOwner) ? CachedCharacterOwner : nullptr;
}