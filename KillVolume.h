// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillVolume.generated.h"

class UBoxComponent;

UCLASS()
class MYPROJECT_API AKillVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKillVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnyWhere)
	TObjectPtr<UBoxComponent> KillVolume;

	UPROPERTY(EditAnywhere)
	FVector BoxExtent = FVector(30.0f, 30.0f, 30.0f);

private:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp,
												 AActor* OtherActor, UPrimitiveComponent* OtherComp,
												 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
