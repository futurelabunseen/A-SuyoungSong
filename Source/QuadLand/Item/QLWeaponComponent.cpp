// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLWeaponComponent.h"
#include "GameFramework/Character.h"
UQLWeaponComponent::UQLWeaponComponent() : bIsReload(false), bIsShooting(false)
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

