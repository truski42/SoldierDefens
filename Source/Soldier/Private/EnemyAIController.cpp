// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"

#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

#include "DefaultEnemy.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Soldier/Hero.h"
#include "GameFramework/CharacterMovementComponent.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);

	bSearchForPlayer = true;
	bCanAttackPlayer = true;
	bMoveToPlayer = false;

	GenerateRandomSearchLoctaion();
	SearchForPlayer();

}

void AEnemyAIController::Tick(float DeltaTime)
{

}

void AEnemyAIController::GenerateRandomSearchLoctaion()
{
	RandomLocation = NavArea->GetRandomReachablePointInRadius(this, GetPawn()->GetActorLocation(), 1000.f);
}

void AEnemyAIController::SearchForPlayer()
{
	MoveToLocation(RandomLocation);
}

void AEnemyAIController::MoveToPlayer()
{
}

void AEnemyAIController::StartChasingPlayer()
{
}

bool AEnemyAIController::IsEnemyCloseToPlayer()
{
	return false;
}

void AEnemyAIController::AttackPlayer()
{
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (bSearchForPlayer)
	{
		GenerateRandomSearchLoctaion();
		SearchForPlayer();
	}
}

FRotator AEnemyAIController::GetControlRotation() const
{
	if (GetPawn() == nullptr)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}
	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void AEnemyAIController::OnDetectPlayerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AEnemyAIController::OnDetectPlayerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
