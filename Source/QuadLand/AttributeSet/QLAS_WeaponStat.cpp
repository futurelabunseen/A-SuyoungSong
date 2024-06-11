// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_WeaponStat.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "GameplayTag/GamplayTags.h"
#include "GameData/QLItemType.h"
#include "Character/QLCharacterPlayer.h"
#include "GameplayEffectAggregatorLibrary.h"

//주먹의 경우 Default
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

	if (Attribute == GetCurrentAmmoAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
		UE_LOG(LogTemp, Log, TEXT("NewValue %lf"), NewValue);
	}
}

bool UQLAS_WeaponStat::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{

	float Minimum = 0.0f;
	float Maximum = 1000.0f;
	if (Data.Target.HasMatchingGameplayTag(CHARACTER_STATE_RELOAD))
	{
		float DeltaAmmo = GetAmmoCnt() - GetCurrentAmmo(); //25 - 17 => 8
		float RemainingCnt = GetMaxAmmoCnt() - DeltaAmmo; // 5 - 8 => 20 변화..

		RemainingCnt = (RemainingCnt >= 0.0f) ? RemainingCnt : 0.0f;
		SetMaxAmmoCnt(FMath::Clamp(RemainingCnt, Minimum, Maximum));
	}
	return true;
}

void UQLAS_WeaponStat::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	//실제 사용 CurrentAmmo = CurrentAmmo -1
	// CurrentAmmo = GetAmmoCnt
	// MaxAmmoCnt = MaxAmmoCnt - GetAmmoCnt
		//현재 태그가 Reload가 아니면 아래는 적용이 안되도록.

	if (Data.EvaluatedData.Attribute == GetCurrentAmmoAttribute())
	{
		if (GetMaxAmmoCnt() <= 0.0f && GetCurrentAmmo() <=0.0f)
		{
			AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>((Data.Target.AbilityActorInfo.Get())->AvatarActor.Get());
			if (Character)
			{
				Character->UpdateAmmoTemp();

				UE_LOG(LogTemp, Log, TEXT("Ammo Update"));
			}
		}
		SetCurrentAmmo(FMath::Clamp(GetCurrentAmmo(), 0.0f, GetAmmoCnt()));
	}

	//Ammo 변경되어서 전달.. MaxAmmo - Ammo 
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
