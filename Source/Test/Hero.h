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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Functions

	//The blocking state
	UFUNCTION(BlueprintCallable)
		void StartBlocking();

	UFUNCTION(BlueprintCallable)
		void StopBlocking();

	// Crouching Section

	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	void CalcCamera(float DelataTime, struct FMinimalViewInfo& OutResult) override;
	void StartCrouch();
	void EndCrouch();

	// Movement Section

	void StartWalking();
	void EndWalking();
	void MoveForward(float Axis);
	void MoveRight(float Axis);

	// Attack Section
	UFUNCTION(BlueprintCallable)
		void AttackFunc();

	UFUNCTION(BlueprintCallable)
		void StopAttcking();

	//Restart Game Function
	UFUNCTION(BlueprintCallable)
		void RestartGame();

	//The character earns experience
	UFUNCTION(BlueprintCallable, Category = "Stats")
		void GainExperience(float _expAmount);


	// Variables
	
	// Camera 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
		FVector CrouchEyeOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
		float CrouchSpeed;

	// Max Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Health;
	// Default Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DefaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool onLadder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool nearLadder;

	// Climbing

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isClimbing;

	// The amount of stamina player is currently has.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float currentStamina;

	// The amount of max stamina player can have at once.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float maxStamina;

	// The rate stamina is is used when sprinting.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float staminaSprintUsageRate;

	// The rate stamina is used when attacking
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float staminaAttackUsageRate;

	// The rate stamina is recharged
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float staminaRechargeRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		bool isSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		bool isAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blocking")
		bool isBlocking;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DMG")
		float Strenght;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int currentLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		int upgradePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float experiencePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float experienceToLevel;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//virtual void Landed(const FHitResult& Hit) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
