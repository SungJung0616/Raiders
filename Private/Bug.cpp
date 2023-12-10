// Fill out your copyright notice in the Description page of Project Settings.


#include "Bug.h"
#include "Components/CapsuleCOmponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABug::ABug()
{
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = Body;
	Body->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere"));
	if (SphereMesh.Succeeded())
	{
		Body->SetStaticMesh(SphereMesh.Object);
	}

	Active = false;
	PoolIndex = -1;

}

// Called when the game starts or when spawned
void ABug::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABug::Tick(float DeltaTime)
{


}

void ABug::SetActive(bool IsActive)
{
	Active = IsActive;
	SetActorHiddenInGame(!IsActive);
	GetWorldTimerManager().SetTimer(LifeSpanTimer, this, &ABug::Deactivate, LifeSpan, false);
}

void ABug::SetLifeSpan(float LifeTime)
{
	LifeSpan = LifeTime;
}

void ABug::SetPoolIndex(int Index)
{
	PoolIndex = Index;
}

bool ABug::IsActive()
{
	return Active;
}

void ABug::Deactivate()
{
	SetActive(false);
	GetWorldTimerManager().ClearAllTimersForObject(this);
	OnPooledObjectDespawn.Broadcast(this);
}

int ABug::GetPoolIndex()
{
	return PoolIndex;
}

