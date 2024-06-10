// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLAnimInstance.h"
#include "GameFramework/Character.h"
#include "Character/QLCharacterPlayer.h"
#include "GameData/QLTurningInPlaceType.h"
#include "GameFramework/CharacterMovementComponent.h"

UQLAnimInstance::UQLAnimInstance()
{
	JumpingThreshold = 50.0f;
	AimSpeedRate = 1.7f;
	TurningInPlaceType = ETurningPlaceType::ETIP_NotTurning;
}

void UQLAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UQLAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		AQLCharacterPlayer *CurrentPlayer = Cast<AQLCharacterPlayer>(GetOwningActor());
		if (CurrentPlayer)
		{
			bIsFalling = Movement->IsFalling();
			bIsJumping = bIsFalling & (Velocity.Z > JumpingThreshold);
			bIsAiming = CurrentPlayer->GetIsAiming();
			TurningInPlaceType = CurrentPlayer->GetTurningInPlaceType();
			bIsPickup = CurrentPlayer->GetPickup();
			bIsProning = CurrentPlayer->GetIsProning();
		}
	}
}
