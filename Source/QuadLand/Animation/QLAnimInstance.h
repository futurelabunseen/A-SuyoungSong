// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/QLBaseAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "GameData/QLTurningInPlaceType.h"
#include "QLAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLAnimInstance : public UQLBaseAnimInstance
{
	GENERATED_BODY()
public:
	UQLAnimInstance();
	
protected:

	virtual void NativeInitializeAnimation() override; //지정된 변수 초기화
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; //변수를 업데이트 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsCrunching : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsProning : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float AimSpeedRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	ETurningPlaceType TurningInPlaceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	uint8 bIsPickup : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	uint8 bIsWin : 1;
};
