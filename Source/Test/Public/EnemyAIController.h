// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class TEST_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

private:

	APawn* PlayerPawn;

	class ADefaultEnemy* ControlledEnemy;

	class UNavigationSystemV1* NavArea;

	FVector RandomLocation = FVector();
	
	bool bMoveToPlayer;
	bool bSearchForPlayer;
	bool bCanAttackPlayer;

	float PlayerStoppingDistance = 60.f;
	float PlayerAttackDistance = 200.f;

	void GenerateRandomSearchLoctaion();
	void SearchForPlayer();

	void MoveToPlayer();
	void StartChasingPlayer();
	bool IsEnemyCloseToPlayer();
	void AttackPlayer();

	FTimerHandle SearchTimerHandle;
	FTimerHandle AttackTimerHandle;

public:
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UFUNCTION()
		void OnDetectPlayerBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnDetectPlayerEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
