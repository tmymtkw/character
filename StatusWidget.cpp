// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Character_Player.h"
#include "Character_Player2.h"
#include "Kismet/GameplayStatics.h"
#include "CustomGameInstance.h"

void UStatusWidget::NativeConstruct() {
	Super::NativeConstruct();
}

bool UStatusWidget::Initialize() {
	bool Success = Super::Initialize();

	if (!Success) return false;

	SPBar_text->TextDelegate.BindUFunction(this, "SetSPText");
	APBar_text->TextDelegate.BindUFunction(this, "SetAPText");
	SPBar->PercentDelegate.BindUFunction(this, "SetSP");
	APBar->PercentDelegate.BindUFunction(this, "SetAP");
	SPBar_Max->PercentDelegate.BindUFunction(this, "SetSPMax");

	return true;
}

FText UStatusWidget::SetSPText() {
	if (const ACharacter_Player2* Player = 
				Cast<ACharacter_Player2>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		return FText::FromString(FString::SanitizeFloat(Player->GetSP()));
	}

	return FText();
}

FText UStatusWidget::SetAPText() {
	if (const ACharacter_Player2* Player =
			Cast<ACharacter_Player2>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		return FText::FromString(FString::SanitizeFloat(Player->GetAP()));
	}

	return FText();
}

float UStatusWidget::SetSP() {
	if (const ACharacter_Player2* Player =
			Cast<ACharacter_Player2>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		return Player->GetSP() / Player->GetSPLIM();
	}

	return 0.0f;
}

float UStatusWidget::SetSPMax() {
	if (const ACharacter_Player2* Player =
			Cast<ACharacter_Player2>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		return Player->GetSPMax() / Player->GetSPLIM();
	}

	return 0.0f;
}


float UStatusWidget::SetAP() {
	if (const ACharacter_Player2* Player =
			Cast<ACharacter_Player2>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))) {
		return Player->GetAP() / Player->GetAPLIM();
	}

	return 0.0f;
}
