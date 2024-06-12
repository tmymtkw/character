// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimInstance_Test.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UAnimInstance_Test : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite)
	float speed;

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
