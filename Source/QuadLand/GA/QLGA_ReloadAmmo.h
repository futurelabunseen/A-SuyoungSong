// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/QLItemData.h"
#include "Abilities/GameplayAbility.h"
#include "QLGA_ReloadAmmo.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGA_ReloadAmmo : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UQLGA_ReloadAmmo();
	/** Input binding stub. */
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;

protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTagContainer StopTag;
	
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<class UGameplayEffect> ReloadAmmoEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TObjectPtr<class UAnimMontage> ReloadAnimMontage;

	UFUNCTION()
	void OnCompletedCallback();

	UFUNCTION()
	void OnInterruptedCallback();

	EItemType ItemType;
};
