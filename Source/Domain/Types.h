#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_Bullet ECC_GameTraceChannel2
#define ECC_Melee ECC_GameTraceChannel3

const FName SocketFPCamera = FName("CameraSocket");
const FName SocketCharacterWeapon = FName("WeaponSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponForeGrip= FName("ForeGripSocket");

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfileRagdoll = FName("Ragdoll");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");
const FName CollisionProfileNoCollision = FName("NoCollision");

const FName DebugCategoryLedgeDetection = FName ("LedgeDetection");
const FName DebugCategoryCharacterAttributes = FName ("CharacterAttributes");
const FName DebugCategoryRangeWeapon = FName ("RangeWeapon");

const FName FXParamTraceEnd = FName ("TraceEnd");

const FName SectionMontageReloadEnd = FName ("ReloadEnd");

UENUM(Blueprintable)
enum class EEquipableItemType : uint8
{
	None,
	Pistol,
	Rifle,
	SingleDagger,
	DualDaggers,
	MAX UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EAmmunitionType : uint8
{
	None,
	Pistol,
	Rifle,
	ShotgunShells,
	SniperRifleBullets,
	MAX UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EEquipmentSlots : uint8
{
	None,
	SideArm,
	PrimaryWeapon,
	SecondaryWeapon,
	MeleeWeapon,
	MAX UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EReticleType : uint8
{
	None,
	Default,
	SniperRifle,
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