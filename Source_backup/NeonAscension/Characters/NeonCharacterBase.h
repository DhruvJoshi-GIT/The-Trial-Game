// Neon Ascension - Base Character Class

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "NeonCharacterBase.generated.h"

class UAbilitySystemComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,
	Running,
	Jumping,
	Falling,
	Flying,
	Dashing,
	Attacking,
	Charging,
	BeamAttack,
	BurstAttack,
	Stunned,
	Dead
};

UCLASS()
class NEONASCENSION_API ANeonCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ANeonCharacterBase();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// === STATS ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 500.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float CurrentHealth = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxStamina = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxKi = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Stats")
	float CurrentKi = 0.0f;

	// === STATE ===
	UPROPERTY(BlueprintReadWrite, Category = "State")
	ECharacterState CurrentState = ECharacterState::Idle;

	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bIsFlying = false;

	UPROPERTY(BlueprintReadWrite, Category = "State")
	bool bIsCharging = false;

	UPROPERTY(BlueprintReadWrite, Category = "State")
	float ChargeIntensity = 0.0f;

	// === MOVEMENT ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float GroundSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float FlightSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashDuration = 0.25f;

	// === AURA EFFECTS ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UNiagaraSystem* AuraEffect;

	UPROPERTY(BlueprintReadOnly, Category = "Effects")
	UNiagaraComponent* AuraComponent;

	// === FUNCTIONS ===
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void TakeDamageAmount(float Damage, FVector KnockbackDirection, float KnockbackForce);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCharging();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopCharging();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformLightAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformHeavyAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformDash(FVector Direction);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartFlight();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopFlight();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireBeam();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformBurst();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnHealthChanged(float NewHealth, float MaxHealthValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnKiChanged(float NewKi, float MaxKiValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnStateChanged(ECharacterState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnChargeIntensityChanged(float Intensity);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnDeath();

protected:
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystemComponent;

	void UpdateCharging(float DeltaTime);
	void UpdateAuraEffects(float DeltaTime);
	void Die();

private:
	float DashTimer = 0.0f;
	FVector DashDirection;
	int32 AttackCombo = 0;
	float AttackTimer = 0.0f;
};
