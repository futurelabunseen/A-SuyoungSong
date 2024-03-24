// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "InputActionValue.h"
#include "QLCharacterPlayer.generated.h"

/**
 * 
 */

UENUM()
enum class ECharacterAttackType : uint8
{
	HookAttack,
	GunAttack
};

UCLASS()
class QUADLAND_API AQLCharacterPlayer : public AQLCharacterBase
{
	GENERATED_BODY()
	
public:
	AQLCharacterPlayer();
	//Default
	virtual void BeginPlay() override;
//Input Section
	//Enhanced Input - Action ����
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetCharacterControl();
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

	void Move(const FInputActionValue& Value); //�̵� ��Ī
	void Look(const FInputActionValue& Value); //���콺 �ü� �̵�
	void Attack();
	void FarmingItem();

	//Run Section
protected:
	void RunInputPressed();
	void RunInputReleased();

	uint8 bIsFirstRunSpeedSetting : 1;
	//Attack Section
protected:

	//TMap���� ��������. Gun ���� �� Montage�� Gun ���� �� Montage �� ����� ���� - enum���� �з� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TMap<ECharacterAttackType, TObjectPtr<class UAnimMontage>> AttackAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TObjectPtr<class UQLPunchAttackData> PunchAttackData;

	void PunchAttackComboBegin();
	void PunchAttackComboEnd(class UAnimMontage* AnimMontage, bool IsProperlyEnded); //MontageDelegate ȣ��� �� �ֵ��� ���� ����
	
	void SetPunchComboCheckTimer();
	void PunchAttackComboCheck();

	virtual void DefaultAttack() override;
	
	uint8 bHasGun : 1;
	uint8 bHasNextPunchAttackCombo : 1;
	
	int CurrentCombo;

	FTimerHandle PunchAttackComboTimer;
	ECharacterAttackType CurrentAttackType;

	virtual void AttackHitCheckUsingPunch() override;
};
