// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/UI/PlayerHUDWidget.h"
#include "Blueprint/WidgetTree.h"

UCrosshairWidget* UPlayerHUDWidget::GetCrosshairWidget()
{
	return WidgetTree->FindWidget<UCrosshairWidget>(CrosshairWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UCharacterAttributesWidget* UPlayerHUDWidget::GetCharacterAttributesWidget()
{
	return WidgetTree->FindWidget<UCharacterAttributesWidget>(AttributesWidgetName);
}