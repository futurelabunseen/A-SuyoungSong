// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_Attack.h"
#include "GameplayTag/GamplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Character/QLCharacterPlayer.h"

//UQLGA_Attack::UQLGA_Attack()
//{
//	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
//}
//
//void UQLGA_Attack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
//{
//	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
//
//	if (OnCheckedAttack.IsBound())
//	{
//		// 델리게이트가 바운드되어 있음
//		OnCheckedAttack.Execute();
//	}
//	
//}
//
//void UQLGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
//{
//	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
//
//	AQLCharacterPlayer* Player = CastChecked<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());
//	ECharacterAttackType AttackType= Player->GetCurrentAttackType();
//
//	if (AttackType == ECharacterAttackType::HookAttack)
//	{
//		UE_LOG(LogTemp, Log, TEXT("hook version, ability system is running"));
//		FGameplayEventData Payload;
//		Payload.OptionalObject = this;
//
//		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Player, CHARACTER_EQUIP_NON, Payload);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Log, TEXT("gun version, ability system is running"));
//	}
//
//	//현재 플레이어가 
//	//FGameplayTagContainer TargetTag();
//}
//
//void UQLGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
//{
//	OnCheckedAttack = nullptr;
//	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
//}
//
//
//void UQLGA_Attack::OnCompleted()
//{
//	bool bReplicateEndAbility = true;
//	bool bWasCancelled = false;
//	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
//}
//
//void UQLGA_Attack::OnInterrupted()
//{
//	bool bReplicateEndAbility = true;
//	bool bWasCancelled = true;
//	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
//}