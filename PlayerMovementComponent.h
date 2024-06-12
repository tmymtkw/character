// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UPlayerMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	UPlayerMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool isBlink;
	bool isJump;
	bool isGrounded;
	FVector MoveInput;
	FVector CameraInput;

	void SetAnglePitch(float dp);
	void SetAngleYaw(float dy);
	FRotator GetCameraAngle();
	FRotator GetBodyAngle();
	void SetJump();
	void SetBlink();
	void SetSprint();
	void SetLock(bool flag);
	void CheckGround(FHitResult Hit);
	bool CheckBlinkTime(float Time);
	float GetSpeed();

private:
	bool isSprint;
	bool isLocked;
	float SPEED_WALK;
	float SPEED_SPRINT;
	float SPEED_BLINK;
	float SPEED_JUMP_MAX;
	float SPEED_FALL;
	float SPEED_FALL_MAX;
	float SPEED_ROTATE;
	float TIME_BLINK_MAX;
	float TIME_BLINK_DEC;
	float Time_Blink;
	float Speed_Jump;
	FRotator CameraAngle;
	FRotator BodyAngle;
	FVector Velocity_Blink;
	FVector Hit_Normal;
	FVector Direction;
	FHitResult HitGround;

	void Calc_Velocity(float DeltaTime);
	void Calc_Velocity_XY(float DeltaTime);
	void Calc_Velocity_Z(float DeltaTime);
	float Calc_Dot(FVector a, FVector b);
	void ChangeBodyRotation(float DeltaTime);
};
