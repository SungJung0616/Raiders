// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/NPCCharacter.h"
#include "Characters/SlashCharacter.h"
#include "Components/SphereComponent.h"


ANPCCharacter::ANPCCharacter()
{
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

void ANPCCharacter::BeginPlay()
{
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ANPCCharacter::OnSphereOverlap);

	Sphere->OnComponentEndOverlap.AddDynamic(this, &ANPCCharacter::OnBoxEndOverlap);
}

//void ANPCCharacter::AnswerToCharacter(FName PlayerLine, TArray<FSubtitle>& SubtitlesToDisplay, float delay)
//{
//	if (!NPCLines) return;
//
//	//Retrieve the corresponding line
//	FString ContextString;
//	FDialog* Dialog = NPCLines->FindRow<FDialog>(PlayerLine, ContextString);
//
//	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//
//	if (Dialog && SlashCharacter)
//	{
//		FTimerHandle TimerHandle;
//		FTimerDelegate TimerDel;
//
//		TimerDel.BindUFunction(this, FName("Talk"), Dialog->Subtitles);
//
//		//Talk to the player after the delay time has passed
//		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, delay, false);
//	}
//}

void ANPCCharacter::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	//if (OtherActor -> IsA<ASlashCharacter>())
	//{
	//	ASlashCharacter* SlashCharacter = Cast< ASlashCharacter>(OtherActor);
	//	SlashCharacter->SetTalkRangeStatus(true);
	//	SlashCharacter->GeneratePlayerLines(*PlayerLines);
	//	SlashCharacter->SetAssociatedPawn(this);
	//}

}

void ANPCCharacter::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	/*if (OtherActor->IsA<ASlashCharacter>())
	{
		ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(OtherActor);
		SlashCharacter->SetTalkRangeStatus(false);
		SlashCharacter->SetAssociatedPawn(nullptr);
	}*/
}

//void ANPCCharacter::Talk(TArray<FSubtitle> Subs)
//{
//	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
//		
//	//Tell the UI to update with the new subtitles
//	SlashCharacter->GetUI()->UpdateSubtitles(Subs);
//}
