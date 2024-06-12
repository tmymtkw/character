// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovementComponent2.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerMovementComponent2::UPlayerMovementComponent2() {
	//PrimaryComponentTick.bCanEverTick = true;

	input = FVector::Zero();
	cameraAngle = FRotator(0.0f, 0.0f, 0.0f);
	dir = FVector::Zero();
	isSprint = false;
	isGrounded = false;
	jumpSpeed = 0.0f;
	blinkTime = 0.0f;
	//UKismetSystemLibrary::PrintString(this, TEXT("Created"), true, false, FColor::Blue, 5.0f, TEXT("Red"));
}

void UPlayerMovementComponent2::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent || !RotatedComponent || ShouldSkipUpdate(DeltaTime)) {
		UKismetSystemLibrary::PrintString(this, TEXT("Failed"), true, false, FColor::Red, DeltaTime, TEXT("None"));
		return;
	}

	// Update Movement
	FHitResult Hit;
	// Check ground
	SetIsGrounded(Hit);
	
	// Get rotated input
	SetInputDir(DeltaTime);

	// Rotate body
	UpdateComponentRotation(DeltaTime);

	// Set velocity
	SetVelocity(DeltaTime);

	if (!Velocity.IsNearlyZero()) {
		SafeMoveUpdatedComponent(Velocity, UpdatedComponent->GetComponentRotation(), true, Hit);

		// If we bumped into something, try to slide along it
		if (Hit.IsValidBlockingHit()) {
			SlideAlongSurface(Velocity, 1.f - Hit.Time, Hit.Normal, Hit);
		}
	}

	UpdateComponentVelocity();

	// Debug
	UKismetSystemLibrary::PrintString(this, isGrounded ? TEXT("Ground") : TEXT("Air"), true, false, FColor::White, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, dir.ToString(), true, false, FColor::White, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, TEXT("Normal: ") + hitNormal.ToString(), true, false, FColor::White, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, TEXT("Blink dir: ") + blinkDir.ToString() + TEXT(" Blink Time: ") + FString::SanitizeFloat(blinkTime), true, false, FColor::White, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, TEXT("input: ") + input.ToString(), true, false, FColor::White, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, cameraAngle.ToString(), true, false, FColor::White, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, RotatedComponent->GetComponentRotation().ToString(), true, false, FColor::White, DeltaTime, TEXT("None"));
}

void UPlayerMovementComponent2::SetInputX(float val) {
	input.X = val;
}

void UPlayerMovementComponent2::SetInputY(float val) {
	input.Y = val;
}

void UPlayerMovementComponent2::SetInput(FVector2D val) {
	input = FVector(val.Y, val.X, 0.0f);
}

void UPlayerMovementComponent2::SetBody(TObjectPtr<USkeletalMeshComponent> ptr) {
	RotatedComponent = ptr;
}

void UPlayerMovementComponent2::SetCameraAngle(FRotator rotator) {
	cameraAngle = rotator;
}

bool UPlayerMovementComponent2::IsBlinkTimeBigger(float val) {
	return val < blinkTime;
}

void UPlayerMovementComponent2::SetIsGrounded(FHitResult& hitResult) {
	isGrounded = UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
																											 UpdatedComponent->GetComponentLocation() - FVector::UpVector * 65.0f,
																											 UpdatedComponent->GetComponentLocation() - FVector::UpVector * 70.0f,
																											 25.0f,
																											 UEngineTypes::ConvertToTraceType(ECC_Visibility),
																											 false,
																											 TArray<AActor*>{ PawnOwner },
																											 EDrawDebugTrace::ForOneFrame,
																											 hitResult,
																											 true,
																											 FColor::Green,
																											 FColor::Red);
	if (!isGrounded) return;
	hitNormal = hitResult.Normal;
}

void UPlayerMovementComponent2::SetInputDir(float DeltaTime) {
	if (input.IsZero()) return;
	dir = input.RotateAngleAxis(cameraAngle.Yaw, FVector::UpVector);
	dir.Normalize(1.0);
}

void UPlayerMovementComponent2::SetVelocity(float DeltaTime) {
	FVector moveVector = FVector::Zero();
	bool isBlink = 0.0f < blinkTime;
	// ブリンク速度は変数 blinkDir から計算

	// 入力の反映
	if (!input.IsZero()) {
		moveVector = dir * (status.WALK + isSprint * status.SPRINT) * DeltaTime;
		//moveVector -= moveVector;
	}
	// 床の角度で調整
	moveVector -= hitNormal.GetSafeNormal() * (moveVector | hitNormal);

	// ジャンプ、落下の反映
	if (!isGrounded) jumpSpeed = FMathf::Max(jumpSpeed - status.FALL * DeltaTime, status.FALLMAX);
	else if (jumpSpeed < 0.0f) jumpSpeed = 0.0f;
	// ジャンプ速度の更新
	if (jumpSpeed < 0.0f) moveVector *= (1 - jumpSpeed / status.FALLMAX * 0.8f);
	moveVector.Z += jumpSpeed;

	// 最終速度の計算
	Velocity = moveVector * (1.0f - 0.8f * isBlink) + blinkDir * status.BLINK * DeltaTime * isBlink;

	// ブリンク時間の更新
	if (blinkTime == 0.0f) return;
	blinkTime = FMathf::Max(blinkTime - status.BLINKDEC * DeltaTime, 0.0f);
}

void UPlayerMovementComponent2::UpdateComponentRotation(float DeltaTime) {
	//if (dir.IsNearlyZero()) return;
	
	FRotator bodyRot = RotatedComponent->GetRightVector().Rotation(); // !!! メッシュが90度回転している
	FRotator rotateDif = dir.Rotation() - bodyRot;

	// -180 ~ 180 になるように調整
	if (rotateDif.Yaw < -180.0f) rotateDif.Yaw += 360.0f;
	else if (180.0f < rotateDif.Yaw) rotateDif.Yaw -= 360.0f;
	float deltaDegree = (FMathf::Abs(rotateDif.Yaw) * 4.0f + status.ROTATE) * DeltaTime;

	// Debug
	//UKismetSystemLibrary::PrintString(this, dir.ToString(), true, false, FColor::Red, DeltaTime, TEXT("None"));
	//UKismetSystemLibrary::PrintString(this, TEXT("bodyDir ") + bodyRot.ToString(), true, false, FColor::White, DeltaTime, TEXT("None"));
	//UKismetSystemLibrary::PrintString(this, TEXT("Dif ") + rotateDif.ToString(), true, false, FColor::White, DeltaTime, TEXT("None"));

	// コンポーネントを回転
	if (rotateDif.Yaw == 0.0f) {
		return;
	}
	else if (FMathf::Abs(rotateDif.Yaw) < 10.0f) {
		RotatedComponent->SetWorldRotation(dir.Rotation() + FRotator(0.0f, -90.0f, 0.0f));
	}
	else {
		if (rotateDif.Yaw < 0.0f) deltaDegree *= -1;
		RotatedComponent->AddRelativeRotation(FRotator(0.0f, deltaDegree, 0.0f));
	}
}

void UPlayerMovementComponent2::SetJump() {
	if (jumpSpeed != 0.0f) return;

	jumpSpeed = status.JUMP;
}

bool UPlayerMovementComponent2::SetBlink() {
	if (0.0f < blinkTime) return false;

	if (input.IsZero()) {
		blinkDir = -1.0f * RotatedComponent->GetRightVector();
	}
	else {
		blinkDir = input.GetSafeNormal();
		blinkDir = blinkDir.RotateAngleAxis(cameraAngle.Yaw, FVector::UpVector);
	}
	blinkTime = status.BLINKTIME;

	return true;
}

void UPlayerMovementComponent2::SetSprint() {
	isSprint = !isSprint;
}