// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QLAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UQLAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override; //������ ���� �ʱ�ȭ
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; //������ ������Ʈ 
	
	//�� �� �Լ��� �����ͼ� GetOwningActor �Լ��� ����ؼ� ���� � ���¸� ������ �ִ����� �ľ���. 
	//����� ������ �ִ� �׷������� ���, �ִϸ��̼� �ڵ� ��� 
	//���� �� �ִϸ��̼��� �����ϰ� �ִ� ��ü ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class AQLCharacterPlayer> Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;
	//MovementComponent�� ����ؼ� �ش� 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;
	
	//�ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bisIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshold;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsCrunching : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float JumpingThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bIsFalling : 1;

	/* �ѱ� ���� ���� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bHasGun : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	float RootYawOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	float YawChangeOverlap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	float YawLastTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character)
	float Yaw;

};
