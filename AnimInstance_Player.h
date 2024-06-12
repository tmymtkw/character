// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstance_Player.generated.h"

class ACharacter_Player;
class UPlayerMovementComponent;

UCLASS()
class MYPROJECT_API UAnimInstance_Player : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UAnimInstance_Player();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	TObjectPtr<ACharacter_Player> Player;

	UPROPERTY()
	TObjectPtr<UPlayerMovementComponent> Comp_Move;

	float Speed;

	UPROPERTY(BlueprintReadOnly)
	bool isJump;

	bool isBlink;
};
