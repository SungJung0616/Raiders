// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:		
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);

protected:
	
	virtual void BeginPlay() override;

private:
	//Current Health
	UPROPERTY(Editanywhere, Category = "Actor Attributes")
	float Health = 100;

	UPROPERTY(Editanywhere, Category = "Actor Attributes")
	float MaxHealth = 100;
	
	//Current Stamina
	UPROPERTY(Editanywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(Editanywhere, Category = "Actor Attributes")
	float MaxStamina;
	
	UPROPERTY(Editanywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(Editanywhere, Category = "Actor Attributes")
	int32 Souls;

	UPROPERTY(Editanywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(Editanywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;
	
public:
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	
	float GetHealthPercent();
	float GetStaminaPercent();
	
	bool IsAlive();
	void AddSouls(int32 NumberofSoul);

	void AddGolds(int32 NumberofGold);

	FORCEINLINE int32 GetGolds() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
};
