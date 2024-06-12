// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_Player.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PlayerMovementComponent.h"
#include "AnimInstance_Player.h"
#include "APAtack.h"

#define LOCK_COLLISION ECC_GameTraceChannel1

// Sets default values
ACharacter_Player::ACharacter_Player()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Comp_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	Comp_SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Comp_Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Comp_Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Comp_Move = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("Movement"));
	
	RootComponent = Comp_Capsule;

	TObjectPtr<USkeletalMesh> Mesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin"));
	TSubclassOf<UAnimInstance_Player> AnimInstance = TSoftClassPtr<UAnimInstance_Player>(FSoftObjectPath(TEXT("AnimBluePrint'/Game/BP/ABP_Player.ABP_Player_C'"))).LoadSynchronous();
	Comp_SkeletalMesh->SetupAttachment(RootComponent);
	Comp_SkeletalMesh->SetSkeletalMesh(Mesh);
	Comp_SkeletalMesh->SetAnimClass((UClass*)AnimInstance);
	Comp_SkeletalMesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	Comp_Capsule->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	Comp_Capsule->SetCapsuleSize(30.0f, 90.0f);
	Comp_Capsule->SetHiddenInGame(false);
	Comp_Capsule->SetSimulatePhysics(false);
	Comp_Capsule->SetCollisionProfileName(TEXT("CharacterCollision"));

	Comp_SpringArm->SetupAttachment(RootComponent);
	Comp_SpringArm->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));
	Comp_SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
	//Comp_SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	Comp_SpringArm->TargetArmLength = 300.f;
	Comp_SpringArm->SetHiddenInGame(true);
	Comp_SpringArm->bInheritPitch = false;
	Comp_SpringArm->bInheritYaw = false;
	Comp_SpringArm->bInheritRoll = false;

	Comp_Camera->SetupAttachment(Comp_SpringArm);
	Comp_Camera->PostProcessSettings.MotionBlurAmount = 0.0f;

	Comp_Move->UpdatedComponent = RootComponent;

	Comp_Capsule->OnComponentBeginOverlap.AddDynamic(this, &ACharacter_Player::OnOverlapBegin);

	HP_LIM = 100.0f;
	AP_LIM = 100.0f;
	HP_BLINK = 15.0f;
	HP_RECOVER = 20.0f;

	LockedEnemy = nullptr;
}

// Called when the game starts or when spawned
void ACharacter_Player::BeginPlay()
{
	Super::BeginPlay();
	
	velocity = FVector::Zero();
	rotation = FVector2D::Zero();

	HP_Max = HP_LIM;
	HP_Curr = HP_Max;

	AP_Max = AP_LIM * 0.5f;
	AP_Curr = 0.0f;

	isLocked = false;
	traceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}

// Called every frame
void ACharacter_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString msg = TEXT("no enemy");
	if (LockedEnemy) {
		msg = LockedEnemy->GetActorLocation().ToString();
	}
	UKismetSystemLibrary::PrintString(this, msg, true, false, FColor::Yellow, DeltaTime, TEXT("None"));

	ChangeCameraPos(DeltaTime);
	FVector bodyLoc = Comp_SpringArm->GetComponentLocation();
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(),
																			 bodyLoc,
																			 bodyLoc + Comp_Capsule->GetForwardVector() * 50.0f,
																			 50.0f,
																			 FColor::Yellow,
																			 DeltaTime * 1.1f, 1.0f);
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(),
																			 bodyLoc - Comp_SpringArm->GetForwardVector() * 50.0f,
																			 bodyLoc + Comp_SpringArm->GetForwardVector() * 50.0f,
																			 50.0f,
																			 FColor::Cyan,
																			 DeltaTime * 1.1f, 1.0f);
	UKismetSystemLibrary::DrawDebugBox(GetWorld(),
																		 Comp_SpringArm->GetComponentLocation(),
																		 FVector::OneVector,
																		 FColor::Magenta,
																		 FRotator::ZeroRotator,
																		 DeltaTime * 1.1f, 1.0f);

	if (isHealing) Heal(DeltaTime);
	if (!Comp_Move->CheckBlinkTime(0.0f)) HP_Curr = FMath::Clamp(HP_Curr + HP_RECOVER * DeltaTime, 0, HP_Max);
	//AddActorLocalOffset(GetVelocity() * (Speed_Walk + isSprint*Speed_Sprint - !isGrounded*500.0f) * DeltaTime, true);
	//RootComponent->AddLocalOffset(GetVelocity() * (Speed_Walk + isSprint * Speed_Sprint - !isGrounded * 500.0f) * DeltaTime, false);
	//Comp_Move->AddInputVector(GetVelocity() * (Speed_Walk + isSprint * Speed_Sprint - !isGrounded * 500.0f) * DeltaTime);
}

// Called to bind functionality to input
void ACharacter_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACharacter_Player::Sprint);
	PlayerInputComponent->BindAction("Blink", IE_Pressed, this, &ACharacter_Player::Blink);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter_Player::Jump);
	PlayerInputComponent->BindAction("Heal", IE_Pressed, this, &ACharacter_Player::HealBegin);
	PlayerInputComponent->BindAction("Heal", IE_Released, this, &ACharacter_Player::HealEnd);
	PlayerInputComponent->BindAction("Attack_Normal", IE_Pressed, this, &ACharacter_Player::Atack_Normal);
	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &ACharacter_Player::ChangeLockMode);
	PlayerInputComponent->BindAxis("V_x", this, &ACharacter_Player::Set_x);
	PlayerInputComponent->BindAxis("V_y", this, &ACharacter_Player::Set_y);
	PlayerInputComponent->BindAxis("Camera_Horizon", this, &ACharacter_Player::Look_x);
	PlayerInputComponent->BindAxis("Camera_Vertical", this, &ACharacter_Player::Look_y);
}

void ACharacter_Player::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, 
																			 AActor* OtherActor, 
																			 UPrimitiveComponent* OtherComp, 
																			 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!OtherComp->ComponentHasTag(FName("Attack")) || OtherComp->ComponentHasTag(FName("Player"))) return;
	if (Comp_Move->CheckBlinkTime(0.0f)) {
		AP_Curr += 10.0f;
		return;
	}
	else {
		HP_Max -= 30.0f;
	}
	UKismetSystemLibrary::PrintString(this, "Attack Hit", true, true, FColor::Red, 3.f, TEXT("None"));
}

void ACharacter_Player::Set_x(float val) { Comp_Move->MoveInput.X = val; }
void ACharacter_Player::Set_y(float val) { Comp_Move->MoveInput.Y = val; }

void ACharacter_Player::Look_x(float val) { 
	if (isLocked) return;
	rotation.X = val; 
	Comp_Move->SetAngleYaw(val);
}
void ACharacter_Player::Look_y(float val) { 
	if (isLocked) return;
	rotation.Y = val; 
	Comp_Move->SetAnglePitch(val);
}

void ACharacter_Player::Blink() {
	if (HP_Curr + AP_Curr < 15.0f || Comp_Move->CheckBlinkTime(0.3f)) return;
	if (HP_Curr < HP_BLINK) {
		AP_Curr = FMath::Max(0.0f, AP_Curr + HP_Curr - HP_BLINK);
		HP_Curr = 0.0f;
	}
	else {
		HP_Curr = FMath::Max(0.0f, HP_Curr - HP_BLINK);
	}
	Comp_Move->SetBlink();
}

void ACharacter_Player::Sprint() { Comp_Move->SetSprint(); }

void ACharacter_Player::Jump() { Comp_Move->SetJump(); }

void ACharacter_Player::HealBegin() { 
	if (AP_Curr <= 0.0f || HP_Max == HP_LIM) return;
	isHealing = true; 
}

void ACharacter_Player::HealEnd() {
	isHealing = false;
}

void ACharacter_Player::Heal(float Delta) {
	if (AP_Curr <= 0.0f || HP_LIM <= HP_Max) {
		HP_Max = FMathf::Clamp(HP_Max, 0.0f, HP_LIM);
		isHealing = false;
		return;
	}

	float HealValue = HP_RECOVER * Delta;
	if (AP_Curr < HealValue) {
		HP_Max += AP_Curr; AP_Curr = 0;
		return;
	}
	AP_Curr -= HealValue; 
	HP_Max += HealValue * 0.7;
}

void ACharacter_Player::Atack_Normal() {
	//if (!AtackActorClass) return;

	FVector Location = Comp_Capsule->GetComponentLocation() + 
		Comp_Capsule->GetForwardVector() * 125.0f + Comp_Capsule->GetUpVector() * 60.0f;
	FRotator Rotation = Comp_Capsule->GetComponentRotation();

	TObjectPtr<UWorld> World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	AAPAtack* Atack = World->SpawnActor<AAPAtack>(Location, Rotation, SpawnParams);
}

void ACharacter_Player::ChangeCameraPos(float DeltaTime) {
	FRotator CameraAngle = Comp_SpringArm->GetComponentRotation();
	if (LockedEnemy) {
		FVector LookLocation = LockedEnemy->GetActorLocation();
		
	}
	else {
		// ã‰º‚ÌˆÚ“®”ÍˆÍ‚ð§ŒÀ‚·‚é‚Ì‚Åaddrelativerotation‚ÍŽg‚¦‚È‚¢
		CameraAngle.Yaw += rotation.X;
		CameraAngle.Pitch = FMath::Clamp(CameraAngle.Pitch + rotation.Y, -85.0f, 60.0f);
		Comp_SpringArm->SetWorldRotation(CameraAngle);
	}
	UKismetSystemLibrary::PrintString(this, TEXT("Camera Rotation in Actor: ") + Comp_SpringArm->GetComponentRotation().ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, TEXT("Root Rotation in Actor: ") + RootComponent->GetComponentRotation().ToString(), true, false, FColor::Blue, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, (isLocked) ? TEXT("true") : TEXT("false"), true, false, FColor::Blue, DeltaTime, TEXT("None"));
}

void ACharacter_Player::ChangeLockMode() {
	if (isLocked) {
		LockedEnemy = nullptr;
		isLocked = !isLocked;
		Comp_Move->SetLock(false);
		return;
	}

	isLocked = FindEnemy();
}

bool ACharacter_Player::FindEnemy() {
	TArray<FHitResult> HitResults;
	FVector stt = Comp_Capsule->GetComponentLocation() + FVector::UpVector * 80.0f;

	bool isWatching = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
																																	stt,
																																	stt + Comp_Camera->GetForwardVector() * 1000.0f,
																																	FVector(0.0f, 300.0f, 100.0f),
																																	Comp_Camera->GetComponentRotation(),
																																	traceObjects,
																																	false,
																																	TArray<AActor*>{ this },
																																	EDrawDebugTrace::ForOneFrame,
																																	HitResults,
																																	true,
																																	FColor::Green,
																																	FColor::Red);

	Comp_Move->SetLock(isWatching);

	if (!HitResults.IsEmpty()) {
		for (auto& result : HitResults) {
			UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(result.Distance), true, false, FColor::Red, 2.0f, TEXT("None"));
		}
		LockedEnemy = HitResults[0].GetActor();
	}
	else {
		FRotator bodyRot = RootComponent->GetComponentRotation();
		FRotator cameraRot = Comp_Camera->GetComponentRotation();
		bodyRot.Yaw -= cameraRot.Yaw;
		Comp_Move->SetAngleYaw(bodyRot.Yaw);
		Comp_SpringArm->AddRelativeRotation(bodyRot);
	}

	return isWatching;
}

//FVector ACharacter_Player::GetVelocity() {
//	FVector velocity_norm = velocity.GetSafeNormal();
//	FVector direction = Comp_SpringArm->GetForwardVector() * velocity_norm.X + Comp_SpringArm->GetRightVector() * velocity_norm.Y;
//	direction.Z = 0.0f;
//	return direction;
//}

void ACharacter_Player::MoveForward(float AxisValue) {
	if (Comp_Move && (Comp_Move->UpdatedComponent == RootComponent)) {
		Comp_Move->AddInputVector(Comp_SpringArm->GetForwardVector() * AxisValue);
	}
}

void ACharacter_Player::MoveRight(float AxisValue) {
	if (Comp_Move && (Comp_Move->UpdatedComponent == RootComponent)) {
		Comp_Move->AddInputVector(Comp_SpringArm->GetRightVector() * AxisValue);
	}
}

UPawnMovementComponent* ACharacter_Player::GetMovementComponent() const { return Comp_Move; }

FRotator ACharacter_Player::GetCameraAngle() {
	return Comp_SpringArm->GetRelativeRotation();
}

float ACharacter_Player::GetCameraYaw() {
	if (!Comp_SpringArm) return 0.0f;

	return Comp_SpringArm->GetComponentRotation().Yaw;
}