// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Domain/Types.h"
#include "Domain/Actors/Equipable/EquippableItem.h"
#include "CrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class DOMAIN_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAimingStateChanged(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquippedItemChanged(const AEquippableItem* EquippedItem);

	UPROPERTY(BlueprintReadOnly, Category = "Reticle")
	EReticleType CurrentReticle;

private:
	TWeakObjectPtr<const AEquippableItem> CurrentEquippedItem;

	void SetupCurrentReticle();
};
