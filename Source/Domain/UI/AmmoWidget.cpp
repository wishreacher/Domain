// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/UI/AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo, int32 MaxAmmo)
{
	Ammo = NewAmmo;
	TotalAmmo = NewTotalAmmo;
	if(NewAmmo == MaxAmmo)
	{
		bIsVisible = false;
	} else
	{
		bIsVisible = true;
	}
}