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
    //����� �� ȣ���ϴ� ��������Ʈ ����
	UQLAS_PlayerStat();

    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, Health);
    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, MaxHealth);
    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, Stamina);
    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, MaxStamina);
    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, Damage);
    ATTRIBUTE_ACCESSORS(UQLAS_PlayerStat, MetaStamina);

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //���ø����̼� ���

protected:

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health, Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Health;

    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth, Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData MaxHealth;

    UPROPERTY(BlueprintReadOnly, Category = "Stemina", ReplicatedUsing = OnRep_Stamina, Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Stamina;

    UPROPERTY(BlueprintReadOnly, Category = "Stemina", ReplicatedUsing = OnRep_MaxStamina, Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData MaxStamina;

    UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_Damage, Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Damage; //Meta Damage 

    UPROPERTY(BlueprintReadOnly, Category = "Stemina", ReplicatedUsing = OnRep_Stamina, Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData MetaStamina; //Meta Data
protected:

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldHealth); //�� �Լ��� ȣ��Ǿ��� ������ �̹� ����� ���� ����. �׷��� OldHealth�� ���� 
    
    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

    UFUNCTION()
    virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

    UFUNCTION()
    virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

    UFUNCTION()
    virtual void OnRep_Damage(const FGameplayAttributeData& OldHealth); //�� �Լ��� ȣ��Ǿ��� ������ �̹� ����� ���� ����. �׷��� OldHealth�� ���� 

    UFUNCTION()
    virtual void OnRep_MetaStamina(const FGameplayAttributeData& OldHealth); //�� �Լ��� ȣ��Ǿ��� ������ �̹� ����� ���� ����. �׷��� OldHealth�� ���� 

};
