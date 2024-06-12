// Fill out your copyright notice in the Description page of Project Settings.


#include "AIController_Test.h"
#include "UObject/ConstructorHelpers.h"

AAIController_Test::AAIController_Test(const FObjectInitializer& ObjectInitializer) {
	Comp_Behavior = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTree"));
	Comp_Blackboard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("Blackboard"));

	ConstructorHelpers::FObjectFinder<UBehaviorTree> BTFinder(TEXT("/Game/BP/BT_Enemy.BT_Enemy"));
	BehaviorTree = BTFinder.Object;

	PlayerKeyName = "Player";
}

void AAIController_Test::BeginPlay() {
	Super::BeginPlay();
}

void AAIController_Test::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);

	Comp_Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	Comp_Behavior->StartTree(*BehaviorTree);
}

void AAIController_Test::OnUnPossess() {
	Super::OnUnPossess();
	Comp_Behavior->StopTree();
}

void AAIController_Test::SetPlayerKey(APawn* Player) {
	ensure(Comp_Blackboard);

	Comp_Blackboard->SetValueAsObject(PlayerKeyName, Player);
}

ACharacter_Player* AAIController_Test::GetPlayerKey() {
	ensure(Comp_Blackboard);

	return Cast<ACharacter_Player>(Comp_Blackboard->GetValueAsObject(PlayerKeyName));
}