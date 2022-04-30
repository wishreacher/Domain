// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SlowMotion.generated.h"

/**
 * 
 */
UCLASS()
class DOMAIN_API UAnimNotifyState_SlowMotion : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = 0, ClampMin = 0, UIMax = 2.f, ClampMax = 2.f))
	float SlowMotionMultiplier = 1.f;

private:
	float PreviousTimeDilation;
};
