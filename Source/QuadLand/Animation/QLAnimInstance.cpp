// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UQLAnimInstance::UQLAnimInstance()
{
	MovingThreshold = 3.0f; //�����ϸ鼭 Ȯ���غ���.
}

void UQLAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());

	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UQLAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D(); //Z���� ������ X/Y�� ���� ũ��
		bisIdle = GroundSpeed < MovingThreshold; //x-y�� ���� ũ�Ⱑ 3.0���� ���� �� Idle�� ����
	}
}
