// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/QLBaseAnimInstance.h"
#include "QLNonPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLNonPlayerAnimInstance : public UQLBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override; //지정된 변수 초기화
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; //변수를 업데이트 


};
