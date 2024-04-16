// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_WeaponStat.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayEffectAggregatorLibrary.h"

//주먹의 경우 Default
UQLAS_WeaponStat::UQLAS_WeaponStat() : AttackDamage(10.0f), MaxDamage(55.0f), AttackDistance(0.0f), MaxAttackDistance(0.0f), AmmoCnt(0.0f), MaxAmmoCnt(0.0f)
{
	
}

void UQLAS_WeaponStat::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	float Minimum = 0.0f;
	if (Data.EvaluatedData.Attribute == GetAmmoCntAttribute())
	{
		//Ammo 개수가 음수 이면 리셋
		SetAmmoCnt(FMath::Clamp(GetAmmoCnt(), Minimum, GetMaxAmmoCnt()));
	}
}

void UQLAS_WeaponStat::OnAttributeAggregatorCreated(const FGameplayAttribute& Attribute, FAggregator* NewAggregator) const
{
	Super::OnAttributeAggregatorCreated(Attribute, NewAggregator);

	if (!NewAggregator)
	{
		return;
	}

	if (Attribute == GetAttackDistanceAttribute())
	{
		NewAggregator->EvaluationMetaData = &FAggregatorEvaluateMetaDataLibrary::MostNegativeMod_AllPositiveMods; //항상 양수의 경우
	}
}

void UQLAS_WeaponStat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, MaxDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, AttackDistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, MaxAttackDistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, AmmoCnt, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, MaxAmmoCnt, COND_None, REPNOTIFY_Always);

}
void UQLAS_WeaponStat::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, AttackDamage, OldDamage);
}

void UQLAS_WeaponStat::OnRep_MaxDamage(const FGameplayAttributeData& OldMaxDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, MaxDamage, OldMaxDamage);
}

void UQLAS_WeaponStat::OnRep_AttackDistance(const FGameplayAttributeData& OldAttackDistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, AttackDistance, OldAttackDistance);
}

void UQLAS_WeaponStat::OnRep_MaxAttackDistance(const FGameplayAttributeData& OldMaxAttackDistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, MaxAttackDistance, OldMaxAttackDistance);
}

void UQLAS_WeaponStat::OnRep_AmmoCnt(const FGameplayAttributeData& OldAmmoCnt)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, AmmoCnt, OldAmmoCnt);
}

void UQLAS_WeaponStat::OnRep_MaxAmmoCnt(const FGameplayAttributeData& OldMaxAmmoCnt)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, MaxAmmoCnt, OldMaxAmmoCnt);
}
