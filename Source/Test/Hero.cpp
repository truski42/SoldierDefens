// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Public/HealthComponent.h"
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
	staminaAttackUsageRate = 0.1;
	staminaRechargeRate = 0.1f;

	// Health
	DefaultHealth = 100.f;
	Health = DefaultHealth;
	bDoOnce = true;
}



// Called when the game starts or when spawned
void AHero::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bDead) {

		if (bIsCrouched) {
			isSprinting = false;
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

		// Taking staming sprinting
		if (isSprinting)
		{
			if (currentStamina > 0.0f) {
				currentStamina = FMath::FInterpConstantTo(currentStamina, 0.0f, DeltaTime, staminaSprintUsageRate);
			}
			else
			{
				EndWalking();
			}
		}
		else
		{
			if (currentStamina < maxStamina)
			{
				currentStamina = FMath::FInterpConstantTo(currentStamina, maxStamina, DeltaTime, staminaSprintUsageRate);
			}
		}

		if (GetCharacterMovement()->IsFalling())
		{
			if (bDoOnce) {
				StartingHeight = GetActorLocation().Z;
				bDoOnce = false;
				if (GEngine)
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f = FloatVariable"), StartingHeight));
			}
		}
	}

}

void AHero::Landed(const FHitResult& Hit)
{
	Hit.GetActor();
	EndHeight = GetActorLocation().Z;
	Difference = StartingHeight - EndHeight;
	if (Difference >= 1000) {
		Health = 0;
	}
	else
	{
		Health = Health - 30.0f;
	}
}


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




}

// Movement Section
void AHero::MoveForward(float Axis)
{

	if (!bDead) {
		if (!isClimbing) {
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
		if (!isClimbing) {
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
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
}
void AHero::StopAttcking() 
{
	isAttacking = false;
}


// Restart Game after dead

void AHero::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
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