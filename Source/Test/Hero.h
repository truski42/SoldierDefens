// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Blueprint/UserWidget.h"

#include "Hero.generated.h"

UCLASS()
class TEST_API AHero : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHero();

	// Camera 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent* FollowCamera;

	bool bDead;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
		float Power;

	UPROPERTY(EditAnyWhere)
		float Power_Treshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isClimbing;

	// Function OnBeginOverlap with Actor

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	//Power Widget

	UPROPERTY(EditAnyWhere, Category = "UI HUD")
		TSubclassOf<UUserWidget> Player_Power_Widget_Class;

	UUserWidget* Player_Power_Widget;


	// Crouching Section

	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void CalcCamera(float DelataTime, struct FMinimalViewInfo& OutResult) override;
	void StartCrouch();
	void EndCrouch();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
		FVector CrouchEyeOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
		float CrouchSpeed;

	// Movement Section

	void StartWalking();
	void EndWalking();
	void MoveForward(float Axis);
	void MoveRight(float Axis);

	// Attack Section
	void Attack();


	//Restart Game Function
	void RestartGame();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// The amount of stamina player is currently has.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float currentStamina;

	// The amount of max stamina player can have at once.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float maxStamina;

	// The rate stamina is is used when sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float staminaSprintUsageRate;

	// The rate stamina is recharged
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float staminaRechargeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool isSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool isAttacking;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
