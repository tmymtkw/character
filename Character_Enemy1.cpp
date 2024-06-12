// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_Enemy1.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACharacter_Enemy1::ACharacter_Enemy1()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Comp_Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Comp_SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeltalMesh"));

	RootComponent = Comp_Capsule;

	Comp_Capsule->SetCapsuleSize(30.0f, 90.0f);
	Comp_Capsule->OnComponentBeginOverlap.AddDynamic(this, &ACharacter_Enemy1::OnOverlapBegin);
	Comp_Capsule->SetHiddenInGame(false);
	Comp_Capsule->SetCollisionProfileName(TEXT("CharacterCollision"));

	TObjectPtr<USkeletalMesh> Mesh = LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin"));
	Comp_SkeletalMesh->SetupAttachment(RootComponent);
	Comp_SkeletalMesh->SetSkeletalMesh(Mesh);
	Comp_SkeletalMesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	Comp_Attack = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EnemyAttack"));
	Comp_Attack->SetupAttachment(RootComponent);
	Comp_Attack->SetRelativeLocation(FVector::ForwardVector * 60.0f);
	Comp_Attack->SetCapsuleSize(30.0f, 30.0f);
	Comp_Attack->SetHiddenInGame(false);
	Comp_Attack->ComponentTags.Add("Attack");
	Comp_Attack->ComponentTags.Add("Enemy");
}

// Called when the game starts or when spawned
void ACharacter_Enemy1::BeginPlay()
{
	Super::BeginPlay();
	
	HP = 500;
}

// Called every frame
void ACharacter_Enemy1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UKismetSystemLibrary::PrintString(this, FString::FromInt(HP), true, false, FColor::Black, DeltaTime, TEXT("None"));
}

void ACharacter_Enemy1::OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
																			 AActor* OtherActor, UPrimitiveComponent* OtherComp,
																			 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	DecreaseHP();
}

void ACharacter_Enemy1::DecreaseHP() {
	HP -= 100;
}