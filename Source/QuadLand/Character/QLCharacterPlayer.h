// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "GameData/QLTurningInPlaceType.h"
#include "GameData/QLItemType.h"
#include "GameplayEffectTypes.h"
#include "Interface/QLLifestoneContainerInterface.h"
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
class QUADLAND_API AQLCharacterPlayer : public AQLCharacterBase,public IQLLifestoneContainerInterface, public IAbilitySystemInterface
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
	
	int GetInventoryCnt(EItemType ItemType);
	FORCEINLINE void SetIsReload(bool Reload) { bIsReload = Reload; }

	FORCEINLINE ETurningPlaceType GetTurningInPlaceType() const { return TurningInPlace; }
	FORCEINLINE const class UQLWeaponComponent* GetWeapon() const { return Weapon; }
	FORCEINLINE float GetCurrnetYaw() { return CurrentYaw; }
	UFUNCTION(Server, Unreliable)
	void ServerRPCShooting(); //효과음이기 때문에 굳이 Reliable 일 필요 없음.
	UFUNCTION(Server, Reliable)
	void ServerRPCReload(); //Reload 행위는 Reliable
	class UQLInventoryComponent* GetInventory() { return QLInventory; }

protected:
	virtual FVector GetVelocity() const override;

	float MovingThreshold;
	uint8 bIsAiming : 1;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLInventoryComponent> QLInventory;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	float SearchRange;

	UFUNCTION(Server, Unreliable)
	void ServerRPCSwitchAttackType(ECharacterAttackType InputKey);

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

//파밍 시스템을 위한 변수
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLWeaponComponent> Weapon;
	
	UFUNCTION(Server, Reliable)
	void ServerRPCFarming();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCFarming(class UQLWeaponStat*Stat);

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
	
	UPROPERTY(Replicated)
	uint8 bIsReload : 1;

	UPROPERTY(Replicated)
	uint8 bIsShooting : 1;

	UFUNCTION()
	void InitializeAttributes();

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

	friend class UQLInventoryComponent;

	friend class UQLInputComponent;

};
