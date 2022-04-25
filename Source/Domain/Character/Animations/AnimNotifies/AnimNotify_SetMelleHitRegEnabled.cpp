// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Character/Animations/AnimNotifies/AnimNotify_SetMelleHitRegEnabled.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_SetMelleHitRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ABaseCharacter* CharacterOwner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(!IsValid(CharacterOwner))
	{
		return;
	}
	AMeleeWeapon* MeleeWeapon = CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeapon();
	if(IsValid(MeleeWeapon))
	{
		MeleeWeapon->SetIsHitRegistrationEnabled(bIsHitRegistrationEnabled);
	}
}
