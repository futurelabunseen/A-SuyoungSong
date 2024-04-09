// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QLWeaponAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UQLWeaponAnimInstance();
protected:
	//virtual void NativeInitializeAnimation() override; //지정된 변수 초기화
	//virtual void NativeUpdateAnimation(float DeltaSeconds) override; //변수를 업데이트 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsShooting : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;
};
