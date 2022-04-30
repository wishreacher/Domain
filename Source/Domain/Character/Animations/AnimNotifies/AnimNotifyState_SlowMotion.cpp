// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Character/Animations/AnimNotifies/AnimNotifyState_SlowMotion.h"

#include "Domain/Character/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_SlowMotion::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(IsValid(BaseCharacter))
	{
		PreviousTimeDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
		BaseCharacter->SetTimeDilation(SlowMotionMultiplier);
	}
	
}

void UAnimNotifyState_SlowMotion::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(IsValid(BaseCharacter))
	{
		BaseCharacter->SetTimeDilation(PreviousTimeDilation);
	}
}
