// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"
#include "Domain/Character/BaseCharacter.h"

void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapEnd);
	}
}

void AInteractiveActor::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
}

void AInteractiveActor::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
}

bool AInteractiveActor::IsOverlappingCharacterCapsule(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
    	if(!IsValid(BaseCharacter))
    	{
    		return false;
    	}
	return true;
}


