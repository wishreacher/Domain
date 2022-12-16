// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEquipmentComponent.h"
#include "Domain/Character/BaseCharacter.h"

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent  can be used only with BaseCharacter"));
	CachedBaseCharacter = StaticCast<ABaseCharacter*>(GetOwner());
}

void UCharacterEquipmentComponent::PickupWeapon(AMeleeWeapon* Weapon)
{
	CurrentWeapon = Weapon;
	DeAttachWeaponToSocket();
	bIsEquipped = false;
}

bool UCharacterEquipmentComponent::GetIsEquipping() const
{
	return bIsEquipping;
}

void UCharacterEquipmentComponent::StartEquipping()
{
	if(bIsEquipping)
	{
		return;
	}
	if(CachedBaseCharacter.IsValid())
	{
		if(CurrentWeapon)
		{
			bIsEquipping = true;
			CachedBaseCharacter->PlayAnimMontage(CurrentWeapon->GetCharacterEquipAnimMontage());
		}
	}
}

void UCharacterEquipmentComponent::StartUnEquipping()
{
	if(CachedBaseCharacter.IsValid())
	{
		if(CurrentWeapon)
		{
			bIsEquipping = true;
			CachedBaseCharacter->PlayAnimMontage(CurrentWeapon->GetCharacterUnEquipAnimMontage());
		}
	}
}

void UCharacterEquipmentComponent::AttachWeaponToSocket()
{
	CurrentWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		CurrentWeapon->GetEquippedSocketName());
}

void UCharacterEquipmentComponent::DeAttachWeaponToSocket()
{
	CurrentWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale,
		CurrentWeapon->GetUnEquippedSocketName());
}

void UCharacterEquipmentComponent::ToggleEquip()
{
	if(bIsEquipped)
	{
		StartUnEquipping();
	} else
	{
		StartEquipping();
	}
}
