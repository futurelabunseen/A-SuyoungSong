// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "QLGA_Dead.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGA_Dead : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UQLGA_Dead();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
protected:
	void OnCompleted();

	void RespawnTimeFunc();

	UPROPERTY(EditAnywhere, Category = Spectator)
	TSubclassOf<class AQLSpectatorPawn> SpectatorPawnClass;
};
