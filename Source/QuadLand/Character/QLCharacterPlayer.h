// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/QLCharacterBase.h"
#include "InputActionValue.h"
#include "QLCharacterPlayer.generated.h"

/**
 * 
 */
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
	TObjectPtr<class UInputAction> FightingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	void Move(const FInputActionValue& Value); //�̵� ��Ī
	void Look(const FInputActionValue& Value); //���콺 �ü� �̵�
	void Fight();
	void FarmingItem();
	void Run();

	virtual void PossessedBy(AController* NewController) override;
protected:
	

};
