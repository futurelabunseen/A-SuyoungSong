// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackHitCheckUsingBomb.h"
#include "GameFramework/Character.h"
#include "Item/QLBomb.h"
#include "EngineUtils.h"
#include "QuadLand.h"
UQLGA_AttackHitCheckUsingBomb::UQLGA_AttackHitCheckUsingBomb()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_AttackHitCheckUsingBomb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());

	TArray<AActor*> tempChildActors;
	Character->GetAttachedActors(tempChildActors,true);

	for (const auto& AttachedChild : tempChildActors)
	{
		AQLBomb* Bomb = Cast<AQLBomb>(AttachedChild);

		if (Bomb)
		{
			QL_GASLOG(QLNetLog, Log, TEXT("Bomb is valid"));
			Bombs.Add(Bomb);

			Bomb->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		}
		
	}

	OnCompletedCallback();
}

void UQLGA_AttackHitCheckUsingBomb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}
void UQLGA_AttackHitCheckUsingBomb::OnCompletedCallback()
{
	QL_GASLOG(QLNetLog, Log, TEXT("Current Bomb Section"));

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}