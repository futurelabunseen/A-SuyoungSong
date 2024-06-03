// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLNonPlayerAnimInstance.h"
#include "Character/QLCharacterNonPlayer.h"
void UQLNonPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UQLNonPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Movement)
	{
		AQLCharacterNonPlayer* CurrentPlayer = Cast<AQLCharacterNonPlayer>(GetOwningActor());
		if (CurrentPlayer)
		{
			bIsAiming = CurrentPlayer->GetIsAiming();
		}
	}
}
