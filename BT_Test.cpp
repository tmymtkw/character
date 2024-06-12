// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_Test.h"
#include "Perception/PawnSensingComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Character_Player.h"
#include "AIController_Test.h"

// Sets default values
ABT_Test::ABT_Test()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Comp_PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	Comp_PawnSensing->SetPeripheralVisionAngle(60.0f);
	Comp_PawnSensing->SightRadius = 2000;
	Comp_PawnSensing->OnSeePawn.AddDynamic(this, &ABT_Test::OnSeePlayer);
}

// Called when the game starts or when spawned
void ABT_Test::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABT_Test::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABT_Test::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABT_Test::OnSeePlayer(APawn* Pawn) {
	TObjectPtr<AAIController_Test> AIController = Cast<AAIController_Test>(GetController());
	TObjectPtr<ACharacter_Player> Player = Cast<ACharacter_Player>(Pawn);

	if (AIController && Player) {
		AIController->SetPlayerKey(Player);
	}

	//UKismetSystemLibrary::PrintString(this, "See", true, false, FColor::Red, 1.0f);
}