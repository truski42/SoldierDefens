// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Public/HealthComponent.h"
#include "DefaultEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

// Sets default values
AHero::AHero()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.0f, 88.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 430.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;


	bDead = false;
	onLadder = false;
	nearLadder = false;

	// Crouch Section

	CrouchEyeOffset = FVector(0.f);
	CrouchSpeed = 12.f;
	
	// Attacking and Blocking

	isAttacking = false;
	isBlocking = false;

	//Stamina Section

	isSprinting = false;
	currentStamina = 1.0f;
	maxStamina = 1.0f;
	staminaSprintUsageRate = 0.05f;
	staminaAttackUsageRate = 0.08;
	staminaRechargeRate = 0.1f;

	// Health
	DefaultHealth = 10.f;
	Health = DefaultHealth;
	
	// Level
	currentLevel = 1;
	upgradePoints = 5;
	experiencePoints = 0.0f;
	experienceToLevel = 2000.f;

	//Damage
	Strenght = 1;
	hasTakenDamage = false;


	// Targeting/locking
	bisLockedOn = false;
	maxTargetingDistance = 2000.0f;
	targetingHeightOffeset = 20.0f;
	lockedOnActor = nullptr;
}



// Called when the game starts or when spawned
void AHero::BeginPlay()
{
	Super::BeginPlay();
	
}

/*
void AHero::Landed(const FHitResult& Hit)
{
	Hit.GetActor();

}
*/


// Called to bind functionality to input
void AHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//MouseMovement
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);

	//Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this , &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::Jump);
	
	//Moving
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AHero::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AHero::MoveRight);

	//Crouching
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AHero::StartCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AHero::EndCrouch);

	//Walking
	PlayerInputComponent->BindAction("Walking", IE_Pressed, this, &AHero::StartWalking);
	PlayerInputComponent->BindAction("Walking", IE_Released, this, &AHero::EndWalking);

	// Atacking
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AHero::AttackFunc);

	// Blocking
	PlayerInputComponent->BindAction("Block", IE_Pressed, this, &AHero::StartBlocking);
	PlayerInputComponent->BindAction("Block", IE_Released, this, &AHero::StopBlocking);

	// Target Enemy
	PlayerInputComponent->BindAction("TargetEnemy", IE_Pressed, this, &AHero::ToggleLockOn);
	PlayerInputComponent->BindAction("ChangingTargetEnemy", IE_Pressed, this, &AHero::ChangeLockOnTarget);
	//PlayerInputComponent->BindAction("TargetEnemy", IE_Released, this, &AHero::ToggleLockOn);


}

// Movement Section
void AHero::MoveForward(float Axis)
{

	if (!bDead) {
		if (!isClimbing ) {
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(Direction, Axis);
		}

	}
}

void AHero::MoveRight(float Axis)
{
	if (!bDead) {
		if (!isClimbing && !isBlocking) {
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(Direction, Axis);
		}
	}

}

void AHero::StartWalking()
{
	isSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;

}

void AHero::EndWalking()
{
	isSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

// Attack and Block Section
void AHero::StartBlocking()
{
	isBlocking = true;
}

void AHero::StopBlocking()
{
	isBlocking = false;
}

void AHero::AttackFunc()
{
	isAttacking = true;
}
void AHero::StopAttcking() 
{
	isAttacking = false;
}
void AHero::TakeDamagePlayer(float _damage) {
	if (isBlocking) {
		float reducedDamage = _damage * 0.5;
		Health -= reducedDamage;
		if (Health <= 0.0f) {
			Die();
		}

	}
	else {
		Health -= _damage;
		if (Health <= 0.0f) {
			Die();
		}
	}

}
void AHero::Die() {
	bDead = true;

}


// Restart Game after dead

void AHero::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}


void AHero::GainExperience(float _expAmount)
{
	experiencePoints += _expAmount;

	if (experiencePoints >= experienceToLevel) {
		++currentLevel;

		experiencePoints -= experienceToLevel;
		experienceToLevel += 500.0f;
	}
}

void AHero::ToggleLockOn() {

	if (bisLockedOn) {
		bisLockedOn = false;
		lockedOnActor = nullptr;

		ToggleLockOnEffect();
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;
	}
	else {
		if (lockOnCandidates.Num() > 0) {
			AActor* closestActor = nullptr;
			closestActor = lockOnCandidates[0];
			//lockedOnActor = lockOnCandidates[0];
			
			for (int i = 0; i < lockOnCandidates.Num(); ++i) {
				if (GetDistanceTo(lockOnCandidates[i]) < GetDistanceTo(closestActor)) {
					closestActor = lockOnCandidates[i];
				}
			}
			lockedOnActor = closestActor;
			if (lockedOnActor)
			{
				bisLockedOn = true;
				ToggleLockOnEffect();

				bUseControllerRotationYaw = true;
				GetCharacterMovement()->bOrientRotationToMovement = true;

			}
		}
	}
}

void AHero::ChangeLockOnTarget()
{
	for (int i = 0; i < lockOnCandidates.Num(); ++i) {
		if (lockedOnActor == lockOnCandidates[i]) {
			if (i >= lockOnCandidates.Num() - 1) {
				lockedOnActor = lockOnCandidates[0];

				ToggleLockOnEffect();
				break;
			}
			else {
				lockedOnActor = lockOnCandidates[i + 1];

				ToggleLockOnEffect();
				break;
			}
		}
	}
}


// Crouching Section
void AHero::StartCrouch()
{
	Crouch();
}

void AHero::EndCrouch()
{
	UnCrouch();
}


void AHero::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}
	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	FollowCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
	GetCapsuleComponent()->SetCapsuleHalfHeight(55.0f);
	GetMesh()->SetRelativeLocation(FVector(-20.f, 0.f, -57.f));
}

void AHero::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}
	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;
	FollowCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
	GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
}

void AHero::CalcCamera(float DelataTime, FMinimalViewInfo& OutResult)
{
	if (FollowCamera) {
		FollowCamera->GetCameraView(DelataTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

// Called every frame
void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bDead) {

		if (bIsCrouched) {
			isSprinting = false;
			isAttacking = false;
			isBlocking = false;
			if (GetCharacterMovement()->Velocity.Size() > 1.0f) {
				GetCapsuleComponent()->SetCapsuleHalfHeight(70.0f);
				GetMesh()->SetRelativeLocation(FVector(-20.f, 0.f, -75.f));
			}
			else {
				GetCapsuleComponent()->SetCapsuleHalfHeight(55.0f);
				GetMesh()->SetRelativeLocation(FVector(-20.f, 0.f, -57.f));
			}
		}
		float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
		CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;

		// Taking Stamina after attack with hand


		if (isAttacking)
		{
			if (currentStamina > 0.1f) {
				currentStamina = FMath::FInterpConstantTo(currentStamina, 0.0f, DeltaTime, staminaAttackUsageRate);
			}
			else
			{
				StopAttcking();
			}
		}
		// Taking Stamina when blocking


		// Taking staming sprinting

		if (isSprinting || isBlocking)
		{
			if (currentStamina > 0.0f) {
				currentStamina = FMath::FInterpConstantTo(currentStamina, 0.0f, DeltaTime, staminaSprintUsageRate);
			}
			else
			{
				EndWalking();
				StopBlocking();
			}
		}
		else
		{
			if (currentStamina < maxStamina)
			{
				currentStamina = FMath::FInterpConstantTo(currentStamina, maxStamina, DeltaTime, staminaSprintUsageRate);
			}
		}
		
		if (bisLockedOn) {
			if (GetDistanceTo(lockedOnActor) <= maxTargetingDistance) {
				FRotator lockAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), lockedOnActor->GetActorLocation());
				lockAtRotation.Pitch -= targetingHeightOffeset;
				GetController()->SetControlRotation(lockAtRotation);
			}
			else {
				ToggleLockOn();
			}
		}
	}


}