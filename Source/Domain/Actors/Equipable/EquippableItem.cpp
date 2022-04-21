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
