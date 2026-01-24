// Neon Ascension - Base Character Implementation

#include "Characters/NeonCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

ANeonCharacterBase::ANeonCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Setup character movement
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.8f;
	GetCharacterMovement()->MaxWalkSpeed = GroundSpeed;
	GetCharacterMovement()->MaxFlySpeed = FlightSpeed;
	GetCharacterMovement()->BrakingDecelerationFlying = 1500.0f;

	// Create ability system component
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// Create aura component (will be initialized with Niagara system in Blueprint)
	AuraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("AuraComponent"));
	AuraComponent->SetupAttachment(RootComponent);
	AuraComponent->bAutoActivate = false;
}

void ANeonCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	CurrentStamina = MaxStamina;
	CurrentKi = 0.0f;
}

void ANeonCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == ECharacterState::Dead)
		return;

	// Update charging
	if (bIsCharging)
	{
		UpdateCharging(DeltaTime);
	}

	// Regenerate stamina when not flying or dashing
	if (CurrentState != ECharacterState::Flying && CurrentState != ECharacterState::Dashing && CurrentState != ECharacterState::Charging)
	{
		CurrentStamina = FMath::Min(CurrentStamina + 15.0f * DeltaTime, MaxStamina);
	}

	// Update aura effects
	UpdateAuraEffects(DeltaTime);

	// Handle dash timer
	if (CurrentState == ECharacterState::Dashing)
	{
		DashTimer -= DeltaTime;
		if (DashTimer <= 0.0f)
		{
			CurrentState = GetCharacterMovement()->IsMovingOnGround() ? ECharacterState::Idle : ECharacterState::Falling;
			OnStateChanged(CurrentState);
		}
	}

	// Handle attack timer
	if (CurrentState == ECharacterState::Attacking)
	{
		AttackTimer -= DeltaTime;
		if (AttackTimer <= 0.0f)
		{
			AttackCombo = 0;
			CurrentState = GetCharacterMovement()->IsMovingOnGround() ? ECharacterState::Idle : ECharacterState::Falling;
			OnStateChanged(CurrentState);
		}
	}
}

void ANeonCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Enhanced Input will be setup in Blueprint or derived class
}

UAbilitySystemComponent* ANeonCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ANeonCharacterBase::TakeDamageAmount(float Damage, FVector KnockbackDirection, float KnockbackForce)
{
	if (CurrentState == ECharacterState::Dead)
		return;

	CurrentHealth -= Damage;
	OnHealthChanged(CurrentHealth, MaxHealth);

	if (KnockbackForce > 0.0f && KnockbackDirection.Size() > 0.0f)
	{
		LaunchCharacter(KnockbackDirection.GetSafeNormal() * KnockbackForce, true, true);
	}

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void ANeonCharacterBase::StartCharging()
{
	if (CurrentState == ECharacterState::Dead)
		return;

	bIsCharging = true;
	CurrentState = ECharacterState::Charging;
	OnStateChanged(CurrentState);

	// Activate aura effect
	if (AuraComponent && AuraEffect)
	{
		AuraComponent->SetAsset(AuraEffect);
		AuraComponent->Activate(true);
	}
}

void ANeonCharacterBase::StopCharging()
{
	bIsCharging = false;
	ChargeIntensity = 0.0f;

	if (AuraComponent)
	{
		AuraComponent->Deactivate();
	}

	CurrentState = GetCharacterMovement()->IsMovingOnGround() ? ECharacterState::Idle : ECharacterState::Falling;
	OnStateChanged(CurrentState);
	OnChargeIntensityChanged(0.0f);
}

void ANeonCharacterBase::UpdateCharging(float DeltaTime)
{
	// Increase Ki while charging
	float ChargeRate = 40.0f;
	CurrentKi = FMath::Min(CurrentKi + ChargeRate * DeltaTime, MaxKi);
	OnKiChanged(CurrentKi, MaxKi);

	// Update charge intensity (0 to 1)
	ChargeIntensity = CurrentKi / MaxKi;
	OnChargeIntensityChanged(ChargeIntensity);

	// Update aura parameters if component exists
	if (AuraComponent && AuraComponent->IsActive())
	{
		AuraComponent->SetFloatParameter(TEXT("ChargeIntensity"), ChargeIntensity);
		AuraComponent->SetFloatParameter(TEXT("FlameHeight"), 2.0f + ChargeIntensity * 4.0f);
		AuraComponent->SetFloatParameter(TEXT("ParticleSpawnRate"), 50.0f + ChargeIntensity * 200.0f);
	}
}

void ANeonCharacterBase::UpdateAuraEffects(float DeltaTime)
{
	// Aura effect updates handled by Niagara system parameters
}

void ANeonCharacterBase::PerformLightAttack()
{
	if (CurrentState == ECharacterState::Dead || CurrentState == ECharacterState::Stunned)
		return;

	// If charging with enough Ki, perform burst instead
	if (bIsCharging && CurrentKi >= 30.0f)
	{
		PerformBurst();
		return;
	}

	CurrentState = ECharacterState::Attacking;
	AttackCombo = (AttackCombo + 1) % 3;
	AttackTimer = 0.4f;
	OnStateChanged(CurrentState);
}

void ANeonCharacterBase::PerformHeavyAttack()
{
	if (CurrentState == ECharacterState::Dead || CurrentState == ECharacterState::Stunned)
		return;

	// If charging with enough Ki, fire beam
	if (bIsCharging && CurrentKi >= 40.0f)
	{
		FireBeam();
		return;
	}

	CurrentState = ECharacterState::Attacking;
	AttackTimer = 0.6f;
	OnStateChanged(CurrentState);
}

void ANeonCharacterBase::PerformDash(FVector Direction)
{
	if (CurrentStamina < 20.0f || CurrentState == ECharacterState::Dead)
		return;

	CurrentStamina -= 20.0f;
	DashDirection = Direction.GetSafeNormal();
	DashTimer = DashDuration;
	CurrentState = ECharacterState::Dashing;

	LaunchCharacter(DashDirection * DashSpeed, true, true);
	OnStateChanged(CurrentState);
}

void ANeonCharacterBase::StartFlight()
{
	if (CurrentStamina < 10.0f || CurrentState == ECharacterState::Dead)
		return;

	bIsFlying = true;
	CurrentState = ECharacterState::Flying;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	OnStateChanged(CurrentState);
}

void ANeonCharacterBase::StopFlight()
{
	bIsFlying = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	CurrentState = ECharacterState::Falling;
	OnStateChanged(CurrentState);
}

void ANeonCharacterBase::FireBeam()
{
	if (CurrentKi < 40.0f || CurrentState == ECharacterState::Dead)
		return;

	CurrentKi -= 40.0f;
	OnKiChanged(CurrentKi, MaxKi);

	StopCharging();
	CurrentState = ECharacterState::BeamAttack;
	OnStateChanged(CurrentState);

	// Beam logic will be implemented in Blueprint with Niagara
}

void ANeonCharacterBase::PerformBurst()
{
	if (CurrentKi < 30.0f || CurrentState == ECharacterState::Dead)
		return;

	CurrentKi -= 30.0f;
	OnKiChanged(CurrentKi, MaxKi);

	StopCharging();
	CurrentState = ECharacterState::BurstAttack;
	OnStateChanged(CurrentState);

	// Burst logic will be implemented in Blueprint with Niagara
}

void ANeonCharacterBase::Die()
{
	CurrentState = ECharacterState::Dead;
	OnStateChanged(CurrentState);
	OnDeath();

	// Disable collision and movement
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
}
