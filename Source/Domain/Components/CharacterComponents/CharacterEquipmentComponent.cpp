// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEquipmentComponent.h"

#include "Domain/Types.h"
#include "Domain/Character/BaseCharacter.h"

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent  can be used only with BaseCharacter"));
	CachedBaseCharacter = StaticCast<ABaseCharacter*>(GetOwner());
	CreateLoadout();
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AmmunitionArray.AddZeroed((uint32)EAmmunitionType::MAX);
	for(const TPair<EAmmunitionType, int32>& AmmoPair : MaxAmmunitionAmount)
	{
		AmmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}
	
	if(!IsValid(SideArmClass))
	{
		return;
	}
	CurrentEquippedWeapon = GetWorld()->SpawnActor<ARangeWeapon>(SideArmClass);
	CurrentEquippedWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketCharacterWeapon);
	CurrentEquippedWeapon->SetOwner(CachedBaseCharacter.Get());
	CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("CurrentWeaponAmmoChanged"));
	CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
	CurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItem() const
{
	EEquipableItemType Result = EEquipableItemType::None;
	if(IsValid(CurrentEquippedWeapon))
	{
		Result = CurrentEquippedWeapon->GetItemType();
	}
	return Result;
}

ARangeWeapon* UCharacterEquipmentComponent::GetCurrentRangeWeapon() const
{
	return CurrentEquippedWeapon;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	checkf(IsValid(CurrentEquippedWeapon), TEXT(""));
	int32 AvailableAmmunition = GetAvailableAmmunitionForCurrentWeapon();
	if(AvailableAmmunition <= 0)
	{
		return;
	}

	CurrentEquippedWeapon->StartReload();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvailableAmmunition = GetAvailableAmmunitionForCurrentWeapon();
	
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmmunition, AmmoToReload);
	if(NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if(bCheckIsFull)
	{
		AvailableAmmunition = AmmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()];
		bool bIsFullyReload = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
		if(AvailableAmmunition == 0 || bIsFullyReload)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::CurrentWeaponAmmoChanged(int32 Ammo)
{
	if(OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetAvailableAmmunitionForCurrentWeapon());
	}
}

int32 UCharacterEquipmentComponent::GetAvailableAmmunitionForCurrentWeapon()
{
	checkf(GetCurrentRangeWeapon(), TEXT(""));
	return AmmunitionArray[(uint32)GetCurrentRangeWeapon()->GetAmmoType()];
}


