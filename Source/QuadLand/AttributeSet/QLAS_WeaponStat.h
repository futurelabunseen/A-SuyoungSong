// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "QLAS_WeaponStat.generated.h"

/**
 * 
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class QUADLAND_API UQLAS_WeaponStat : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UQLAS_WeaponStat();

    ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, Damage);
    ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, MaxDamage);
    ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, AttackDistance);
    ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, MaxAttackDistance);
	ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, AmmoCnt);
	ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, MaxAmmoCnt);

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackDistance;
	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackDistance;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoCnt;
	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAmmoCnt;

};