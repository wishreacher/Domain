// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "InteractiveCameraActor.generated.h"

/**
 * 
 */
UCLASS()
class DOMAIN_API AInteractiveCameraActor : public ACameraActor
{
	GENERATED_BODY()
	
public:
	AInteractiveCameraActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition Settings", meta = (ClampMin = 0.f, UIMin = 0.f))
	float TransitionToCameraTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition Settings", meta = (ClampMin = 0.f, UIMin = 0.f))
	float TransitionToPawnTime = 2.0f;
	
};
