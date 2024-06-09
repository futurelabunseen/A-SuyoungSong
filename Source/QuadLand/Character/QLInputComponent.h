// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameData/WeaponType.h"
#include "Components/PawnComponent.h"
#include "Components/TimelineComponent.h"
#include "QLInputComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUADLAND_API UQLInputComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UQLInputComponent(const FObjectInitializer& ObjectInitializer);

	void InitPlayerImputComponent(class UInputComponent* InputComponent);
	void InitGASInputComponent(class UInputComponent* InputComponent);
	virtual void BeginPlay() override;


protected:
	//Look & Move Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	void Move(const FInputActionValue& Value); //이동 매칭

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	void Look(const FInputActionValue& Value); //마우스 시선 이동


protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PutWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> FarmingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> VisibilityInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MapAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> PutLifeStoneAction;

	//ProneAction

	//Jump & Crouch Section
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ProneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchAction;

	void PressedJump();

	void PressedCrouch();

	void PressedProne();

	UFUNCTION(Server, Reliable)
	void ServerRPCPressedProne();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPressedProne();

	//몽타주를 가지고 있는다
	//Mesh Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AnimMontage)
	TObjectPtr<class UAnimMontage> ToProne;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AnimMontage)
	TObjectPtr<class UAnimMontage> ToStand;


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
	float MinCameraHeight = 18.0f;

	//AimSection
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AimAction;

	void Aim();
	void StopAiming();

	UPROPERTY()
	TObjectPtr<class UTimelineComponent> ZoomInTimeline;

	UPROPERTY()
	TObjectPtr<class UCurveFloat> AimAlphaCurve;

	FOnTimelineFloat AimInterpFunction{};

	UFUNCTION()
	void TimelineFloatReturn(float Alpha);


protected:
	void PutLifeStone();

protected:

	void PutWeapon();

protected:

	UFUNCTION(BlueprintCallable)
	void SetInventory();

protected:

	uint8 bIsVisibleMap : 1;
	//Map Section
	void SetMap();

protected:
	//Farming Section
	void FarmingItemPressed();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	//누르면 태그 수정... 
	TMap<ECharacterAttackType, TObjectPtr<class UInputAction>> WeaponSwitcherAction;

	void SelectDefaultAttackType();
	void SelectGunAttackType();
	void SelectBombAttackType();

	TObjectPtr<class UInputAction> ShootingMethodAction;

	void ChangeShootingMethod();

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeShootingMethod();
	//GAS Input
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ReloadAction;
protected:

	void GASInputPressed(int32 id);
	void GASInputReleased(int32 id);
	int8 GetInputNumber(int32 id);


protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MenuAction;

	uint8 bShowMenuUI : 1;

	UFUNCTION(BlueprintCallable)
	void ShowMenuUI();

public:

	UFUNCTION(BlueprintCallable)
	void SetShowMenuUI();

};
