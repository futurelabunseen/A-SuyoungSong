// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/QLWeapon.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameData/QLWeaponStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"

AQLWeapon::AQLWeapon()
{
	Stat = CreateDefaultSubobject<UQLAS_WeaponStat>(TEXT("WeaponStat"));
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
}

UAbilitySystemComponent* AQLWeapon::GetAbilitySystemComponent() const
{
	return ASC;
}

void AQLWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ASC->InitAbilityActorInfo(this, this);
}