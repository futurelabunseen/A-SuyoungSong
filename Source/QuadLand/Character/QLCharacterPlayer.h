// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "Interface/QLLifestoneContainerInterface.h"
#include "GameData/QLItemType.h"
#include "GameplayEffectTypes.h"
#include "Components/TimelineComponent.h"

#include "QLCharacterPlayer.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_TwoParams(FOnChagneShootingMethod, bool,bool);
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
class QUADLAND_API AQLCharacterPlayer : public AQLCharacterBase,public IQLLifestoneContainerInterface
{
	GENERATED_BODY()
	
public:

	FOnChagneShootingMethod OnChangeShootingMethod;
	AQLCharacterPlayer(const FObjectInitializer& ObjectInitializer);

	//Default
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	//Enhanced Input - Action 연결
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetCharacterControl();

	 const ECharacterAttackType& GetCurrentAttackType() const { return CurrentAttackType; }

	 class UQLPunchAttackData* GetPunchAttackData() { return PunchAttackData; }

	 virtual void OnRep_PlayerState() override;
	 virtual void OnRep_Controller() override;

	 virtual void Tick(float DeltaSeconds) override;
;
	//Attack을 위한 카메라 위치 가져오기
	FVector CalPlayerLocalCameraStartPos();
	FVector GetCameraForward();

	//const class UQLWeaponStat* GetWeaponStat() const;

	UFUNCTION()
	void UpdateAmmo(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION(Server,Unreliable)
	void ServerRPCUpdateAmmo(uint32 ItemCnt);
	FORCEINLINE bool GetHasGun() const { return bHasGun; }
	FORCEINLINE bool GetPickup() const { return bPressedFarmingKey; }
	FORCEINLINE bool GetIsProning() const { return bIsProning; }
	bool GetIsJumping();
	FORCEINLINE void SetIsProning(bool IsProning) { bIsProning = IsProning; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

	class UQLInventoryComponent* GetInventory() { return QLInventory; }

	int GetInventoryCnt(EItemType ItemType);

	UFUNCTION(Server,Unreliable)
	void ServerRPCInitNickname();

	void InitNickname();
	UFUNCTION(Server, Unreliable)

	void ServerRPCDetachBomb();

	void StopAim();

	bool IsMontagePlaying(class UAnimMontage* Montage) const;

	uint8 ThrowBomb : 1;

	UFUNCTION(Client,Unreliable)
	void ClientRPCThrowBomb();

protected:

	FVector ProneMeshLoc;
	float ProneCapsuleRadius;
	float ProneCapsuleHeight;

	FVector StandMeshLoc;
	float OriginalCapsuleRadius;
	float OriginalCapsuleHeight;

protected:

	uint8 bIsProning : 1;
protected:

	virtual FVector GetVelocity() const override;

	float MovingThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ArmLength, Meta = (AllowPrivateAccess = "true"))
	float MaxArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ArmLength, Meta = (AllowPrivateAccess = "true"))
	float MinArmLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLInputComponent> QLInputComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLInventoryComponent> QLInventory;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, Meta = (AllowPrivateAccess = "true"))
	float SearchRange;

	UFUNCTION(Server, Unreliable)
	void ServerRPCSwitchAttackType(ECharacterAttackType InputKey);


	void RotateWidgetComponent();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCSwitchAttackType(ECharacterAttackType InputKey);

	//*GAS Event Callback 함수*/

	void ResetNotEquip(const FGameplayTag CallbackTag, int32 NewCount);
	void ResetEquipTypeA(const FGameplayTag CallbackTag, int32 NewCount);
	void ResetBomb(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	void SetupStartAbilities();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TObjectPtr<class UAnimMontage> PickupMontage;

	UFUNCTION(Client,Unreliable)
	void ClientRPCPlayAnimation(AQLCharacterPlayer* CharacterPlay);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TObjectPtr<class UQLPunchAttackData> PunchAttackData;

	uint8 bHasNextPunchAttackCombo : 1;

	int CurrentCombo;

	FTimerHandle PunchAttackComboTimer;

	//GAS
protected:

	//GAS - Input Abilities
	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> InputAbilities;

//파밍 시스템을 위한 변수
protected:
	UFUNCTION(Server, Reliable)
	void ServerRPCFarming();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCFarming(class UQLWeaponStat*Stat);

	//리플리케이션(ReplicatedUsing = 함수명) -> 도착 알림 이벤트 함수 
	//Replicated
	UPROPERTY(Replicated) //복제만 수행하면 된다.
	uint8 bPressedFarmingKey : 1;

	int32 FarmingTraceDist;

	void FarmingItem();
	void EquipWeapon(class AQLItem* ItemInfo);
	void HasLifeStone(class AQLItem* ItemInfo);
	void GetAmmo(class AQLItem* IItemInfo);
	//Take
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;
	
	FOnTakeItemDestoryDelegate TakeItemDestory;

	UFUNCTION()
	void DestoryItem(class AQLItemBox* Item);

protected:

	UFUNCTION()
	void InitializeGAS();
	UFUNCTION()
	void ServerInitializeGAS();
protected:

	UFUNCTION(Server,WithValidation, Reliable)
	void ServerRPCPuttingWeapon();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPuttingWeapon();

	uint8 bIsNearbyBox : 1; //오버랩 될 때 변경할 부울 변수

public:
	virtual void CheckBoxOverlap() override; 
	//Inventory Section
	void GetItem(class AQLItem* ItemInfo);

	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Bomb, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USplineComponent> BombPath;

	UPROPERTY()
	uint8 bThrowBomb : 1;

public:
	
	class USplineComponent* GetBombPath() { return BombPath; }

	void SetThrowBomb(bool OutThrowBomb) { bThrowBomb = OutThrowBomb; }
	bool GetThrowBomb() { return bThrowBomb; }
	UFUNCTION()
	void OnPlayMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);
	

	uint8 bIsSemiAutomatic : 1;
	friend class UQLInventoryComponent;

	friend class UQLInputComponent;


	//총 반동에 대해서 
protected:
	FTimeline RecoilTimeline;

	UPROPERTY()
	TObjectPtr<class UCurveFloat> HorizontalRecoil;

	UPROPERTY()
	TObjectPtr<class UCurveFloat> VerticalRecoil;

	UFUNCTION()
	void StartHorizontalRecoil(float Value);

	UFUNCTION()
	void StartVerticalRecoil(float Value);
public:

	void StartRecoil();

	void ReverseRecoil();

protected:
	float StartHeight;
	void ProneToStand();

	void StandToProne();


public:
	void UpdateAmmoTemp();

	UFUNCTION(Client,Unreliable)
	void ClientRPCUpdateAmmoUI();

};
