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
	GetCharacterMovement()->JumpZVelocity = 400.0f;
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
	/*
	Power -= DeltaTime * Power_Treshold;

	if (Power <= 0) {
		if (!bDead) {
			bDead = true;
			
			GetMesh()->SetSimulatePhysics(true);

			FTimerHandle UnsedHandle;
			GetWorldTimerManager().SetTimer(UnsedHandle, this, &AHero::RestartGame, 3.0f, false);

		}
	}
	*/
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
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AHero::EndWalking()
{
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
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

//Climbing
void AHero::ForwardTrace()
{
	TArray<FHitResult> OutHits;
	
	FVector ActorLocation = GetActorLocation();
	
	FVector ActorEnd = GetActorForwardVector();
	
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(10.f);
	
	const bool isHit = GetWorld()->SweepMultiByChannel(OutHits, ActorLocation, ActorEnd * 150 + ActorLocation, FQuat::Identity, ECC_Camera, MyColSphere);

	DrawDebugSphere(GetWorld(), ActorLocation, MyColSphere.GetSphereRadius(), 10, FColor::Red, true);
	
	/*if (isHit) {
		for (FHitResult const HitResult : OutHits)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, FString::Printf(TEXT("Hit: %s"), *HitResult.GetActor()->GetName()));
		}
	}
	*/
}
void AHero::HeightTrace()
{
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