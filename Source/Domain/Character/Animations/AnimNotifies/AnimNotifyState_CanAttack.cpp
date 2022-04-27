// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Character/Animations/AnimNotifies/AnimNotifyState_CanAttack.h"
#include "Domain/Character/BaseCharacter.h"

void UAnimNotifyState_CanAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(IsValid(Character))
	{
		Character->SetCanAttack(false);
	}
}

void UAnimNotifyState_CanAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(IsValid(Character))
	{
		Character->SetCanAttack(true);
	}
}