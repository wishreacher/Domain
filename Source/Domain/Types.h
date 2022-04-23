#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_Bullet ECC_GameTraceChannel2

const FName SocketFPCamera = FName("CameraSocket");
const FName SocketCharacterWeapon = FName("WeaponSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponForeGrip= FName("ForeGripSocket");

const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfileRagdoll = FName("Ragdoll");
const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");

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
	MAX UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EAmmunitionType : uint8
{
	None,
	Pistol,
	Rifle,
	ShotgunShellss,
	MAX UMETA(Hidden)
};

UENUM(Blueprintable)
enum class EEquipmentSlots : uint8
{
	None,
	SideArm,
	PrimaryWeapon,
	SecondaryWeapon,
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