// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Engine/DataTable.h"
#include "Subtitle.h"
#include "NPCCharacter.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class SLASH_API ANPCCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	ANPCCharacter();
	
	virtual void BeginPlay() override;

	///*Answers to the character after a specified delay*/
	//void AnswerToCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay);

	///*Retrieves the corresponding character lines*/
	//UDataTable* GetPlayerLines() { return PlayerLines; }

	///*UFUNCTION()
	//void Talk(TArray<FSubtitle> Subs);*/

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, Category = DialogSystem)
	UDataTable* PlayerLines;

	UPROPERTY(EditAnywhere, Category = DialogSystem)
	UDataTable* NPCLines;

public:
	
};
