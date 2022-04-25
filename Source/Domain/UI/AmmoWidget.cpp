// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/UI/AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo, int32 NewMaxAmmo)
{
	Ammo = NewAmmo;
	TotalAmmo = NewTotalAmmo;
	MaxAmmo = NewMaxAmmo;
}