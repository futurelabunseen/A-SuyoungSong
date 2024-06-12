// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "QLGA_AttackHitCheckUsingBomb.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGA_AttackHitCheckUsingBomb : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UQLGA_AttackHitCheckUsingBomb();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnCompletedCallback();
protected:


	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> AttackDamageEffect;

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bomb)
	TWeakObjectPtr<class AQLBomb> Bomb;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Bomb)
	TSubclassOf<class AQLFirewall> Fire;

	void SpawnFire();


	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCShowGameplayCue();
};
