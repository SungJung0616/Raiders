// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bug.generated.h"

class UCapsuleComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPooledObjectDespawn, ABug*, PoolActor);

UCLASS()
class SLASH_API ABug : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABug();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Pooled Object")
	void Deactivate();

	void SetActive(bool IsActive);
	void SetLifeSpan(float LifeTime);
	void SetPoolIndex(int Index);

	bool IsActive();
	int GetPoolIndex();
	FOnPooledObjectDespawn OnPooledObjectDespawn;
	FTimerHandle LifeSpanTimer;


	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Body;

	virtual void Tick(float DeltaTime) override;		

protected:
	bool Active;
	float LifeSpan;
	int PoolIndex;

public:	
	

};
