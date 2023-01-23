// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultEnemy.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Test/Hero.h"


// Sets default values
ADefaultEnemy::ADefaultEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	isDead = false;
	hasTakenDamage = false;

	health = 1.0f;
	expWorth = 0.0f;
	defaultHealth = health;
	
	DetectPlayerCollisionSphere =
		CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));

	DetectPlayerCollisionSphere->SetupAttachment(RootComponent); 
}

// Called when the game starts or when spawned
void ADefaultEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADefaultEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADefaultEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADefaultEnemy::TakeDamage(float _damage)
{
	health -= _damage;
	if (health <= 0.0f) {
		Die();
	}
	else {
		hasTakenDamage = true;
	}
}

void ADefaultEnemy::Die()
{
	isDead = true;
}

USphereComponent* ADefaultEnemy::GetDetectPlayerCollisionSphere()
{
	return DetectPlayerCollisionSphere;
}

