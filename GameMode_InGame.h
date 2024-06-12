// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameMode_InGame.generated.h"

class ACharacter_Player2;



UCLASS()
class MYPROJECT_API AGameMode_InGame : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGameMode_InGame();

protected:
	virtual void BeginPlay() override;

public:
	FTransform SpawnTransform;

	void KillPlayer(TObjectPtr<ACharacter_Player2> Player);

private:
	void Respawn();
};
