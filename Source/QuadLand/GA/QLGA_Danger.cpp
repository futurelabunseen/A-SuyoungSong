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
		PC->OnDeathCheckDelegate.Unbind();
		PC->OnDeathCheckDelegate.BindUObject(this, &UQLGA_Danger::OnCompleted);
		QL_GASLOG(QLLog, Log, TEXT("Danger %s"), *PC->GetName());
		PC->ActivateDeathTimer(Time);
	}
	//플레이어 스테이트도 제거해야할듯! (하지만 나중에 해보자)

}

void UQLGA_Danger::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_Danger::ServerRPCDead_Implementation()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	FGameplayTagContainer TargetTag(CHARACTER_STATE_DEAD);
	
	if (ASC)
	{
		ASC->TryActivateAbilitiesByTag(TargetTag);
	}
}

void UQLGA_Danger::OnCompleted()
{

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	FGameplayTagContainer TagContainer(CHARACTER_STATE_DEAD);
	if (ASC)
	{
		ASC->TryActivateAbilitiesByTag(TagContainer);
	}

	ServerRPCDead();

	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

}
