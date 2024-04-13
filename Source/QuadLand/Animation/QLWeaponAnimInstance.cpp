// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLWeaponAnimInstance.h"
#include "Item/QLWeaponComponent.h"

UQLWeaponAnimInstance::UQLWeaponAnimInstance()
{
}

void UQLWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Owner = Cast<UQLWeaponComponent>(GetOwningActor());
	bIsReload = false;
	bIsShooting = false;
}

void UQLWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Owner)
	{
		//Owner�� �پ��ִ� ������ �����´�.
		bIsReload = Owner->GetIsReload();
		bIsShooting = Owner->GetIsShooting();

		UE_LOG(LogTemp, Log, TEXT("????????? %d"), bIsShooting);
	}
}
