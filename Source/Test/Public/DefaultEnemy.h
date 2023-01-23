// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Test/Hero.h"
#include "DefaultEnemy.generated.h"

UCLASS()
class TEST_API ADefaultEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADefaultEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void TakeDamage(float _damage);

	UFUNCTION(BlueprintCallable)
		void Die();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enemy)
		bool isDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enemy)
		bool hasTakenDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enemy)
		float health;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enemy)
		float defaultHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enemy)
		float expWorth;

	class USphereComponent* GetDetectPlayerCollisionSphere();

private:
	UPROPERTY(EditAnywhere)
		class USphereComponent* DetectPlayerCollisionSphere;

};
