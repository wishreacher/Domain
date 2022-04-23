// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/UI/CrosshairWidget.h"

void UCrosshairWidget::OnEquippedItemChanged_Implementation(const AEquippableItem* EquippedItem)
{
	CurrentEquippedItem = EquippedItem;
	SetupCurrentReticle();
}

void UCrosshairWidget::OnAimingStateChanged_Implementation(bool bIsAiming)
{
	SetupCurrentReticle();
}

void UCrosshairWidget::SetupCurrentReticle()
{
	CurrentReticle = CurrentEquippedItem.IsValid() ? CurrentEquippedItem->GetReticleType() : EReticleType::None;
}
