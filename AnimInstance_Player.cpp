// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance_Player.h"
#include "Character_Player.h"
#include "PlayerMovementComponent.h"

UAnimInstance_Player::UAnimInstance_Player() {
	Speed = 0.0f;
	isJump = false;
	isBlink = false;
}

void UAnimInstance_Player::NativeInitializeAnimation() {
	if (Player == nullptr) Player = Cast<ACharacter_Player>(TryGetPawnOwner());
	if (Player != nullptr && Comp_Move == nullptr) Comp_Move = Cast<UPlayerMovementComponent>(Player->Comp_Move);
}

void UAnimInstance_Player::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Player != nullptr) {
		Speed = Comp_Move->GetSpeed();
		isJump = Comp_Move->isJump;
		isBlink = Comp_Move->isBlink;
	}

}