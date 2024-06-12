// Fill out your copyright notice in the Description page of Project Settings.


#include "APAtack.h"
#include "Components/BoxComponent.h"

// Sets default values
AAPAtack::AAPAtack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Comp_Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Comp_Box->SetHiddenInGame(false);
	Comp_Box->SetBoxExtent(FVector(100.0f, 50.0f, 50.0f), true);
	RootComponent = Comp_Box;

	GameplayTags.AddTag(FGameplayTag::RequestGameplayTag("Player.Attack"));

	this->Comp_Box->ComponentTags.Add(FName("Player"));
	this->Comp_Box->ComponentTags.Add(FName("Atack"));
}

// Called when the game starts or when spawned
void AAPAtack::BeginPlay()
{
	Super::BeginPlay();
	
	LeaveTime = 3.0f;
	SetLifeSpan(LeaveTime);
}

// Called every frame
void AAPAtack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

