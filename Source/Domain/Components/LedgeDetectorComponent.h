
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedgeDetectorComponent.generated.h"

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

	/** Player's location for detected ledge (considering a player's capsule half-height) */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Description")
	FVector Location;

	/* Ledge 2D normal (Without Z-coordinate). Returned by forward sweep test in a ULedgeDetectorComponent::DetectLedge()*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Description")
	FRotator Rotation;

	/** Player's rotation an a detected ledge. Orientation of negative ledge normal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ledge Description")
	FVector LedgeNormal;
};

/** Component for detecting available ledges for mantling movement
 *
 * Detects a suitable ledge (Collision chanel ECC_Climbing) with DetectLedge() method,
 * considering minimal and maximal ledge heights.
 * 
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DOMAIN_API ULedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**Detects ledge available for mantling
	 *
	 * Uses 3 geometric queries:
	 * 1. Forward test (Chanel ECC_Climbing): capsule sweep from a player's position in a movement direction by ForwardCheckDistance value.
	 *	  Capsule radius equals to player's capsule radius. Capsule height equals to (MaximumLedgeHeight - MinimumLedgeHeight)
	 * 2. Downward test (Chanel ECC_Climbing): sphere sweep from a ledge maximum height (considering a current players's bottom and sweep radius).
	 *	  Radius equals to player's capsule radius
	 * 3. Overlap test (Pawn profile): checks that player's capsule doesn't overlap with anything at a ledge's position
	 *
	 * @param LedgeDescription - output parameter set to a valid value when a ledge is detected
	 * @return true when a ledge is detected, false - otherwise
	*/
	bool DetectLedge(OUT FLedgeDescription& LedgeDescription);
	
protected:
	virtual void BeginPlay() override;

	/** Minimal height from player's bottom to detected ledge. Usually equals to MaxStepHeight from CharacterMovementComponent */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MinimumLedgeHeight = 40.0f;

	/** Maximal height from player's bottom to detected ledge. Determines maximal height of an obstacle available for mantling */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MaximumLedgeHeight = 200.0f;

	//* Maximum distance from player to detected ledge in a movement direction. Determines maximum distance when a player starts mantling */ 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float ForwardCheckDistance = 100.0f;
	
private:
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;
};
