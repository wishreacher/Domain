// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEquipmentComponent.h"
#include "Domain/Types.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/BaseCharacterMovementComponent.h"

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

	if(!MaxAmmunitionAmount.IsEmpty())
	{
		for(const TPair<EAmmunitionType, int32>& AmmoPair : MaxAmmunitionAmount)
		{
			AmmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
		}
	}
	
	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for(const TPair<EEquipmentSlots, TSubclassOf<AEquippableItem>>& ItemPair : ItemsLoadout)
	{
		if(!IsValid(ItemPair.Value))
		{
			continue;
		}
		AEquippableItem* Item = GetWorld()->SpawnActor<AEquippableItem>(ItemPair.Value);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		ItemsArray[(uint32)ItemPair.Key] = Item;

		if(IsValid(Item) && Item->GetIsDual())
		{
			CurrentSecondHandWeapon = GetWorld()->SpawnActor<AEquippableItem>(Item->GetClass());
			CurrentSecondHandWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetSecondHandUnEquippedSocketName());
			CurrentSecondHandWeapon->SetOwner(CachedBaseCharacter.Get());
		}
	}
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItem() const{

	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquippedItem))
	{
		Result = CurrentEquippedItem->GetItemType();
	}
	return Result;
}

ARangeWeapon* UCharacterEquipmentComponent::GetCurrentRangeWeapon() const
{
	if(IsValid(CurrentEquippedItem))
	{
		return Cast<ARangeWeapon>(CurrentEquippedItem);
	}
	return nullptr;
}

void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if(IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
		                                       CurrentEquippedItem->GetUnEquippedSocketName());
		if(CurrentEquippedItem->GetIsDual() && IsValid(CurrentSecondHandWeapon))
		{
			CurrentSecondHandWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
											   CurrentEquippedItem->GetSecondHandUnEquippedSocketName());
		}
	}
	
	if(IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
		CurrentRangeWeapon->EndReload(false);
		CurrentRangeWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangedHandle);
		CurrentRangeWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadedHandle);
	}
	
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
	                                       CurrentEquippedItem->GetEquippedSocketName());
	if(CurrentEquippedItem->GetIsDual() && IsValid(CurrentSecondHandWeapon))
	{
		CurrentSecondHandWeapon->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
										   CurrentEquippedItem->GetSecondHandEquippedSocketName());
	}
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if(bIsEquipping)
	{
		return;
	}
	if(CachedBaseCharacter->GetBaseCharacterMovementComponent()->IsMantling())
	{
		return;
	}
	UnEquipCurrentItem();
	
	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentRangeWeapon = Cast<ARangeWeapon>(CurrentEquippedItem);
	CurrentMeleeWeapon = Cast<AMeleeWeapon>(CurrentEquippedItem);
	if(IsValid(CurrentMeleeWeapon))
	{
		CachedBaseCharacter->SetCanAttack(true);
	}

	if(IsValid(CurrentEquippedItem))
	{
		UAnimMontage* EquipAnimMontage = CurrentEquippedItem->GetCharacterEquipAnimMontage();
		if(IsValid(EquipAnimMontage))
		{
			bIsEquipping = true;
			float EquipDuration =  CachedBaseCharacter->PlayAnimMontage(EquipAnimMontage);
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished,
				EquipDuration, false);
		} else
		{
			AttachCurrentItemToEquippedSocket();
		}
		
		CurrentEquippedSlot = Slot;
	}

	if(IsValid(CurrentRangeWeapon))
	{
		OnCurrentWeaponAmmoChangedHandle = CurrentRangeWeapon->OnAmmoChanged.AddUFunction(this, FName("CurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadedHandle = CurrentRangeWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		CurrentWeaponAmmoChanged(CurrentRangeWeapon->GetAmmo());
	}
	if(OnEquippedItemChanged.IsBound())
	{
		OnEquippedItemChanged.Broadcast(CurrentEquippedItem);
	}
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);

	while(CurrentSlotIndex != NextSlotIndex && !IsValid(ItemsArray[NextSlotIndex]))
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}
	
	if(CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = PreviousItemsArraySlotIndex(CurrentSlotIndex);

	while(CurrentSlotIndex != PreviousSlotIndex && !IsValid(ItemsArray[PreviousSlotIndex]))
	{
		PreviousSlotIndex = PreviousItemsArraySlotIndex(PreviousSlotIndex);
	}
	
	if(CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if(CurrentSlotIndex == ItemsArray.Num() - 1)
	{
		return 0;
	} else
	{
		return CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PreviousItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if(CurrentSlotIndex == 0)
	{
		return ItemsArray.Num() - 1;
	} else
	{
		return CurrentSlotIndex - 1;
	}
}

bool UCharacterEquipmentComponent::GetIsEquipping() const
{
	return bIsEquipping;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	checkf(IsValid(CurrentRangeWeapon), TEXT(""));
	int32 AvailableAmmunition = GetAvailableAmmunitionForCurrentWeapon();
	if(AvailableAmmunition <= 0)
	{
		return;
	}

	CurrentRangeWeapon->StartReload();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvailableAmmunition = GetAvailableAmmunitionForCurrentWeapon();
	
	int32 CurrentAmmo = CurrentRangeWeapon->GetAmmo();
	int32 AmmoToReload = CurrentRangeWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmmunition, AmmoToReload);
	if(NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}

	AmmunitionArray[(uint32)CurrentRangeWeapon->GetAmmoType()] -= ReloadedAmmo;
	CurrentRangeWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if(bCheckIsFull)
	{
		AvailableAmmunition = AmmunitionArray[(uint32)CurrentRangeWeapon->GetAmmoType()];
		bool bIsFullyReload = CurrentRangeWeapon->GetAmmo() == CurrentRangeWeapon->GetMaxAmmo();
		if(AvailableAmmunition == 0 || bIsFullyReload)
		{
			CurrentRangeWeapon->EndReload(true);
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

void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;
	AttachCurrentItemToEquippedSocket();
}


