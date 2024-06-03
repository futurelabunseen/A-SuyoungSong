// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLBaseAnimInstance.h"
#include "GameFramework/Character.h"
#include "Character/QLCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UQLBaseAnimInstance::UQLBaseAnimInstance()
{
	MovingThreshold = 3.0f; //변경하면서 확인해보자.
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
		GroundSpeed = Velocity.Size2D(); //Z값을 제외한 X/Y에 대한 크기
		bisIdle = GroundSpeed < MovingThreshold; //x-y에 대한 크기가 3.0보다 작을 시 Idle로 판정
		RootYawOffset = Player->GetCurrnetYaw();
		PitchOffset = Player->GetCurrentPitch();
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation()); //Locomotion Angle
	}
}
