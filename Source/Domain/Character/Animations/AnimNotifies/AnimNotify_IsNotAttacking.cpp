// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Character/Animations/AnimNotifies/AnimNotify_IsNotAttacking.h"

#include "Domain/Character/BaseCharacter.h"

void UAnimNotify_IsNotAttacking::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(IsValid(Character))
	{
		Character->SetIsAttacking(false);
	}
}
