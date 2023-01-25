// Fill out your copyright notice in the Description page of Project Settings.


#include "Hero_GameMode.h"
#include "GameFramework/Actor.h"

AHero_GameMode::AHero_GameMode() {

	PrimaryActorTick.bCanEverTick = true;

}

void AHero_GameMode::BeginPlay()
{
	Super::BeginPlay();

	/*FTimerHandle UnsedHandle;
	GetWorldTimerManager().SetTimer(UnsedHandle, this, &AHero_GameMode::SpawnPlayerRecharge, 
		FMath::RandRange(2, 5), true);
		*/
}
void AHero_GameMode::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AHero_GameMode::SpawnPlayerRecharge()
{
	float RandX = FMath::RandRange(SpawnX_Min, SpawnX_Max);
	float RandY = FMath::RandRange(SpawnY_Min, SpawnY_Max);

	FVector SpawnPosition = FVector(RandX, RandY, SpawnZ);
	FRotator SpawnRotator = FRotator(0.0f, 0.0f, 0.0f);

	GetWorld()->SpawnActor(PlayerRecharge, &SpawnPosition, &SpawnRotator);	
}

void AHero_GameMode::SpawnChairs()
{
}
