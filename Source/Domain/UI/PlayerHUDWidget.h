// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.h"
#include "CrosshairWidget.h"
#include "CharacterAttributesWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class DOMAIN_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	class UCrosshairWidget* GetCrosshairWidget();
	class UAmmoWidget* GetAmmoWidget();
	class UCharacterAttributesWidget* GetCharacterAttributesWidget();
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Names")
	FName CrosshairWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Names")
	FName AmmoWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Names")
	FName AttributesWidgetName;
};
