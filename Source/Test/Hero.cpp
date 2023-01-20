// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
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
	Power = 100.0f;

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
	staminaRechargeRate = 0.1f;
}



// Called when the game starts or when spawned
void AHero::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AHero::OnBeginOverlap);

	if (Player_Power_Widget_Class != nullptr) {
		Player_Power_Widget = CreateWidget(GetWorld(), Player_Power_Widget_Class);
		Player_Power_Widget->AddToViewport();
	}
	
}


// Called every frame
void AHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AHero::Attack);

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

void AHero::Attack()
{
	isAttacking = true;
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
}

// BeginOverlap Section
void AHero::OnBeginOverlap(UPrimitiveComponent* HitComp, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Recharge")) {
		Power += 10.0f;

		if (Power > 100.0f) {
			Power = 100.0f;
		}
		OtherActor->Destroy();
	}

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