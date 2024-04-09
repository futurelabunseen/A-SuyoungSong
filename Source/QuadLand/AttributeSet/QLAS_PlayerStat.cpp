// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSet/QLAS_PlayerStat.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

UQLAS_PlayerStat::UQLAS_PlayerStat():MaxHealth(100.0f), Stemina(30.0f),MaxStemina(50.0f)
{
	InitHealth(GetMaxHealth());
}

void UQLAS_PlayerStat::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{

	}
}
