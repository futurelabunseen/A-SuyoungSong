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

	//Ammo 변경될 때 호출할 델리게이트 선언
    ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, AttackDamage);
    ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, MaxDamage);
    ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, AttackDistance);
    ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, MaxAttackDistance);
	ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, CurrentAmmo);
	ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, AmmoCnt);
	ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, MaxAmmoCnt);
	ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, AttackSpeed);
	ATTRIBUTE_ACCESSORS(UQLAS_WeaponStat, MaxAttackSpeed);

	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	virtual void OnAttributeAggregatorCreated(const FGameplayAttribute& Attribute, FAggregator* NewAggregator) const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //리플리케이션 등록

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_Damage, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackDamage;
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_MaxDamage, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxDamage;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AttackDistance, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackDistance;
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_MaxAttackDistance, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackDistance;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_CurrentAmmoCnt, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CurrentAmmo; //현재 총알 개수 

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AmmoCnt, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoCnt; //총을 쏘기 위해서 필요한 총알

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_MaxAmmoCnt, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAmmoCnt; //미리 가지고 있을 수 있는 남아있는 총알

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AttackSpeed, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackSpeed; //총알의 속도

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_MaxAttackSpeed, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackSpeed; //총알의 속도


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

	UFUNCTION()
	virtual void OnRep_CurrentAmmoCnt(const FGameplayAttributeData& OldMaxAmmoCnt);

	UFUNCTION()
	virtual void OnRep_AttackSpeed(const FGameplayAttributeData& OldAmmoCnt);

	UFUNCTION()
	virtual void OnRep_MaxAttackSpeed(const FGameplayAttributeData& OldMaxAmmoCnt);
};