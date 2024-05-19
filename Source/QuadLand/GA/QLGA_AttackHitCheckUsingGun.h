// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameData/WeaponType.h"
#include "QLGA_AttackHitCheckUsingGun.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGA_AttackHitCheckUsingGun : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UQLGA_AttackHitCheckUsingGun();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UFUNCTION()
	void OnCompletedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> AttackDamageEffect;

	UPROPERTY(EditAnywhere, Category = "AttackDist")
	float HeadDistThreshold; //루트를 적용하면 계산값이 증가하기 때문에, 제곱근을 사용해서 계산을 줄임. 

};
