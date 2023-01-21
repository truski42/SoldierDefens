// Fill out your copyright notice in the Description page of Project Settings.


#include "Ladder.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Test/Hero.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"



// Sets default values
ALadder::ALadder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LadderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LadderMesh"));
	LadderMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->AttachToComponent(LadderMesh, FAttachmentTransformRules::KeepRelativeTransform);
	Box->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnEndOverlap);

}

// Called when the game starts or when spawned
void ALadder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALadder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALadder::OnBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AHero* Player = Cast<AHero>(OtherActor))
	{
		Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		Player->onLadder = true;
	}
}

void ALadder::OnEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AHero* Player = Cast<AHero>(OtherActor))
	{
		Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		Player->onLadder = false;
	}
}


