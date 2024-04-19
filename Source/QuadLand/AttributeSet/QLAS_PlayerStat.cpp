// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_PlayerStat.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayTag/GamplayTags.h"

UQLAS_PlayerStat::UQLAS_PlayerStat():MaxHealth(100.0f), Stamina(30.0f),MaxStamina(50.0f)
{
	InitHealth(GetMaxHealth());
}

void UQLAS_PlayerStat::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		//NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}

	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
}

void UQLAS_PlayerStat::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();

	float Minimum = 0.0f;

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamageDone = GetDamage();
		SetDamage(0.0f);

		float NewHealth = FMath::Clamp(GetHealth() - LocalDamageDone, Minimum, GetMaxHealth());
		SetHealth(NewHealth);

		//여기서 맞은 애니메이션을 플레이하고 싶을 때 TargetCharacter에 대해 PlayHitReact를 표현하네, 그러면 Ability 없애도될듯?
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), Minimum, GetMaxStamina()));
	}
	
	if ((GetHealth() <= 0.0f))
	{
		FGameplayTagContainer TargetTag(CHARACTER_STATE_DEAD);
		//Data.Target.TryActivateAbilitiesByTag(TargetTag);

		Data.Target.AddLooseGameplayTag(CHARACTER_STATE_DEAD); //제거는 RemoveLooseGameplayTag
	}
}

void UQLAS_PlayerStat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, Damage, COND_None, REPNOTIFY_Always);
}

void UQLAS_PlayerStat::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_PlayerStat, Health, OldHealth);
}

void UQLAS_PlayerStat::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_PlayerStat, MaxHealth, OldMaxHealth);
}

void UQLAS_PlayerStat::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_PlayerStat, Stamina, OldStamina);
}

void UQLAS_PlayerStat::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_PlayerStat, MaxStamina, OldMaxStamina);
}

void UQLAS_PlayerStat::OnRep_Damage(const FGameplayAttributeData& OldDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UQLAS_PlayerStat, Damage, OldDamage);
}
