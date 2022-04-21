// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Character/Animations/AnimNotifies/AnimNotify_AttachEquippedItem.h"

#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_AttachEquippedItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ABaseCharacter* CharacterOwner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(!IsValid(CharacterOwner))
	{
		return;
	}
	CharacterOwner->GetCharacterEquipmentComponent()->AttachCurrentItemToEquippedSocket();
}
