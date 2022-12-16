// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_DeAttachEquippedItem.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_DeAttachEquippedItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ABaseCharacter* CharacterOwner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(IsValid(CharacterOwner))
	{
		if(UCharacterEquipmentComponent* Component = CharacterOwner->GetCharacterEquipmentComponent())
		{
			Component->DeAttachWeaponToSocket();
			Component->bIsEquipping = false;
			Component->bIsEquipped = false;
		}
	}
}
