// Fill out your copyright notice in the Description page of Project Settings.


#include "Domain/UI/CharacterAttributesWidget.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/Components/CharacterComponents/CharacterAttributeComponent.h"

float UCharacterAttributesWidget::GetHealthPercent() const
{
	float Result = 1.f;
	APawn* PlayerPawn = GetOwningPlayerPawn();
	ABaseCharacter* Character = Cast<ABaseCharacter>(PlayerPawn);
	
	if(IsValid(Character))
	{
		Result = Character->GetCharacterAttributeComponent()->GetHealthPercent();
	}
	return Result;
}

float UCharacterAttributesWidget::GetStaminaPercent() const
{
	float Result = 1.f;
	APawn* PlayerPawn = GetOwningPlayerPawn();
	ABaseCharacter* Character = Cast<ABaseCharacter>(PlayerPawn);
	
	if(IsValid(Character))
	{
		Result = Character->GetCharacterAttributeComponent()->GetStaminaPercent();
	}
	return Result;
}