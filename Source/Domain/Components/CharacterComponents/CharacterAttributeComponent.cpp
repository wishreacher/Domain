// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeComponent.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Domain/Types.h"
#include "Domain/Character/BaseCharacter.h"
#include "Domain/SubSystems/DebugSubsystem.h"
#include "Domain/Components/BaseCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

UCharacterAttributeComponent::UCharacterAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UCharacterAttributeComponent can be used only with ABaseCharacter class"));
	checkf(MaxHealth > 0.f, TEXT("UCharacterAttributeComponent::GetHealthPercent() max health cannot be lower than 0"))
	
	Health = MaxHealth;
	Stamina = MaxStamina;

	CachedBaseCharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributeComponent::OnTakeAnyDamage);
}

void UCharacterAttributeComponent::UpdateStaminaValue(float DeltaTime)
{
	if (!CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting())
	{
		Stamina += StaminaRestoreVelocity * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
	}
	if (CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->IsSprinting())
	{
		Stamina -= StaminaConsumptionVelocity * DeltaTime;
		Stamina = FMath::Clamp(Stamina, 0.0f, MaxStamina);
	}
	if (Stamina<MaxStamina) 
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Stamina: %.1f"), Stamina));
	}
	if (FMath::IsNearlyZero(Stamina))
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Red, FString::Printf(TEXT("Stamina: %.1f"), Stamina));
		if(OnOutOfStaminaEvent.IsBound())
		{
			OnOutOfStaminaEvent.Broadcast(true);
		}
	}

	if(FMath::IsNearlyEqual(Stamina, MaxStamina))
	{

		if(OnOutOfStaminaEvent.IsBound())
		{
			OnOutOfStaminaEvent.Broadcast(false);
			CachedBaseCharacterOwner->GetBaseCharacterMovementComponent()->CanJumpAndSprint();
		}
	}
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if(!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}

	FVector HealthTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 5.0f) * FVector::UpVector;
	FVector StaminaTextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+ 15.0f) * FVector::UpVector;

	DrawDebugString(GetWorld(), HealthTextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);
	DrawDebugString(GetWorld(), StaminaTextLocation, FString::Printf(TEXT("Stamina: %.2f"), Stamina), nullptr, FColor::Blue, 0.0f, true);

}
#endif

void UCharacterAttributeComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if(!IsAlive())
	{
		return;
	}
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::OnTakeAnyDamage %s recieved %.2f amount of damage from %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());

	if(Health <= 0.0f)
	{
		if(OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
			UE_LOG(LogDamage, Warning, TEXT("Character %s is killed by an actor  %s"), *CachedBaseCharacterOwner->GetName(), *DamageCauser->GetName());
		}
	}
}

void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateStaminaValue(DeltaTime);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}

float UCharacterAttributeComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float UCharacterAttributeComponent::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}