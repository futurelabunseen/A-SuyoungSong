// Fill out your copyright notice in the Description page of Project Settings.

#include "QLGA_TakenDamage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "GameFramework/Character.h"
#include "Player/QLPlayerController.h"
#include "AI/QLAIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/GamplayTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "QuadLand.h"
UQLGA_TakenDamage::UQLGA_TakenDamage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//태그 부착 -> Required Tag : 공격받았을 때 호출
}

void UQLGA_TakenDamage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ACharacter* Character = Cast< ACharacter>(ActorInfo->AvatarActor.Get());

	AQLPlayerController* PC = Character->GetController<AQLPlayerController>();

	if (IsLocallyControlled() && PC)
	{
		PC->BlinkBloodWidget();
		QL_GASLOG(QLNetLog, Log, TEXT("this?"));
	}
	
	
	//만약 NonPlayer라면

	AQLAIController* AIController = Character->GetController<AQLAIController>();

	if (AIController && TriggerEventData->Instigator)
	{
		//여기서 타겟을 지정함
		UBlackboardComponent* BC = AIController->GetBlackboardComponent();
		if (BC)
		{
			BC->SetValueAsObject(TEXT("TargetActor"), Cast<UObject>(TriggerEventData->Instigator));
		}
	
	}
	OnCompletedCallback();
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
