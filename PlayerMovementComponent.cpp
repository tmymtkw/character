// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerMovementComponent::UPlayerMovementComponent() {
  SPEED_WALK = 500.0f;
  SPEED_SPRINT = 200.0f;
  SPEED_BLINK = 1000.0f;
  SPEED_JUMP_MAX = 1000.0f;
  SPEED_FALL = 30.0f;
  SPEED_FALL_MAX = -1500.0f;
  SPEED_ROTATE = 1000.0f;
  TIME_BLINK_MAX = 1.0f;
  TIME_BLINK_DEC = 3.0f;

  isSprint = false;
  isJump = false;
  isBlink = false;
  isGrounded = true;
  isLocked = false;

  MoveInput = FVector::Zero();
  Velocity_Blink = FVector::Zero();
  Direction = FVector::Zero();
  CameraAngle = FRotator(-10.0f, 0.0f, 0.0f);
  BodyAngle = FRotator::ZeroRotator;
  Speed_Jump = 0.0f;
  Time_Blink = 0.0f;
}

void UPlayerMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
  Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

  // Make sure that everything is still valid, and that we are allowed to move.
  if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) {
    return;
  }

  // Check isGround Flag
  CheckGround(HitGround);
  UKismetSystemLibrary::PrintString(this, isGrounded ? TEXT("Onground") : TEXT("Floating"), true, false, FColor::Blue, DeltaTime, TEXT("None"));
  UKismetSystemLibrary::PrintString(this, Hit_Normal.ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));

  // Calculate Velocity
  Calc_Velocity(DeltaTime);
  //Calc_Velocity_XY(DeltaTime);
  //Calc_Velocity_Z(DeltaTime);

  // Update Rotation
  ChangeBodyRotation(DeltaTime);

  // Update Movement
  if (!Velocity.IsNearlyZero()) {
    FHitResult Hit;
    // if (HitObjectInfo.Normal != FVector::UpVector) Velocity.Z -= 5.0f;
    SafeMoveUpdatedComponent(Velocity, UpdatedComponent->GetComponentRotation(), true, Hit);
    
    // If we bumped into something, try to slide along it
    if (Hit.IsValidBlockingHit()) {
      SlideAlongSurface(Velocity, 1.f - Hit.Time, Hit.Normal, Hit);
    }
  }

  UpdateComponentVelocity();

  if (!CheckBlinkTime(0.0f) && isBlink) isBlink = false;

  // Debug
  UKismetSystemLibrary::PrintString(this, TEXT("Camera Rotation in Comp_Move: ") + CameraAngle.ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));
  //UKismetSystemLibrary::PrintString(this, TEXT("Input Direction: ") + Direction.ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));
  //UKismetSystemLibrary::PrintString(this, TEXT("Velocity Direction: ") + Direction.ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));
  //UKismetSystemLibrary::PrintString(this, TEXT("Component Direction: ") + UpdatedComponent->GetComponentRotation().ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));
  UKismetSystemLibrary::PrintString(this, TEXT("Rotator: ") + BodyAngle.ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));
  //UKismetSystemLibrary::PrintString(this, TEXT("Velocity: ") + Velocity.ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));
  //UKismetSystemLibrary::PrintString(this, TEXT("JumpSpeed: ") + FString::SanitizeFloat(Speed_Jump), true, false, FColor::Blue, DeltaTime, TEXT("None"));
}

void UPlayerMovementComponent::Calc_Velocity_XY(float DeltaTime) {
  // 単位ベクトル
  FVector dir = MoveInput.GetSafeNormal().RotateAngleAxis(CameraAngle.Yaw, FVector::UpVector);

  // x_y平面の速度を乗算
  if (0.0f < Time_Blink) {
    Velocity.X = (Velocity_Blink.X * Time_Blink + dir.X * (TIME_BLINK_MAX - Time_Blink)) * SPEED_BLINK * DeltaTime;
    Velocity.Y = (Velocity_Blink.Y * Time_Blink + dir.Y * (TIME_BLINK_MAX - Time_Blink)) * SPEED_BLINK * DeltaTime;
    Time_Blink -= TIME_BLINK_DEC * DeltaTime;
  }
  else {
    Velocity.X = dir.X * DeltaTime * (SPEED_WALK + isSprint * SPEED_SPRINT);
    Velocity.Y = dir.Y * DeltaTime * (SPEED_WALK + isSprint * SPEED_SPRINT);
  }

  //Velocity -= HitObjectInfo.Normal.GetSafeNormal() * Calc_Dot(dir, Hit_Normal);
}

void UPlayerMovementComponent::Calc_Velocity_Z(float DeltaTime) {
  if (0.0f < Time_Blink) {
    Speed_Jump = -100.0f;
  }
  else {
    Velocity.X *= 0.8f;
    Velocity.Y *= 0.8f;
    Speed_Jump = FMath::Max(Speed_Jump - SPEED_FALL, SPEED_FALL_MAX);
  }

  Velocity.Z = Speed_Jump * DeltaTime;
}

void UPlayerMovementComponent::Calc_Velocity(float DeltaTime) {
  /*
  Input: FVector MoveInput
  normal: FVector HitObjectResult.normal
  */

  FVector dir = MoveInput.GetSafeNormal().RotateAngleAxis(CameraAngle.Yaw, FVector::UpVector);
  if (!dir.IsNearlyZero()) Direction = dir;

  // ブリンク中の処理
  if (0.0f < Time_Blink) {
    dir = Velocity_Blink + dir * 0.2f;
    Time_Blink -= TIME_BLINK_DEC * DeltaTime;
  }

  if (isGrounded) {
    float Dot = Calc_Dot(dir, Hit_Normal);

    // 地面が傾いているときの処理
    if (Dot < float(FMath::Sqrt(3.0) / 2.0)) dir -= Dot * Hit_Normal;
    // 落下速度の調整
    if (Speed_Jump < 0.0f) {
      Speed_Jump = 0.0f;
      isJump = false;
    }

    // 速度の更新
    Velocity = dir * GetSpeed() * DeltaTime;
    if (isJump) Velocity.Z = Speed_Jump * DeltaTime;
  }
  else {
    Velocity = dir * GetSpeed() * DeltaTime;
    Calc_Velocity_Z(DeltaTime);
  }
  //UKismetSystemLibrary::PrintString(this, TEXT("Dot: ") + FString::SanitizeFloat(Dot), true, false, FColor::Red, DeltaTime, TEXT("None"));
}

void UPlayerMovementComponent::ChangeBodyRotation(float DeltaTime) {
  if (isLocked) BodyAngle = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, UpdatedComponent->GetForwardVector());
  else BodyAngle = UKismetMathLibrary::FindLookAtRotation(FVector::ZeroVector, FVector(Direction.X, Direction.Y, 0.0f));
  float Rotator_diff = BodyAngle.Yaw - UpdatedComponent->GetComponentRotation().Yaw;

  if (Rotator_diff < -180.0f) Rotator_diff += 360.0f;
  else if (180.0f < Rotator_diff) Rotator_diff -= 360.0f;

  if (10.0f < Rotator_diff) {
    UpdatedComponent->AddWorldRotation(FRotator(0.0f, SPEED_ROTATE * (1.0f - !isGrounded * !isBlink * 0.95f) * DeltaTime, 0.0f));
  }
  else if (Rotator_diff < -10.0f) {
    UpdatedComponent->AddWorldRotation(FRotator(0.0f, -1 * SPEED_ROTATE * (1.0f - !isGrounded * !isBlink * 0.95f) * DeltaTime, 0.0f));
  }
  else {
    UpdatedComponent->SetWorldRotation(BodyAngle);
  }
}

void UPlayerMovementComponent::SetAnglePitch(float dp) {
  CameraAngle.Pitch = FMath::Clamp(CameraAngle.Pitch + dp, -85.0f, 60.0f);
}

void UPlayerMovementComponent::SetAngleYaw(float dy) {
  CameraAngle.Yaw += dy;
  if (180.0f < CameraAngle.Yaw) CameraAngle.Yaw -= 360.0f;
  if (CameraAngle.Yaw < -180.0f) CameraAngle.Yaw += 360.0f;
}

FRotator UPlayerMovementComponent::GetCameraAngle() {
  return CameraAngle;
}

FRotator UPlayerMovementComponent::GetBodyAngle() {
  return BodyAngle;
}

void UPlayerMovementComponent::SetLock(bool flag) {
  isLocked = flag;
}

void UPlayerMovementComponent::SetJump() {
  if (!isGrounded) return;
  Speed_Jump = SPEED_JUMP_MAX;
  isJump = true;
}

void UPlayerMovementComponent::SetBlink() {
  isBlink = true;
  Velocity_Blink = (MoveInput.IsZero()) ? FVector::BackwardVector.RotateAngleAxis(CameraAngle.Yaw, FVector::UpVector) : MoveInput.GetSafeNormal().RotateAngleAxis(CameraAngle.Yaw, FVector::UpVector);
  Time_Blink = TIME_BLINK_MAX;
}

void UPlayerMovementComponent::SetSprint() {
  isSprint = !isSprint;
}

void UPlayerMovementComponent::CheckGround(FHitResult Hit) {
  //GetWorld()->LineTraceSingleByChannel(HitObjectInfo, UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentLocation() - FVector::UpVector * 95.0f, ECC_Visibility);
  isGrounded = UKismetSystemLibrary::SphereTraceSingle(GetWorld(),
                                                        UpdatedComponent->GetComponentLocation() - FVector::UpVector * 65.0f,
                                                        UpdatedComponent->GetComponentLocation() - FVector::UpVector * 68.0f,
                                                        25.0f,
                                                        UEngineTypes::ConvertToTraceType(ECC_Visibility),
                                                        false,
                                                        TArray<AActor*>{ PawnOwner },
                                                        EDrawDebugTrace::ForOneFrame,
                                                        HitGround,
                                                        true,
                                                        FColor::Green,
                                                        FColor::Red);

  if (isGrounded) Hit_Normal = HitGround.Normal;
}

bool UPlayerMovementComponent::CheckBlinkTime(float Time) {
  // Time_Blinkが引数よりおおきいならtrue
  return Time < Time_Blink;
}

float UPlayerMovementComponent::Calc_Dot(FVector a, FVector b) {
  return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
}

float UPlayerMovementComponent::GetSpeed() {
  return (0.0f < Time_Blink) ? SPEED_BLINK : SPEED_WALK + isSprint * SPEED_SPRINT;
}