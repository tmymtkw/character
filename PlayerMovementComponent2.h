// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PlayerMovementComponent2.generated.h"

class USkeletalMeshComponent;

struct Status {
public:
	// Horizon move
	const float WALK = 500.0f;
	const float SPRINT = 200.0f;
	const float BLINK = 1500.0f;
	const float BLINKTIME = 1.0f;
	const float BLINKDEC = 5.0f;

	// Vertical move
	const float JUMP = 15.0f;
	const float FALL = 40.0f;
	const float FALLMAX = -15.0f;

	// Camera rotation
	const float ROTATE = 500.0f;
};

UCLASS()
class MYPROJECT_API UPlayerMovementComponent2 : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	UPlayerMovementComponent2();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

public:
	void SetInputX(float val);
	void SetInputY(float val);
	void SetInput(FVector2D val);
	void SetSprint();
	void SetJump();
	bool SetBlink();
	void SetBody(TObjectPtr<USkeletalMeshComponent> ptr);
	void SetCameraAngle(FRotator rotator);
	bool IsBlinkTimeBigger(float val);

private:
	void SetIsGrounded(FHitResult& hitResult);
	void SetInputDir(float DeltaTime);
	void SetVelocity(float DeltaTime);
	void UpdateComponentRotation(float DeltaTime);

	Status status;
	TObjectPtr<USkeletalMeshComponent> RotatedComponent;
	FVector input;
	FVector dir;
	FVector blinkDir;
	FVector hitNormal;
	FRotator cameraAngle;
	bool isSprint;
	bool isGrounded;
	float jumpSpeed;
	float blinkTime;
};
