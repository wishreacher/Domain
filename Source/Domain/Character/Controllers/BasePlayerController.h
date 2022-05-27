// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/UI/PlayerHUDWidget.h"

#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DOMAIN_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;
	
protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;
private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void Mantle();
	void Jump();
	void ChangeCrouchState();
	void StartSprint();
	void StopSprint();
	void StartFire();
	void StopFire();
	void StartAim();
	void StopAim();
	void Reload();
	void NextItem();
	void PreviousItem();
	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();
	void EquipSideArmWeapon();
	void EquipPrimaryWeapon();
	void EquipSecondaryWeapon();
	void EquipMeleeWeapon();
	void TakeDown();

	UPlayerHUDWidget* PlayerHUDWidget = nullptr;
	void CreateAndInitializeWidgets();
	TSoftObjectPtr<class ABaseCharacter> CachedBaseCharacter;
};
