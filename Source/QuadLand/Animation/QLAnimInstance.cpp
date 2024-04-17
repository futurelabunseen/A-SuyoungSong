// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLAnimInstance.h"
#include "GameFramework/Character.h"
#include "Character/QLCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

UQLAnimInstance::UQLAnimInstance()
{
	MovingThreshold = 3.0f; //변경하면서 확인해보자.
	JumpingThreshold = 100.0f;
	AimSpeedRate = 1.7f;
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
		bHasGun = Player->GetHasGun();
	}

	if (Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D(); //Z값을 제외한 X/Y에 대한 크기
		bisIdle = GroundSpeed < MovingThreshold; //x-y에 대한 크기가 3.0보다 작을 시 Idle로 판정
		bIsFalling = Movement->IsFalling();
		bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold);
		bIsCrunching = Player->GetIsCrunching();
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation()); //Locomotion Angle
		bIsAiming = Player->GetIsAiming();
		AimSpeedRate = Player->GetIsRunning()? 1.0f : 1.5f;
		RootYawOffset = Player->GetCurrnetYaw();
		TurningInPlaceType = Player->GetTurningInPlaceType();

	}
}
