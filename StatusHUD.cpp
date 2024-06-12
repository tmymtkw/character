// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"

void AStatusHUD::BeginPlay() {
	FString StatusWidgetPath = TEXT("/Game/BP/UI/BP_Status.BP_Status_C");
	TSubclassOf<UUserWidget> StatusWidgetClass = TSoftClassPtr<UUserWidget>(FSoftObjectPath(*StatusWidgetPath)).LoadSynchronous();

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (StatusWidgetClass && PlayerController) {
		TObjectPtr<UUserWidget> StatusWidget = UWidgetBlueprintLibrary::Create(GetWorld(), StatusWidgetClass, PlayerController);
		//StatusWidget->SetVisibility(ESlateVisibility::Collapsed);
		StatusWidget->AddToViewport(0);
	}
}