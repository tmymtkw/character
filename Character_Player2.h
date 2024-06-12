// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "Character_Player2.generated.h"

class USkeletalMeshComponent;
class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class UPlayerMovementComponent2;
class UInputMappingContext;
class UInputAction;
class AAPAtack;

UCLASS()
class MYPROJECT_API ACharacter_Player2 : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACharacter_Player2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
											AActor* OtherActor, UPrimitiveComponent* OtherComp,
											int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	float GetSP() const { return sp; }

	float GetAP() const { return ap; }

	float GetSPMax() const { return spMax; }

	float GetAPMax() const { return apMax; }

	float GetSPLIM() const { return SPLIM; }

	float GetAPLIM() const { return APLIM; }

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Input")
	const TObjectPtr<UInputAction> InputAction;

	// âÒïúÇ≈Ç´ÇÈç≈ëÂíl
	float spMax;

	float apMax;

	// åªç›ÇÃíl
	float sp;

	float ap;

private:
	// Components
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> compCapsule;

	UPROPERTY(VisibleAnyWhere)
	TObjectPtr<USkeletalMeshComponent> compMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> compCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerMovementComponent2> compMove;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> compArm;

	FRotator cameraInput;

	// Input
	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ControlAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* cameraAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* lockonAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* jumpAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* blinkAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* sprintAction;

	UPROPERTY(EditAnywhere, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* atackAction;

	// Power params
	const float SPLIM = 100.0f;

	const float APLIM = 100.0f;

	const float BLINKPOWER = 15.0f;

	const float MINBLINKPOWER = 10.0f;

	const float RECOVPOWER = 20.0f;

	// Target lock
	TObjectPtr<AActor> lockEnemy;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjects;

	bool isCameraLocked;

	// Functions
	void InitializePower();

	void MoveX(float val);

	void MoveY(float val);

	void Move(const FInputActionValue& val);

	void Sprint();

	void TryJump();

	void TryBlink();

	void Atack();

	void MoveCameraPitch(float val);

	void MoveCameraYaw(float val);

	void MoveCamera(const FInputActionValue& val);

	void TryLockON();

	void SetCameraLag(bool val);

	void UpdateCameraAngle();
};
