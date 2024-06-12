// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character_Player.h"
#include "AIController_Test.generated.h"

class APawn;
class UBehaviorTree;
/**
 * 
 */
UCLASS()
class MYPROJECT_API AAIController_Test : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIController_Test(const FObjectInitializer& ObjectInitializer);

public:
	void SetPlayerKey(APawn* Player);

	UFUNCTION()
	ACharacter_Player* GetPlayerKey();

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> Comp_Behavior;

	UPROPERTY()
	TObjectPtr<UBlackboardComponent> Comp_Blackboard;

	UPROPERTY()
	FName PlayerKeyName;

protected:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UBehaviorTree> BehaviorTree;

	FORCEINLINE TObjectPtr<UBehaviorTreeComponent> GetBehaviorComp() const { return Comp_Behavior; }
	FORCEINLINE TObjectPtr<UBlackboardComponent> GetBlackboardComp() const { return Comp_Blackboard; }
};
