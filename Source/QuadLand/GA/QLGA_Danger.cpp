// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Danger.h"
#include "QuadLand.h"
#include "GameFramework/Character.h"
#include "Player/QLPlayerController.h"
#include "GameplayTag/GamplayTags.h"
#include "AbilitySystemComponent.h"

UQLGA_Danger::UQLGA_Danger() : Time(10.0f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_Danger::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	QL_GASLOG(QLLog, Log, TEXT("begin"));
	
	ACharacter* Character = Cast<ACharacter>(GetActorInfo().AvatarActor.Get());

	AQLPlayerController* PC = Cast<AQLPlayerController>(Character->GetController());

	if (PC)
	{
		PC->OnDeathCheckDelegate.BindUObject(this, &UQLGA_Danger::OnCompleted);
		PC->ActivateDeathTimer(Time);
	}
	//�÷��̾� ������Ʈ�� �����ؾ��ҵ�! (������ ���߿� �غ���)

}

void UQLGA_Danger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_Danger::OnCompleted()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	if (ASC)
	{
		ASC->AddLooseGameplayTag(CHARACTER_STATE_DEAD);
	}

	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

}
