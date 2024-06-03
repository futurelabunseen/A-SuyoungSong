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
	virtual void NativeInitializeAnimation() override; //������ ���� �ʱ�ȭ
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; //������ ������Ʈ 


};
