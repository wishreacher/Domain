// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/UI/PlayerHUDWidget.h"
#include "Blueprint/WidgetTree.h"

UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget()
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(AttributesWidgetName);
}