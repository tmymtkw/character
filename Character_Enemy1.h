// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character_Enemy1.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;

UCLASS()
class MYPROJECT_API ACharacter_Enemy1 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacter_Enemy1();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> Comp_Capsule;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> Comp_SkeletalMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> Comp_Attack;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
											AActor* OtherActor, UPrimitiveComponent* OtherComp,
											int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DecreaseHP();

private:
	int HP;
};
