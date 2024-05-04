// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameData/QLTurningInPlaceType.h"
#include "GameData/QLItemType.h"
#include "GameplayEffectTypes.h"
#include "Components/TimelineComponent.h"
#include "QLCharacterPlayer.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class AQLItem*);
DECLARE_DELEGATE_OneParam(FOnTakeItemDestoryDelegate, class AQLItemBox*);
USTRUCT(BlueprintType)
struct FTakeItemDelegateWrapper
{
	GENERATED_BODY()
	FTakeItemDelegateWrapper() {}
	FTakeItemDelegateWrapper(const FOnTakeItemDelegate& InItemDelegate) : ItemDelegate(InItemDelegate) {}
	FOnTakeItemDelegate ItemDelegate;
};

UCLASS()
class QUADLAND_API AQLCharacterPlayer : public AQLCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AQLCharacterPlayer(const FObjectInitializer& ObjectInitializer);

	//Default
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	//Enhanced Input - Action 연결
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetCharacterControl();

	//ASC
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//FORCEINLINE
	 class UAnimMontage* GetAnimMontage() const { return AttackAnimMontage[CurrentAttackType]; }

	 const ECharacterAttackType& GetCurrentAttackType() const { return CurrentAttackType; }

	 class UQLPunchAttackData* GetPunchAttackData() { return PunchAttackData; }

	 virtual void OnRep_PlayerState() override;

	 virtual void Tick(float DeltaSeconds) override;

	//Attack을 위한 카메라 위치 가져오기
	FVector CalPlayerLocalCameraStartPos();
	
	FVector GetCameraForward();

	//const class UQLWeaponStat* GetWeaponStat() const;
	FORCEINLINE bool GetHasGun() const { return bHasGun; }
	FORCEINLINE bool GetIsCrunching() const { return bIsCrouched; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE bool GetIsReload() const { return bIsReload; }
	FORCEINLINE bool GetIsShooting() const { return bIsShooting; }
	FORCEINLINE bool GetPickup() const { return bPressedFarmingKey; }

	FORCEINLINE void SetIsReload(bool Reload) { bIsReload = Reload; }

	FORCEINLINE ETurningPlaceType GetTurningInPlaceType() const { return TurningInPlace; }
	FORCEINLINE const class UQLWeaponComponent* GetWeapon() const { return Weapon; }
	FORCEINLINE float GetCurrnetYaw() { return CurrentYaw; }
	UFUNCTION(Server, Unreliable)
	void ServerRPCShooting(); //효과음이기 때문에 굳이 Reliable 일 필요 없음.
	UFUNCTION(Server, Reliable)
	void ServerRPCReload(); //Reload 행위는 Reliable
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FarmingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PutWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> VisibilityInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	void Move(const FInputActionValue& Value); //이동 매칭
	void Look(const FInputActionValue& Value); //마우스 시선 이동

	void JumpPressed();

	void SetupStartAbilities();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//Movement Section
protected:
	FORCEINLINE float CalculateSpeed();
	FRotator PreviousRotation;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	//Turning in Place Section
protected:
	ETurningPlaceType TurningInPlace;
	
	void RotateBornSetting(float DeltaTime);
	void TurnInPlace(float DeltaTime);

	float InterpYaw; //보간용도
	float CurrentYaw;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TObjectPtr<class UQLPunchAttackData> PunchAttackData;

	uint8 bHasNextPunchAttackCombo : 1;

	int CurrentCombo;

	FTimerHandle PunchAttackComboTimer;

	//GAS
protected:

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	//GAS - Input Abilities
	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> InputAbilities;

	void GASInputPressed(int32 id);
	void GASInputReleased(int32 id);
	int8 GetInputNumber(int32 id);
	void SetupGASInputComponent();

//파밍 시스템을 위한 변수
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLWeaponComponent> Weapon;
	
	UFUNCTION(Server, Reliable)
	void ServerRPCFarming();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCFarming(class UQLWeaponStat *Stat);

	UPROPERTY(Replicated) //복제만 수행하면 된다.
	uint8 bPressedFarmingKey : 1;

	int32 FarmingTraceDist;

	void FarmingItemPressed();
	void FarmingItemReleased();

	void FarmingItem();
	void EquipWeapon(class AQLItem* ItemInfo);
	void GetItem(class AQLItem* ItemInfo);
	void HasLifeStone(class AQLItem* ItemInfo);
	void GetAmmo(class AQLItem* IItemInfo);
	//Take
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;
	
	FOnTakeItemDestoryDelegate TakeItemDestory;

	UFUNCTION()
	void DestoryItem(class AQLItemBox* Item);

protected:
	void PressedCrouch();

	UPROPERTY()
	TObjectPtr<class UTimelineComponent> CameraDownTimeline;

	UPROPERTY()
	TObjectPtr<class UCurveFloat> CameraUpDownCurve;

	FOnTimelineFloat DownInterpFunction{};

	UFUNCTION()
	void TimelineCameraUpDownFloatReturn(float Alpha);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ArmHeight, Meta = (AllowPrivateAccess = "true"))
	float MaxCameraHeight = 48.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ArmHeight, Meta = (AllowPrivateAccess = "true"))
	float MinCameraHeight = 28.0f;

protected:
	uint8 bIsAiming : 1;

	void Aim();
	void StopAiming();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	
	//카메라 줌인 작업을 위해서 TimelineComponent 사용
	UPROPERTY()
	TObjectPtr<class UTimelineComponent> ZoomInTimeline;

	UPROPERTY()
	TObjectPtr<class UCurveFloat> AimAlphaCurve;

	FOnTimelineFloat AimInterpFunction{};

	UFUNCTION()
	void TimelineFloatReturn(float Alpha);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ArmLength, Meta = (AllowPrivateAccess = "true"))
	float MaxArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ArmLength, Meta = (AllowPrivateAccess = "true"))
	float MinArmLength;

	//Weapon Section

protected:
	
	UPROPERTY(Replicated)
	uint8 bIsReload : 1;

	UPROPERTY(Replicated)
	uint8 bIsShooting : 1;

	UFUNCTION()
	void InitializeAttributes();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PutLifeStoneAction;

	//Put LifeStone 
	 //서버에게 눌렀음을 전달
	void PutLifeStone();

	//무기를 버림
	void PutWeapon();

	UFUNCTION(Server,WithValidation, Reliable)
	void ServerRPCPuttingWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPuttingWeapon();

	//Inventory Section

protected:
	UFUNCTION(Client, Reliable)
	void ClientRPCAddItem(UQLItemData* Item, int32 ItemCnt);

	uint8 bIsSetVisibleInventory : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, Meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, int32> InventoryItem;

public:
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerRPCRemoveItem(EItemType ItemId, int32 ItemCnt);

	UFUNCTION(Client,Reliable)
	void ClientRPCRemoveItem(UQLItemData* Item, int32 ItemCnt);

	UFUNCTION(BlueprintCallable)
	void SetInventory();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	float SearchRange;

	void UseItem(EItemType ItemId); //아이템을 사용
	void AddInventoryByDraggedItem(EItemType ItemId, int32 ItemCnt);

	UFUNCTION(Server, Reliable)
	void ServerRPCAddInventoryByDraggedItem(EItemType ItemId, int32 ItemCnt);
	UFUNCTION(Client, Reliable)
	void ClientRPCRollbackInventory(EItemType ItemId, int32 ItemCnt);

	void AddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt);

	UFUNCTION(Server, Reliable)
	void ServerRPCAddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt);

};
