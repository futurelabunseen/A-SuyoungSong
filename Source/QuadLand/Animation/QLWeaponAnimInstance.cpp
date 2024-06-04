// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLWeaponAnimInstance.h"
#include "Item/QLWeaponComponent.h"
#include "Character/QLCharacterBase.h"

UQLWeaponAnimInstance::UQLWeaponAnimInstance()
{
}

void UQLWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<AQLCharacterBase>(GetOwningActor());
	bIsReload = false;
	bIsShooting = false;
}

void UQLWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Owner)
	{
		bIsReload = Owner->GetIsReload();
		bIsShooting = Owner->GetIsShooting();
	}
}
