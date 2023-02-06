// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultEnemy.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Soldier/Hero.h"
#include "Soldier/Hero_GameMode.h"



// Sets default values
ADefaultEnemy::ADefaultEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	isDead = false;
	hasTakenDamage = false;

	defaultHealth = 1.0f;
	expWorth = 0.0f;
	health = defaultHealth;
	EnemyStrenght = 1.0f;

	//EnemyLevel = 0;

	
	EnemyAttackCollisionBox =
		CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Sphere"));
	EnemyAttackCollisionBox->SetupAttachment(RootComponent);

	GetCharacterMovement()->MaxWalkSpeed = 400.f;

	GameModeWave = 0;

	//DetectPlayerCollisionSphere =
		//CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));

	//DetectPlayerCollisionSphere->SetupAttachment(RootComponent); 
}

// Called when the game starts or when spawned
void ADefaultEnemy::BeginPlay()
{
	Super::BeginPlay();
	AHero_GameMode* GameMode = (AHero_GameMode*)GetWorld()->GetAuthGameMode();
	//EnemyLevel = GameMode->WaveLevel;
	//for (int i = 0; i < GameModeWave; i++) {

	//}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Enemy Health %f"), defaultHealth));
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

void ADefaultEnemy::LevelUPStats() {
	defaultHealth += 2.0f;
	EnemyStrenght += 1.0f;
}

void ADefaultEnemy::Die()
{
	isDead = true;
}

void ADefaultEnemy::AttackEnemy(AActor* Actor)
{

}




