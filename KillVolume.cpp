// Fill out your copyright notice in the Description page of Project Settings.


#include "KillVolume.h"
#include "Components/BoxComponent.h"
#include "GameMode_InGame.h"
#include "Kismet/GameplayStatics.h"
#include "Character_Player.h"
#include "Character_Player2.h"

// Sets default values
AKillVolume::AKillVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneRoot;

	KillVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	KillVolume->SetupAttachment(RootComponent);
	KillVolume->SetHiddenInGame(false);

	KillVolume->OnComponentBeginOverlap.AddDynamic(this, &AKillVolume::OnBoxBeginOverlap);
}

// Called when the game starts or when spawned
void AKillVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKillVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKillVolume::OnConstruction(const FTransform& Transform) {
	KillVolume->SetBoxExtent(BoxExtent);
}

void AKillVolume::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp,
																		AActor* OtherActor, UPrimitiveComponent* OtherComp,
																		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (TObjectPtr<ACharacter_Player2> Player = Cast<ACharacter_Player2>(OtherActor)) {
		if (TObjectPtr<AGameMode_InGame> GameMode = Cast<AGameMode_InGame>(UGameplayStatics::GetGameMode(GetWorld()))) {
			GameMode->KillPlayer(Player);
		}
	}
}