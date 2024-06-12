// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode_InGame.h"
#include "Character_Player.h"
#include "Character_Player2.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "StatusHUD.h"

AGameMode_InGame::AGameMode_InGame() {
	DefaultPawnClass = ACharacter_Player2::StaticClass();
	HUDClass = AStatusHUD::StaticClass();
}

void AGameMode_InGame::BeginPlay() {
	Super::BeginPlay();

	const APlayerStart* PlayerStart = 
		Cast<APlayerStart>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()));

	SpawnTransform = PlayerStart->GetActorTransform();
}

void AGameMode_InGame::Respawn() {
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	TObjectPtr<ACharacter_Player2> Player = GetWorld()->SpawnActor<ACharacter_Player2>(ACharacter_Player2::StaticClass(), SpawnTransform, SpawnParams);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->Possess(Player);
}

void AGameMode_InGame::KillPlayer(TObjectPtr<ACharacter_Player2> Player) {
	Player->Destroy();

	Respawn();
}