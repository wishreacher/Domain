// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/Character/Animations/AnimNotifies/AnimNotify_AddCameraShake.h"

#include "Camera/CameraShakeSourceComponent.h"
#include "Domain/Character/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotify_AddCameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                        const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComp->GetOwner());
	if(IsValid(Player))
	{
		Player->GetCameraShakeComponent()->CameraShake = CameraShake;
		Player->GetCameraShakeComponent()->Start();
	}
	
}
