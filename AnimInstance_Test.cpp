// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance_Test.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"

void UAnimInstance_Test::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	TObjectPtr<ACharacter> Owner = Cast<ACharacter>(GetOwningActor());

	if (Owner != nullptr) {
		speed = Owner->GetVelocity().Size();
	}
}
