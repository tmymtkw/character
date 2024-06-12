// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class MYPROJECT_API UStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SPBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SPBar_Max;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> APBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SPBar_text;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> APBar_text;

	UFUNCTION()
	FText SetSPText();

	UFUNCTION()
	FText SetAPText();

	UFUNCTION()
	float SetSP();

	UFUNCTION()
	float SetSPMax();

	UFUNCTION()
	float SetAP();

	float SPLIM;
	float APLIM;
};
