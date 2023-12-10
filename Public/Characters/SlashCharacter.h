// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "BaseCharacter.h"
#include "Interfaces/Dialogue.h"
#include "Interfaces/PickupInterface.h"
#include "CharacterTypes.h"
#include "NPCCharacter.h"
#include "HUD/DialoguUI.h"
#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class ATreasure;
class ASoul;
class UAnimMontage;
class UInputMappingContext;
class UInputAction;
class USlashOverlay;
class ABug;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	void SetHUDHealth();
	virtual void SetOverlappintItem(class AItem* Item) override;
	virtual void AddSoulds(ASoul* Soul) override;
	virtual void AddGolds(ATreasure* Treasure) override;
	
	/* <IHitInterFace> */
	


	/* </IHitInterFace> */

	/** Callbacks for input */

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	virtual void Jump() override;
	bool IsUnoccupied();
	void EkeyPressed();
	
	
	virtual void Attack()override;
	void Combo();

	void PlayComboMontage();
	void Dodge();

	void EquipWeapon(AWeapon* Weapon);
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	
	void PlayEquipMontage(const FName& SectionName); //여기에선 필요한 부분의 Section Name 만 필요함
	virtual void Die() override;

	bool HasEnoughStamina();
	bool IsOccupied();

	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();
	
	bool bIsInAttakCheckWhenAttck;
	int AttackCount;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void AttackInputCheck();

	/*Objec Pool System*/
	void FkeyPressed();

	UFUNCTION()
	void SpawnPooledObjectTimerFunction();
	
	UPROPERTY(EditAnywhere, Category = "Object Pool")
	TSubclassOf<ABug> PooledObjectSubclass;

	FTimerHandle SpawnTimerHandle;

	int32 NumSpawnedObjects = 0;

	void InitializeObjectPool();

	UPROPERTY(EditAnywhere, Category = "Object Pool")
	int PoolSize = 10;

	UPROPERTY(EditAnywhere, Category = "Object Pool")
	float PooledObjectLifeSpan = 9.0f;

	UFUNCTION()
	ABug* SpawnPooledObject();

	UFUNCTION()
	void OnPooledObjectDespawn(ABug* PoolActor);

	TArray<ABug*> ObjectPool;
	TArray<int> SpawnedPoolIndexes;

	

	

	/*UFUNCTION(BlueprintCallable)
	void DisableWeaponCollision();*/

	/*Dialogue System*/

	/*Generates the player lines*/
	//void GeneratePlayerLines(UDataTable& PlayerLines);

	///*This array is essentially an Array of Excerpts from our dialogs!*/
	//UPROPERTY(BlueprintReadOnly)
	//TArray<FString> Questions;

	///*Performs the actual talking - informs the associated pawn if necessary in order to answer
	//The subtitles array contain all the subtitles for this talk - it should be passed to our UI*/
	//UFUNCTION(BlueprintCallable, Category = DialogSystem)
	//void Talk(FString Excerpt, TArray<FSubtitle>& Subtitles);

	///*Enables / disables our talk ability. The player can't talk if he's not in a valid range*/
	//void SetTalkRangeStatus(bool Status) { bIsInTalkRange = Status; }

	///*Sets a new associated pawn*/
	//void SetAssociatedPawn(ANPCCharacter* Pawn) { AssociatedPawn = Pawn; }

	///*Retrieves the UI reference*/
	//UDialoguUI* GetUI() { return UI; }
private:
	void InitializeSlashOverlay();

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/* Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	UGroomComponent* Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* ComboMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> ComboMontageSections;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	///* Dialogue system */
	///*True if the player is currently talking with any pawn*/
	//bool bIsTalking;

	///*True if the player is inside a valid range to start talking to a pawn*/
	//bool bIsInTalkRange;

	/*Initiates or terminates a conversation*/
	/*UFUNCTION(BlueprintCallable)
	void Interact();*/

	///*The pawn that the player is currently talking to*/
	//ANPCCharacter* AssociatedPawn;

	///*A reference to our lines - retrieved from the associated pawn*/
	//UDataTable* AvailableLines;

	///*Searches in the given row inside the specified table*/
	//FDialog* RetrieveDialog(UDataTable* TableToSearch, FName RowName);

protected:
	virtual void BeginPlay() override;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* FKeyAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ComboAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* SlashContext;
	
	///*Opens or closes the conversation UI*/
	//UFUNCTION(BlueprintImplementableEvent, Category = DialogSystem)
	//void ToggleUI();

	///*UI Reference*/
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	//UDialoguUI* UI;

public:
	//인라인함수사용 FORCEINLINE = 인라인 사용 매크로
	
	//const 의 활용
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
