// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/SlashCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Bug.h"



ASlashCharacter::ASlashCharacter()
{ 	
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->AddRelativeRotation(FRotator(-20.f, 0.f, 0.f));

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	ViewCamera->SetupAttachment(CameraBoom);
	ViewCamera->bUsePawnControlRotation = false;

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair-> AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
		
	bIsInAttakCheckWhenAttck = false;
	AttackCount = 0;
		
	PooledObjectSubclass = ABug::StaticClass();
}

void ASlashCharacter::Tick(float DeltaTime)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay -> SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Movement
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		//Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		//Ekeypress
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EkeyPressed);
		//Attack				
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		//shift
		EnhancedInputComponent->BindAction(ComboAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Combo);
		//Fkeypress
		EnhancedInputComponent->BindAction(FKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::FkeyPressed);
		//Dodge
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
	}

}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}

	Tags.Add(FName("EngageableTarget"));

	InitializeSlashOverlay();
	InitializeObjectPool();
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (IsOccupied()) return;
	const FVector2d  MovementVector = Value.Get<FVector2d>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (Controller)
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

void ASlashCharacter::EkeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void ASlashCharacter::Dodge()
{
	

	if (IsOccupied() || !HasEnoughStamina()) return;
		
	PlayDodgeMontage();
	
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && SlashOverlay)
	{
		Attributes->UseStamina(Attributes -> GetDodgeCost());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());		
	}
}

bool ASlashCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool ASlashCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::Combo()
{
	if (CanAttack())
	{
		UE_LOG(LogTemp, Warning, TEXT("Current attackcount: %d"), AttackCount);
		PlayComboMontage();
		ActionState = EActionState::EAS_Attacking;
		if (ComboMontageSections.Num() > 0)
		{
			AttackCount++;
			
			if (AttackCount >= ComboMontageSections.Num())
			{
				AttackCount = 0;
			}
		}
	}
}


void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr; // OverlappingWeapon 이 아닌 overlappingitem
	EquippedWeapon = Weapon;
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount && Attributes);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}
//블루프린트랑 연동 시킴
void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;

}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::AttackInputCheck()
{	
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && 
			CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
			CharacterState != ECharacterState::ECS_Unequipped;		
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
			CharacterState == ECharacterState::ECS_Unequipped &&
			EquippedWeapon;
}
void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}
//블루프린트랑 연동 시킴
void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}
//블루프린트랑 연동 시킴
void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	AttackCount = 0;
}

void ASlashCharacter::Die()
{
	Super::Die();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

//PickupInterface
void ASlashCharacter::SetOverlappintItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSoulds(ASoul* Soul)
{
	if (Attributes)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGolds(ATreasure* Treasure)
{
	if (Attributes)
	{
		Attributes->AddGolds(Treasure->GetGolds());
		SlashOverlay->SetGolds(Attributes->GetGolds());
	}
}

//PlayMontage() 
void ASlashCharacter::PlayComboMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ComboMontage)
	{
		if (ComboMontageSections.Num() <= 0) return;
		AnimInstance->Montage_Play(ComboMontage);
		//AnimInstance->Montage_Play(AttackMontage2);
		AnimInstance->Montage_JumpToSection(ComboMontageSections[AttackCount], ComboMontage);
	}
}
void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}


bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

//UserWidget 
void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetGolds(0);
				SlashOverlay->SetSouls(0);
			}
		}
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

//Object Pool

void ASlashCharacter::FkeyPressed()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(SpawnTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, TEXT("Timer Off"));
	}
	else // 타이머가 비활성화되어 있다면, 타이머를 시작합니다.
	{
		GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ASlashCharacter::SpawnPooledObjectTimerFunction, 1.0f, true, 0.0f);
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, TEXT("Timer ON"));
	}
}

void ASlashCharacter::SpawnPooledObjectTimerFunction()
{
	SpawnPooledObject();
}

void ASlashCharacter::InitializeObjectPool()
{
	if (PooledObjectSubclass != nullptr)
	{
		UWorld* const World = GetWorld();

		if (World != nullptr)
		{
			for (int i = 0; i < PoolSize; i++)
			{
				FVector Location = GetActorLocation();
				FRotator Rotation = GetActorRotation();

				ABug* PooledObject = World->SpawnActor<ABug>(PooledObjectSubclass, Location, Rotation);

				if (PooledObject != nullptr)
				{
					PooledObject->SetActive(false);
					PooledObject->SetPoolIndex(i);
					PooledObject->OnPooledObjectDespawn.AddDynamic(this, &ASlashCharacter::OnPooledObjectDespawn);
					ObjectPool.Add(PooledObject);

				}
			}
		}

	}
}

ABug* ASlashCharacter::SpawnPooledObject()
{
	FVector Location = GetActorLocation() + FMath::VRand() * FMath::RandRange(0.0f, 500.0f);
	FRotator Rotation = GetActorRotation();

	for (ABug* PoolableActor : ObjectPool)
	{
		if (PoolableActor != nullptr && !PoolableActor->IsActive())
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, TEXT("SphereObject"));
			PoolableActor->TeleportTo(Location, Rotation);
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, FString::Printf(TEXT("Spawned Object Location: X=%f, Y=%f, Z=%f"), Location.X, Location.Y, Location.Z));
			PoolableActor->SetLifeSpan(PooledObjectLifeSpan);
			PoolableActor->SetActive(true);
			SpawnedPoolIndexes.Add(PoolableActor->GetPoolIndex());

			return PoolableActor;
		}
	}

	if (SpawnedPoolIndexes.Num() > 0)
	{
		int PooledObjectIndex = SpawnedPoolIndexes[0];
		SpawnedPoolIndexes.Remove(PooledObjectIndex);
		ABug* PoolableActor = ObjectPool[PooledObjectIndex];

		if (PoolableActor != nullptr)
		{
			PoolableActor->SetActive(false);
			PoolableActor->TeleportTo(FVector(0, 0, 0), FRotator(0, 0, 0));
			PoolableActor->SetLifeSpan(PooledObjectLifeSpan);
			PoolableActor->SetActive(true);
			SpawnedPoolIndexes.Add(PoolableActor->GetPoolIndex());

			return PoolableActor;
		}
	}
	return nullptr;
}

void ASlashCharacter::OnPooledObjectDespawn(ABug* MyBug)
{
	SpawnedPoolIndexes.Remove(MyBug->GetPoolIndex());
	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, TEXT("Despawn"));
}

//void ASlashCharacter::ToggleTalking() //interact 로 바꿔야함. 
//{
//	if (bIsInTalkRange)
//	{
//		//If we are in talk range handle the talk status and the UI
//		bIsTalking = !bIsTalking;
//		ToggleUI();
//		if (bIsTalking && AssociatedPawn)
//		{
//			//The associated pawn is polite enough to face us when we talk to him!
//			FVector Location = AssociatedPawn->GetActorLocation();
//			FVector TargetLocation = GetActorLocation();
//
//			AssociatedPawn->SetActorRotation((TargetLocation - Location).Rotation());
//		}
//
//	}
//}
///// 여기가지는 작동 
//
//void ASlashCharacter::GeneratePlayerLines(UDataTable& PlayerLines)
//{
//	//Get all the row names of the table
//	TArray<FName> PlayerOptions = PlayerLines.GetRowNames();
//
//	//For each row name try to retrieve the contents of the table
//	for (auto It : PlayerOptions)
//	{
//		//Retrieve the contents of the table
//		FDialog* Dialog = RetrieveDialog(&PlayerLines, It);
//
//		if (Dialog)
//		{
//			//We retrieved a valid row - populate the questions array with our excerpts
//			Questions.Add(Dialog->QuestionExcerpt);
//		}
//	}
//
//	//Make sure to create a reference of the available line for later use
//	AvailableLines = &PlayerLines;
//}
//
//FDialog* ASlashCharacter::RetrieveDialog(UDataTable* TableToSearch, FName RowName)
//{
//	if (!TableToSearch) return nullptr;
//
//	//The table is valid - retrieve the given row if possible
//	FString ContextString;
//
//	return TableToSearch->FindRow<FDialog>(RowName, ContextString);
//}
//
//
//
//void ASlashCharacter::Talk(FString Excerpt, TArray<FSubtitle>& Subtitles)
//{
//	//Get all the row names based on our stored lines
//	TArray<FName> PlayerOptions = AvailableLines->GetRowNames();
//
//	for (auto It : PlayerOptions)
//	{
//		//Search inside the available lines table to find the pressed Excerpt from the UI
//		FDialog* Dialog = RetrieveDialog(AvailableLines, It);
//
//		if (Dialog && Dialog->QuestionExcerpt == Excerpt)
//		{
//			//Update the corresponding subtitles
//			Subtitles = Dialog->Subtitles;
//
//
//			if (UI && AssociatedPawn && Dialog->bShouldNPCAnswer)
//			{
//				//Calculate the total displayed time for our subtitles
//				//When the subtitles end - the associated pawn will be able to talk to our character
//
//				TArray<FSubtitle> SubtitlesToDisplay;
//
//				float TotalSubsTime = 0.f;
//
//				for (int32 i = 0; i < Subtitles.Num(); i++)
//				{
//					TotalSubsTime += Subtitles[i].AssociatedTime;
//				}
//
//				//Just a hardcoded value in order for the AI not to answer right after our subs.
//				//It would be better if we expose that to our editor? Sure!
//				TotalSubsTime += 1.f;
//
//				//Tell the associated pawn to answer to our character after the specified time!
//				AssociatedPawn->AnswerToCharacter(It, SubtitlesToDisplay, TotalSubsTime);
//
//			}
//			else if (!Dialog->bShouldNPCAnswer) ToggleTalking();
//			break;
//
//		}
//	}
//}