// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_Player2.h"

// components
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerMovementComponent2.h"
#include "Kismet/KismetSystemLibrary.h"

// Enhanced input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"

// Atack
#include "APAtack.h"

// Sets default values
ACharacter_Player2::ACharacter_Player2()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	compCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	compCapsule->SetCapsuleSize(30.0f, 90.0f);
	compCapsule->SetHiddenInGame(false);
	compCapsule->SetSimulatePhysics(false);
	compCapsule->SetCollisionProfileName(TEXT("CharacterCollision"));
	compCapsule->OnComponentBeginOverlap.AddDynamic(this, &ACharacter_Player2::OnOverlapBegin);
	RootComponent = compCapsule;

	compMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	TObjectPtr<USkeletalMesh> mesh = LoadObject<USkeletalMesh>(nullptr, 
																														 TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin"));
	compMesh->SetSkeletalMesh(mesh);
	compMesh->SetupAttachment(RootComponent);
	compMesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	compArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	compArm->SetupAttachment(RootComponent);
	compArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 80.0f), FRotator(-10.0f, 0.0f, 0.0f));
	compArm->TargetArmLength = 300.f;
	compArm->bInheritPitch = false;
	compArm->bInheritYaw = false;
	compArm->bInheritRoll = false;
	compArm->CameraLagSpeed = 5.0f;
	compArm->CameraLagMaxDistance = 25.0f;
	compArm->CameraRotationLagSpeed = 5.0f;
	compArm->SetHiddenInGame(true);

	compCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	compCamera->SetupAttachment(compArm);
	compCamera->PostProcessSettings.MotionBlurAmount = 0.0f;

	compMove = CreateDefaultSubobject<UPlayerMovementComponent2>(TEXT("Move"));
	compMove->SetUpdatedComponent(RootComponent);
	compMove->SetBody(compMesh);
	compMove->SetCameraAngle(compArm->GetComponentRotation());

	DefaultMappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/Input/IM_Controls.IM_Controls"));
	ControlAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Action/IA_Control"));
	cameraAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Action/IA_Look"));
	jumpAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Action/IA_Jump"));
	blinkAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Action/IA_Blink"));
	sprintAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Action/IA_Sprint"));
	atackAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Action/IA_Atack"));
	lockonAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/Input/Action/IA_LockON"));

	isCameraLocked = false;
	lockEnemy = nullptr;
}

// Called when the game starts or when spawned
void ACharacter_Player2::BeginPlay()
{
	Super::BeginPlay();
	
	cameraInput = FRotator(0.0f, 0.0f, 0.0f);

	InitializePower();

	if (const APlayerController* playerController = Cast<APlayerController>(Controller)) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void ACharacter_Player2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCameraAngle();

	// Update Power
	sp = FMathf::Min(sp + RECOVPOWER * DeltaTime, spMax);

	// Debug
	//UKismetSystemLibrary::PrintString(this, compArm->GetComponentRotation().ToString(), true, false, FColor::Red, DeltaTime, TEXT("None"));
	FVector bodyLoc = compArm->GetComponentLocation();
	FVector armDir = compArm->GetForwardVector();
	FVector bodyDir = compMesh->GetRightVector();
	armDir.Z = 0.0f;
	armDir.Normalize();
	bodyDir.Normalize();
	//UKismetSystemLibrary::PrintString(this, TEXT("SP: ") + FString::SanitizeFloat(sp) + TEXT(" / ") + FString::SanitizeFloat(spMax), true, false, FColor::Red, DeltaTime, TEXT("None"));
	//UKismetSystemLibrary::PrintString(this, TEXT("AP: ") + FString::SanitizeFloat(ap) + TEXT(" / ") + FString::SanitizeFloat(apMax), true, false, FColor::Red, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, isCameraLocked ? TEXT("ON") : TEXT("OFF"), true, false, FColor::Red, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::PrintString(this, lockEnemy ? lockEnemy->GetActorLocation().ToString() : TEXT("No enemy locked"), true, false, FColor::Red, DeltaTime, TEXT("None"));
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(),
																			 bodyLoc - armDir * 30.0f,
																			 bodyLoc + armDir * 30.0f,
																			 50.0f,
																			 FColor::Yellow,
																			 DeltaTime * 1.1f, 1.0f);
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(),
																			 bodyLoc - bodyDir * 25.0f,
																			 bodyLoc + bodyDir * 25.0f,
																			 50.0f,
																			 FColor::Blue,
																			 DeltaTime * 1.1f, 1.0f);
}

// Called to bind functionality to input
void ACharacter_Player2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (TObjectPtr<UEnhancedInputComponent> enhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		enhancedInput->BindAction(ControlAction, ETriggerEvent::Triggered, this, &ACharacter_Player2::Move);
		enhancedInput->BindAction(ControlAction, ETriggerEvent::Completed, this, &ACharacter_Player2::Move);
		enhancedInput->BindAction(cameraAction, ETriggerEvent::Triggered, this, &ACharacter_Player2::MoveCamera);
		enhancedInput->BindAction(cameraAction, ETriggerEvent::Completed, this, &ACharacter_Player2::MoveCamera);
		enhancedInput->BindAction(jumpAction, ETriggerEvent::Started, this, &ACharacter_Player2::TryJump);
		enhancedInput->BindAction(blinkAction, ETriggerEvent::Started, this, &ACharacter_Player2::TryBlink);
		enhancedInput->BindAction(sprintAction, ETriggerEvent::Started, this, &ACharacter_Player2::Sprint);
		enhancedInput->BindAction(atackAction, ETriggerEvent::Started, this, &ACharacter_Player2::Atack);
		enhancedInput->BindAction(lockonAction, ETriggerEvent::Started, this, &ACharacter_Player2::TryLockON);
	}

	traceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}

void ACharacter_Player2::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
																				AActor* OtherActor, UPrimitiveComponent* OtherComp,
																				int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!OtherComp->ComponentHasTag(FName("Attack")) || OtherComp->ComponentHasTag(FName("Player"))) return;
	if (compMove->IsBlinkTimeBigger(0.0f)) {
		ap = FMathf::Min(ap + 10.0f, apMax); // TODO: rewrite
	}
	else {
		spMax = FMathf::Max(spMax - 30.0f, 0.0f); // TODO: rewrite
	}
}


void ACharacter_Player2::InitializePower() {
	spMax = SPLIM; sp = SPLIM;
	apMax = APLIM; ap = 0.0f;
}

void ACharacter_Player2::MoveX(float val) {
	compMove->SetInputX(val);
}

void ACharacter_Player2::MoveY(float val) {
	compMove->SetInputY(val);
}

void ACharacter_Player2::Move(const FInputActionValue& val) {
	const FVector2D inputVal = val.Get<FVector2D>();

	compMove->SetInput(inputVal);
}

void ACharacter_Player2::Sprint() {
	compMove->SetSprint();
}

void ACharacter_Player2::TryJump() {
	UKismetSystemLibrary::PrintString(this, TEXT("Jump"), true, false, FColor::Emerald, 2.0f, TEXT("None"));
	compMove->SetJump();
}

void ACharacter_Player2::TryBlink() {
	if (sp + ap < MINBLINKPOWER) return;
	if (!compMove->SetBlink()) return;
	UKismetSystemLibrary::PrintString(this, TEXT("Blink"), true, false, FColor::Emerald, 2.0f, TEXT("None"));
	sp = FMathf::Max(0.0f, sp - BLINKPOWER);
}

void ACharacter_Player2::Atack() {
	//if (!AtackActorClass) return;

	FVector Location = compMesh->GetComponentLocation() + compMesh->GetRightVector() * 125.0f + compMesh->GetUpVector() * 60.0f;
	FRotator Rotation = compMesh->GetComponentRotation() + FRotator(0.0f, 90.0f, 0.0f);

	TObjectPtr<UWorld> World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;

	AAPAtack* Atack = World->SpawnActor<AAPAtack>(Location, Rotation, SpawnParams);
}

void ACharacter_Player2::MoveCameraPitch(float val) {
	cameraInput.Yaw = val;
}

void ACharacter_Player2::MoveCameraYaw(float val) {
	cameraInput.Pitch = val;
}

void ACharacter_Player2::MoveCamera(const FInputActionValue& val) {
	const FVector2D inputVal = val.Get<FVector2D>();

	cameraInput = FRotator(inputVal.Y, inputVal.X, 0.0f);
}

void ACharacter_Player2::TryLockON() {
	if (isCameraLocked) {
		isCameraLocked = false;
		lockEnemy = nullptr;
		SetCameraLag(false);
		return;
	}
	else {
		TArray<FHitResult> HitResults;
		FVector stt = compCapsule->GetComponentLocation() + FVector::UpVector * 80.0f;
		FVector end = compCamera->GetForwardVector() * 1000.0f;
		end.Z = 0.0f;
		FRotator rot = compCamera->GetComponentRotation();
		rot.Pitch = 0.0f;

		isCameraLocked = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(),
																																		stt,
																																		stt + end,
																																		FVector(0.0f, 300.0f, 100.0f),
																																		rot,
																																		traceObjects,
																																		false,
																																		TArray<AActor*>{ this },
																																		EDrawDebugTrace::ForDuration,
																																		HitResults,
																																		true,
																																		FColor::Green,
																																		FColor::Red);

		if (!isCameraLocked) return;

		SetCameraLag(true);
		lockEnemy = HitResults.Top().GetActor();
		if (!GEngine) return;
		for (auto& hit : HitResults) {
			AActor* enemy = hit.GetActor();
			UKismetSystemLibrary::DrawDebugString(GetWorld(), enemy->GetActorLocation(), FString::Printf(TEXT("Enemy")), nullptr, FLinearColor::Red, 3.0f);
		}
	}
}

void ACharacter_Player2::SetCameraLag(bool val) {
	compArm->bEnableCameraLag = val;
	compArm->bEnableCameraRotationLag = val;
}

void ACharacter_Player2::UpdateCameraAngle() {
	FRotator cameraAngle = compArm->GetComponentRotation();

	if (isCameraLocked) {
		cameraAngle = ((lockEnemy->GetActorLocation() - compCapsule->GetComponentLocation()) * 0.5f - FVector(0.0f, 0.0f, 100.0f)).Rotation();
	}
	else {
		if (cameraInput.IsZero()) return;

		cameraAngle += cameraInput;
		cameraAngle.Pitch = FMath::Clamp(cameraAngle.Pitch, -85.0f, 60.0f);
	}

	compArm->SetWorldRotation(cameraAngle);
	compMove->SetCameraAngle(compArm->GetComponentRotation());
}