// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Character/Animations/AnimNotifies/AnimNotifyState_Combo.h"

#include "Domain/Character/Animations/BaseCharacterAnimInstance.h"

void UAnimNotifyState_Combo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	UBaseCharacterAnimInstance* AnimInstance = Cast<UBaseCharacterAnimInstance>(MeshComp->GetAnimInstance());
	if(IsValid(AnimInstance))
	{
		AnimInstance->SetNextComboSegment(NextLightAttack, NextHeavyAttack);
	}
}

void UAnimNotifyState_Combo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	UBaseCharacterAnimInstance* AnimInstance = Cast<UBaseCharacterAnimInstance>(MeshComp->GetAnimInstance());
	if(IsValid(AnimInstance))
	{
		AnimInstance->SetDefaultCombos();
	}
}
