// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_WeaponStat.h"
#include "GameplayEffectExtension.h"

//�ָ��� ��� Default
UQLAS_WeaponStat::UQLAS_WeaponStat() : Damage(10.0f), MaxDamage(55.0f), AttackDistance(0.0f), MaxAttackDistance(0.0f), AmmoCnt(0.0f), MaxAmmoCnt(0.0f)
{
	
}

void UQLAS_WeaponStat::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	float Minimum = 0.0f;
	if (Data.EvaluatedData.Attribute == GetAmmoCntAttribute())
	{
		//Ammo ������ ���� �̸� ����
		SetAmmoCnt(FMath::Clamp(GetAmmoCnt(), Minimum, GetMaxAmmoCnt()));
	}
}
