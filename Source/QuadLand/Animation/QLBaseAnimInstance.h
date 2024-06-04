// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QLBaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UQLBaseAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override; //지정된 변수 초기화
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; //변수를 업데이트 


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class AQLCharacterBase> Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	//속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bisIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bHasGun : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	uint8 bIsDead : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State)
	uint8 bIsWin : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	float RootYawOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	float PitchOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float Direction;

	/* Aim Offset*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsAiming : 1;
};
