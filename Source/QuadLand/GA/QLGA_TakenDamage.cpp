// Fill out your copyright notice in the Description page of Project Settings.

#include "QLGA_TakenDamage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/GamplayTags.h"

UQLGA_TakenDamage::UQLGA_TakenDamage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//태그 부착 -> Required Tag : 공격받았을 때 호출
}

void UQLGA_TakenDamage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Log, TEXT("Current Class TakenDamage Class"));

	//데미지가 0이면 죽는다. - 원천자는 승리한다.
	
	//UAbilitySystemComponent* Source = GetAbilitySystemComponentFromActorInfo_Checked(); 
	//UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
	//if (Source)
	//{
	//	const UQLAS_PlayerStat* PlayerStat = Source->GetSet<UQLAS_PlayerStat>();
	//	FGameplayEventData Payload;

	//	if (PlayerStat && PlayerStat->GetHealth() <= 0.0f)
	//	{
	//		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ActorInfo->AvatarActor.Get(), CHARACTER_STATE_DEAD, Payload);
	//		
	//		OnCompletedCallback();
	//		return;
	//	}
	//}
	
	float AnimSpeedRate = 1.0f;
	float CurrentTypeIdx = TriggerEventData->EventMagnitude;
	UAbilityTask_PlayMontageAndWait* AttackUsingPunchMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("TakenDamageActor"), DamageMontage[CurrentTypeIdx], AnimSpeedRate);
	AttackUsingPunchMontage->OnCompleted.AddDynamic(this, &UQLGA_TakenDamage::OnCompletedCallback);
	AttackUsingPunchMontage->OnInterrupted.AddDynamic(this, &UQLGA_TakenDamage::OnInterruptedCallback);
	AttackUsingPunchMontage->ReadyForActivation();

}

void UQLGA_TakenDamage::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_TakenDamage::OnCompletedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_TakenDamage::OnInterruptedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}
