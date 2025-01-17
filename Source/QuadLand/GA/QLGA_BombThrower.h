// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/QLItemType.h"
#include "Abilities/GameplayAbility.h"
#include "QLGA_BombThrower.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLGA_BombThrower : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UQLGA_BombThrower();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	/** Destroys instanced-per-execution abilities. Instance-per-actor abilities should 'reset'. Any active ability state tasks receive the 'OnAbilityStateInterrupted' event. Non instance abilities - what can we do? */
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility);


protected:

	uint8 bGrapBomb : 1;
	uint8 bThrowBomb : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TObjectPtr<class UAnimMontage> ThrowAnimMontage;

	UFUNCTION()
	void OnCompletedCallback();
	
	UFUNCTION()
	void OnInterruptedCallback();

	EItemType ItemType;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> GrapAndThrowMontage;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_WaitInputRelease> TrackDrawer;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMesh> DrawerStaticMesh;

	float AnimSpeedRate;

};
