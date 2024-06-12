// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Character_Player.generated.h"

class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;
class USceneComponent;
class UPlayerMovementComponent;
class UPrimitiveComponent;
class AAPAtack;

UCLASS()
class MYPROJECT_API ACharacter_Player : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACharacter_Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	FRotator GetCameraAngle();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Comp_SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USpringArmComponent> Comp_SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Comp_Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> Comp_Capsule;

	UPROPERTY()
	TObjectPtr<UPlayerMovementComponent> Comp_Move;

	UPROPERTY()
	TSubclassOf<AAPAtack> AtackActorClass;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, 
											AActor* OtherActor, UPrimitiveComponent* OtherComp, 
											int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	float GetHP_Curr() const { return HP_Curr; }
	float GetAP_Curr() const { return AP_Curr; }
	float GetHP_Max() const { return HP_Max; }
	float GetAP_Max() const { return AP_Max; }
	float GetHP_LIM() const { return HP_LIM; }
	float GetAP_LIM() const { return AP_LIM; }
	float GetCameraYaw();

	float HP_LIM;
	float AP_LIM;
	float HP_BLINK;
	float HP_RECOVER;
	float HP_Max;
	float HP_Curr;
	float AP_Max;
	float AP_Curr;

private:
	// function
	void Set_y(float AxisValue);
	void Set_x(float AxisValue);
	void Look_x(float AxisValue);
	void Look_y(float axisValue);
	void Sprint();
	void Blink();
	void Jump();
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void HealBegin();
	void HealEnd();
	void Heal(float Delta);
	void Atack_Normal();
	void ChangeCameraPos(float DeltaTime);
	void ChangeLockMode();
	bool FindEnemy();
	//FVector GetVelocity();

	// variable
	bool isSprint;
	bool isBlink;
	bool isJump;
	bool isGrounded;
	bool isHealing;
	bool isLocked;
	float HEAL_MAX;
	FVector velocity;
	FVector2D rotation;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjects;

	TObjectPtr<AActor> LockedEnemy;
};
