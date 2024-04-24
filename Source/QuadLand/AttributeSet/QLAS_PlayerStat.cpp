// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_PlayerStat.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayTag/GamplayTags.h"

#include "QuadLand.h"
UQLAS_PlayerStat::UQLAS_PlayerStat()
{
	InitHealth(GetMaxHealth());
	InitStamina(GetMaxStamina());
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
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;

		//UE_LOG(LogTemp, Log, TEXT("Current Stamina %lf"), GetStamina());
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (GetStamina() <= 0.0f)
		{
			if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_STOP) == false)
			{
				UE_LOG(LogTemp, Log, TEXT("Current Stamina %lf"), GetStamina());
				//FGameplayTagContainer TargetTag(CHARACTER_STATE_STOP);
				ASC->AddLooseGameplayTag(CHARACTER_STATE_STOP);
			}
		}
		else
		{
			if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_STOP))
			{
				ASC->RemoveLooseGameplayTag(CHARACTER_STATE_STOP); //제거는 RemoveLooseGameplayTag
			}
		}

	}
	UE_LOG(QLNetLog, Log, TEXT("Current Stamina %lf\n"), GetStamina());
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

	//처음에 호출되는 0.0때문에 STOP딱지가 붙였다면, 100으로 리셋되면서 태그를 제거해줌
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_STOP))
		{
			ASC->RemoveLooseGameplayTag(CHARACTER_STATE_STOP); //제거는 RemoveLooseGameplayTag
		}
	}

	if ((GetHealth() <= 0.0f))
	{
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
