// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLWeaponComponent.h"
#include "GameFramework/Character.h"
#include "GameData/QLWeaponStat.h"
UQLWeaponComponent::UQLWeaponComponent()
{
	//GetOwner()
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));

	//AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceRef(TEXT("/Game/QuadLand/Animations/Blueprint/ABQL_WeaponTypeA.ABQL_WeaponTypeA_C"));

	if (AnimInstanceRef.Class)
	{
		Weapon->SetAnimClass(AnimInstanceRef.Class);
	}
}

const UQLWeaponStat* UQLWeaponComponent::GetStat(EWeaponType Type)
{
	if(Weapons.Contains(Type))
		return Weapons[Type];
	return nullptr;
}

