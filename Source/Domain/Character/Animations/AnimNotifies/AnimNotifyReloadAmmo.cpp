// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyReloadAmmo.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Domain/Character/BaseCharacter.h"

void UAnimNotifyReloadAmmo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ABaseCharacter* CharacterOwner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(!IsValid(CharacterOwner))
	{
		return;
	}
	CharacterOwner->GetCharacterEquipmentComponent()->ReloadAmmoInCurrentWeapon(NumberOfAmmo, true);
}
