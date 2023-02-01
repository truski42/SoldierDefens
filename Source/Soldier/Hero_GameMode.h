// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Hero_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class SOLDIER_API AHero_GameMode : public AGameMode
{
	GENERATED_BODY()

		AHero_GameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnyWhere)
		TSubclassOf<APawn> Chairs;


	UPROPERTY(EditAnyWhere)
		TSubclassOf<APawn> PlayerRecharge;
	
	float SpawnZ = 500.0f;

	UPROPERTY(EditAnywhere)
		float SpawnX_Min;

	UPROPERTY(EditAnywhere)
		float SpawnX_Max;

	UPROPERTY(EditAnywhere)
		float SpawnY_Min;

	UPROPERTY(EditAnywhere)
		float SpawnY_Max;

	void SpawnPlayerRecharge();
	void SpawnChairs();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int EnemyAlive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int WaveLevel;
};
