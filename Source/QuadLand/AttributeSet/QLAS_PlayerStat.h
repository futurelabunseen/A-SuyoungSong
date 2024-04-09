// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "QLAS_PlayerStat.generated.h"

/**
 * 
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class QUADLAND_API UQLAS_PlayerStat : public UAttributeSet
{
	GENERATED_BODY()

public:
	UQLAS_PlayerStat();

    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, Health);
    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, MaxHealth);
    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, Stemina);
    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, MaxStemina);

    //virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:

    UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Health;
    UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData MaxHealth;

    UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Stemina;
    UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData MaxStemina;

};
