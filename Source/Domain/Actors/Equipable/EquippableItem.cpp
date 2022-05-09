// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableItem.h"

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

FName AEquippableItem::GetSecondHandEquippedSocketName() const
{
	return SecondHandEquippedSocketName;
}

FName AEquippableItem::GetSecondHandUnEquippedSocketName() const
{
	return SecondHandUnEquippedSocketName;
}

bool AEquippableItem::GetIsDual() const
{
	return bIsWeaponDual;
}

UAnimMontage* AEquippableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}

EReticleType AEquippableItem::GetReticleType() const
{
	return ReticleType;
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