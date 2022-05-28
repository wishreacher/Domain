// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Domain/Actors/Equipable/Weapons/MeleeWeapon.h"
#include "TakeDownComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class DOMAIN_API UTakeDownComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTakeDownComponent();

	void BeginTakeDown();
	void ExecuteTakeDown(ETakeDownType Type, AActor* Target);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsExecutingTakeDown() const {return bIsExecutingTakeDown;}
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetShouldExecuteBehindTakeDown() const {return bShouldExecuteBehindTakeDown;}
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetShouldExecuteTopTakeDown() const {return bShouldExecuteTopTakeDown;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetShouldExecuteBehindCooldown(bool bNewShouldExecute) {bShouldExecuteBehindTakeDown = bNewShouldExecute;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetShouldExecuteTopCooldown(bool bNewShouldExecute) {bShouldExecuteTopTakeDown = bNewShouldExecute;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetBehindTakeDownTarget(AActor* NewBehindTakeDownTarget) {BehindTakeDownTarget = NewBehindTakeDownTarget;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetTopTakeDownTarget(AActor* NewTopTakeDownTarget) {TopTakeDownTarget = NewTopTakeDownTarget;}

	bool CanExecuteTakeDown() const;

	UFUNCTION(BlueprintImplementableEvent)
	void SetupTarget();
	
	UFUNCTION(BlueprintImplementableEvent)
	void MoveToTakeDownTarget(AActor* TargetActor, FVector Offset);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayTakeDownMontages(FTakeDownDescription TakeDownDescription);

	UFUNCTION(BlueprintImplementableEvent)
	void EnemyTakeDownInterfaceCall(AActor* Target, FTakeDownDescription TakeDown);

	UFUNCTION(BlueprintImplementableEvent)
	void TakeDownCameraRigActivation(FTakeDownDescription TakeDown);

	UFUNCTION(BlueprintNativeEvent)
	void TakeDownEnd();

protected:
	virtual void BeginPlay() override;
private:
	bool bIsExecutingTakeDown;
	bool bShouldExecuteBehindTakeDown;
	bool bShouldExecuteTopTakeDown;

	FTimerHandle TakeDownTimerHandle;
	
	AActor* BehindTakeDownTarget;
	AActor* TopTakeDownTarget;

	TWeakObjectPtr<ABaseCharacter> CachedBaseCharacter;
};
