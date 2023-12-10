// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
//using progress bar in wbp_healthbar
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	
};
