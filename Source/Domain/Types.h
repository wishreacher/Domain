#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_Bullet ECC_GameTraceChannel2
#define ECC_Melee ECC_GameTraceChannel3

const FName SocketFPCamera = FName("CameraSocket");

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfileRagdoll = FName("Ragdoll");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");
const FName CollisionProfileNoCollision = FName("NoCollision");

const FName DebugCategoryLedgeDetection = FName ("LedgeDetection");
const FName DebugCategoryCharacterAttributes = FName ("CharacterAttributes");

const FName SectionMontageReloadEnd = FName ("ReloadEnd");

UENUM(Blueprintable)
enum class EEquipableItemType : uint8
{
	None,
	Ken,
	Tanken,
	MAX UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EMeleeAttackType : uint8
{
	None,
	PrimaryAttack,
	SecondaryAttack,
	MAX UMETA(Hidden)
};

UENUM(Blueprintable)
enum class ETakeDownType : uint8
{
	None,
	Behind,
	Top,
	MAX UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EFractions : uint8
{
	None,
	Bolshevik,
	Neutral,
	UNO,
	MAX UMETA(Hidden)
};