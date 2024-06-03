// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLBaseAnimInstance.h"
#include "GameFramework/Character.h"
#include "Character/QLCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UQLBaseAnimInstance::UQLBaseAnimInstance()
{
	MovingThreshold = 3.0f; //�����ϸ鼭 Ȯ���غ���.
}

void UQLBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	Player = Cast<AQLCharacterBase>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}

}

void UQLBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Player)
	{
		bHasGun = Player->bIsUsingGun();
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D(); //Z���� ������ X/Y�� ���� ũ��
		bisIdle = GroundSpeed < MovingThreshold; //x-y�� ���� ũ�Ⱑ 3.0���� ���� �� Idle�� ����
		RootYawOffset = Player->GetCurrnetYaw();
		PitchOffset = Player->GetCurrentPitch();
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation()); //Locomotion Angle
	}
}
