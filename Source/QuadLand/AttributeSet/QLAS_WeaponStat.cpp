// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_WeaponStat.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayTag/GamplayTags.h"
#include "GameplayEffectAggregatorLibrary.h"

//�ָ��� ��� Default
UQLAS_WeaponStat::UQLAS_WeaponStat()
{
	
}

void UQLAS_WeaponStat::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetAmmoCntAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
		UE_LOG(LogTemp, Log, TEXT("NewValue %lf"), NewValue);
	}
}

void UQLAS_WeaponStat::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	float Minimum = 0.0f;
	float Maximum = 1000.0f;
	//���� ��� CurrentAmmo = CurrentAmmo -1
	// CurrentAmmo = GetAmmoCnt
	// MaxAmmoCnt = MaxAmmoCnt - GetAmmoCnt

	if (Data.EvaluatedData.Attribute == GetCurrentAmmoAttribute())
	{
		float RemainingCnt = GetMaxAmmoCnt() - FMath::Abs( GetCurrentAmmo() - GetAmmoCnt() ); // 25 - abs(20 - 25) => 20 ��ȭ..
		//Ammo ������ ���� �̸� ����
		UE_LOG(LogTemp, Log, TEXT("Current Ammo %lf"), GetCurrentAmmo());
		SetCurrentAmmo(FMath::Clamp(GetCurrentAmmo(), Minimum, Maximum));

		//���� �±װ� Reload�� �ƴϸ� �Ʒ��� ������ �ȵǵ���.
		if (Data.Target.HasMatchingGameplayTag(CHARACTER_STATE_RELOAD))
		{
			UE_LOG(LogTemp, Log, TEXT("Reload %lf"), RemainingCnt);

			SetMaxAmmoCnt(FMath::Clamp(RemainingCnt, Minimum, Maximum));
		}
		//Ammo ����Ǿ ����.. MaxAmmo - Ammo 
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
		NewAggregator->EvaluationMetaData = &FAggregatorEvaluateMetaDataLibrary::MostNegativeMod_AllPositiveMods; //�׻� ����� ���
	}
}

void UQLAS_WeaponStat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, MaxDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, AttackDistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, MaxAttackDistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_WeaponStat, CurrentAmmo, COND_None, REPNOTIFY_Always);
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

void UQLAS_WeaponStat::OnRep_CurrentAmmoCnt(const FGameplayAttributeData& OldAmmoCnt)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, CurrentAmmo, OldAmmoCnt);
}

void UQLAS_WeaponStat::OnRep_AttackSpeed(const FGameplayAttributeData& OldSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, AttackSpeed, OldSpeed);
}

void UQLAS_WeaponStat::OnRep_MaxAttackSpeed(const FGameplayAttributeData& OldMaxSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_WeaponStat, MaxAttackSpeed, OldMaxSpeed);
}
