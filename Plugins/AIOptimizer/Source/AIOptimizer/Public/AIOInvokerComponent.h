// Copyright (C) 2021 Grzegorz Szewczyk - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIOInvokerComponent.generated.h"

class UUserWidget;


/**
 * Component which should be added to actors to whom all calculations will be made to.
 * In most cases it should be added to your PlayerCharacter blueprint.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIOPTIMIZER_API UAIOInvokerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIOInvokerComponent();

	// BEGIN UActorComponent
	virtual void OnRep_IsActive() override;
	virtual void OnComponentDestroyed( bool bDestroyingHierarchy ) override;
	virtual void Activate( bool bReset = false ) override;
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;
	virtual void InitializeComponent() override;
	// END UActorComponent



protected:
	void RegisterInvoker();
	void UnregisterInvoker();

	/************************************************************************/
	/*							DEBUG                                       */
	/************************************************************************/
public:
	UFUNCTION( BlueprintCallable, Category = Debug )
	void DebugAIOptimizer( bool bDebug );


	

protected:
	UPROPERTY()
	UUserWidget* DebugWidget;
	/************************************************************************/


};
