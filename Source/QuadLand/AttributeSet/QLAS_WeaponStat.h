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
	virtual void OnAttributeAggregatorCreated(const FGameplayAttribute& Attribute, FAggregator* NewAggregator) const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //리플리케이션 등록

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_Damage, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_MaxDamage, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AttackDistance, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackDistance;
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_MaxAttackDistance, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackDistance;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AmmoCnt, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoCnt;
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_MaxAmmoCnt, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAmmoCnt;

protected:

	UFUNCTION()
	virtual void OnRep_Damage(const FGameplayAttributeData& OldDamage); //이 함수가 호출되었을 때에는 이미 변경된 값을 가짐. 그래서 OldHealth를 전달 

	UFUNCTION()
	virtual void OnRep_MaxDamage(const FGameplayAttributeData& OldMaxDamage);

	UFUNCTION()
	virtual void OnRep_AttackDistance(const FGameplayAttributeData& OldAttackDistance);

	UFUNCTION()
	virtual void OnRep_MaxAttackDistance(const FGameplayAttributeData& OldMaxAttackDistance);

	UFUNCTION()
	virtual void OnRep_AmmoCnt(const FGameplayAttributeData& OldAmmoCnt);

	UFUNCTION()
	virtual void OnRep_MaxAmmoCnt(const FGameplayAttributeData& OldMaxAmmoCnt);


};