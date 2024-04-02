// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "QLCharacterPlayer.generated.h"

/**
 * 
 */


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
	 class UAnimMontage* GetAnimMontageUsingPunch() const { return AttackAnimMontage[ECharacterAttackType::HookAttack]; }

	 class UAnimMontage* GetAnimMontageUsingGun() const { return AttackAnimMontage[ECharacterAttackType::GunAttack]; }
	
	 const ECharacterAttackType& GetCurrentAttackType() const { return CurrentAttackType; }

	 class UQLPunchAttackData* GetPunchAttackData() { return PunchAttackData; }

	 virtual void OnRep_PlayerState() override;

	 virtual void Tick(float DeltaSeconds) override;
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

	//Run Section
protected:
	void RunInputPressed();
	void RunInputReleased();

	uint8 bIsFirstRunSpeedSetting : 1;
	//Attack Section
protected:

	//TMap으로 변경하자. Gun 없을 때 Montage와 Gun 있을 때 Montage 로 나누어서 관리 - enum으로 분류 예정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TMap<ECharacterAttackType, TObjectPtr<class UAnimMontage>> AttackAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TObjectPtr<class UQLPunchAttackData> PunchAttackData;

	uint8 bHasGun : 1;
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
	uint8 bPressedFarmingKey : 1;

	int32 FarmingTraceDist;

	void EquipWeapon(UQLWeaponStat* ItemInfo);

	void FarmingItemPressed();
	void FarmingItemReleased();
};
