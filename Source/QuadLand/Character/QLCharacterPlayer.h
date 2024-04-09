// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "QLCharacterPlayer.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnTakeItemDelegate, class UQLItemData*);
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
	AQLCharacterPlayer();

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
	FORCEINLINE bool GetIsCrunching() const { return bIsCrunching; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE const class USkeletalMeshComponent* GetWeaponMesh() const { return Weapon; }
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	void Move(const FInputActionValue& Value); //이동 매칭
	void Look(const FInputActionValue& Value); //마우스 시선 이동
	//Movement Section
protected:
	void RunInputPressed();
	void RunInputReleased();

	uint8 bIsRunning : 1;
	//Attack Section
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

	void GASInputPressed();
	void GASInputReleased();
	void SetupGASInputComponent();

//파밍 시스템을 위한 변수
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Weapon;

	uint8 bPressedFarmingKey : 1;

	int32 FarmingTraceDist;

	void FarmingItemPressed();
	void FarmingItemReleased();

	void EquipWeapon(class UQLItemData* ItemInfo);

	//Take
	UPROPERTY()
	TArray<FTakeItemDelegateWrapper> TakeItemActions;
	
	FOnTakeItemDestoryDelegate TakeItemDestory;

	UFUNCTION()
	void DestoryItem(class AQLItemBox* Item);

protected:
	uint8 bIsCrunching : 1;

	void Crunch();

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

	UFUNCTION()
	void TimelineFloatReturn(float Alpha);

	FOnTimelineFloat InterpFunction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	float MaxArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	float MinArmLength;


};
