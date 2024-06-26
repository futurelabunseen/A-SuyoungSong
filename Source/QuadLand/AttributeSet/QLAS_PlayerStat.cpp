// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_PlayerStat.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameplayTag/GamplayTags.h"

#include "QuadLand.h"
UQLAS_PlayerStat::UQLAS_PlayerStat() : Stamina(10.0f)
{
	InitHealth(GetMaxHealth());
	InitStamina(GetMaxStamina());
}

void UQLAS_PlayerStat::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}

	if (Attribute == GetStaminaAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
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

		//���⼭ ���� �ִϸ��̼��� �÷����ϰ� ���� �� TargetCharacter�� ���� PlayHitReact�� ǥ���ϳ�, �׷��� Ability ���ֵ��ɵ�?
	}

	//ó���� ȣ��Ǵ� 0.0������ STOP������ �ٿ��ٸ�, 100���� ���µǸ鼭 �±׸� ��������
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (ASC->HasMatchingGameplayTag(CHARACTER_STATE_STOP))
		{
			ASC->RemoveLooseGameplayTag(CHARACTER_STATE_STOP); //���Ŵ� RemoveLooseGameplayTag
		}
	}

	if ((GetHealth() <= 0.0f))
	{
		TObjectPtr<UAbilitySystemComponent> ASC = GetOwningAbilitySystemComponent();

		if (ASC)
		{
			FGameplayTagContainer TagContainer(CHARACTER_STATE_DEAD);
			ASC->TryActivateAbilitiesByTag(TagContainer);
			
			GetWorld()->GetTimerManager().SetTimerForNextTick
			(
				[ASC]()
				{
					if (ASC)
					{
						FGameplayTagContainer TagContainer(CHARACTER_STATE_DEAD);

						if (ASC->HasAnyMatchingGameplayTags(TagContainer) == false)
						{
							ASC->TryActivateAbilitiesByTag(TagContainer);
						}
					}
				}
			);
		}
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
