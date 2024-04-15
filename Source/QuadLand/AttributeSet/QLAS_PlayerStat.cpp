// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_PlayerStat.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

UQLAS_PlayerStat::UQLAS_PlayerStat():MaxHealth(100.0f), Stamina(30.0f),MaxStamina(50.0f)
{
	InitHealth(GetMaxHealth());
}

void UQLAS_PlayerStat::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());

		UE_LOG(LogTemp, Log, TEXT("New Value %lf"), NewValue);
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

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		const float LocalDamageDone = Data.EvaluatedData.Magnitude;
		UE_LOG(LogTemp, Log, TEXT("Health %lf"), GetHealth());
		//const float NewHealth = GetHealth() + LocalDamageDone;
		//FMath::Clamp(NewHealth, Minimum, GetMaxHealth())
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		UE_LOG(LogTemp, Log, TEXT("NewHealth %lf"), GetHealth());
		//여기서 맞은 애니메이션을 플레이하고 싶을 때 TargetCharacter에 대해 PlayHitReact를 표현하네, 그러면 Ability 없애도될듯?
	}
	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), Minimum, GetMaxStamina()));
	}
}

void UQLAS_PlayerStat::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UQLAS_PlayerStat, MaxStamina, COND_None, REPNOTIFY_Always);

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
