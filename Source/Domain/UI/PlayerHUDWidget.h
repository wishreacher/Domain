// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	class UCharacterAttributesWidget* GetCharacterAttributesWidget();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Names")
	FName AttributesWidgetName;
};
