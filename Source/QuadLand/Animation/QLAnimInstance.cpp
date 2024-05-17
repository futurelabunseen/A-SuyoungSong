// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLAnimInstance.h"
#include "GameFramework/Character.h"
#include "Character/QLCharacterPlayer.h"
#include "GameData/QLTurningInPlaceType.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UQLAnimInstance::UQLAnimInstance()
{
	MovingThreshold = 3.0f; //�����ϸ鼭 Ȯ���غ���.
	JumpingThreshold = 50.0f;
	AimSpeedRate = 1.7f;
	TurningInPlaceType = ETurningPlaceType::ETIP_NotTurning;
}

void UQLAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<ACharacter>(GetOwningActor());
	Player = Cast<AQLCharacterPlayer>(GetOwningActor());
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UQLAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Player)
	{
		bHasGun = Player->bIsUsingGun();
	}

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D(); //Z���� ������ X/Y�� ���� ũ��
		bisIdle = GroundSpeed < MovingThreshold; //x-y�� ���� ũ�Ⱑ 3.0���� ���� �� Idle�� ����
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold);
		bIsCrunching = Player->GetIsCrunching();
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation()); //Locomotion Angle
		bIsAiming = Player->GetIsAiming();
		//AimSpeedRate = Player->GetIsRunning()? 1.0f : 1.5f;
		TurningInPlaceType = Player->GetTurningInPlaceType();
		bIsPickup = Player->GetPickup();
		RootYawOffset = Player->GetCurrnetYaw();
	}
}
