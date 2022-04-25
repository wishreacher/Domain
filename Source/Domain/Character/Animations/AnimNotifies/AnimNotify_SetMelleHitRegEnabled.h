// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetMelleHitRegEnabled.generated.h"

/**
 * 
 */
UCLASS()
class DOMAIN_API UAnimNotify_SetMelleHitRegEnabled : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee Weapon")
	bool bIsHitRegistrationEnabled;
};
