// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Domain/Actors/Equipable/EquippableItem.h"
#include "Domain/Actors/Projectiles/BaseProjectile.h"
#include "Throwable.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class DOMAIN_API AThrowable : public AEquippableItem
{
	GENERATED_BODY()

public:
	void Throw();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<ABaseProjectile> ProjectileClass;
};
