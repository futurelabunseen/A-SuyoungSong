// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_BombThrower.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "Character/QLCharacterPlayer.h"
#include "GameplayTag/GamplayTags.h"
#include "GameData/QLDataManager.h"
#include "Item/QLBomb.h"
#include "QuadLand.h"

UQLGA_BombThrower::UQLGA_BombThrower():ItemType(EItemType::Bomb), Bomb(nullptr)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_BombThrower::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
}

void UQLGA_BombThrower::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UQLDataManager* DataManager = GetWorld()->GetSubsystem<UQLDataManager>();
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());

	if (DataManager)
	{
		FActorSpawnParameters Params;
		Params.Owner = Character;
		//Bomb - Actor »ý¼º
		Bomb = GetWorld()->SpawnActor<AQLBomb>(BombClass, Params);
		Bomb->SetActorScale3D(FVector(1.5f, 1.5f, 1.5f));
		Bomb->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Bomb"));
	}

	AnimSpeedRate = 1.0f;
	GrapAndThrowMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PunchAnimMontage"), ThrowAnimMontage, AnimSpeedRate, FName("Grap"));
	GrapAndThrowMontage->OnCompleted.AddDynamic(this, &UQLGA_BombThrower::OnCompletedCallback);
	GrapAndThrowMontage->OnInterrupted.AddDynamic(this, &UQLGA_BombThrower::OnInterruptedCallback);
	GrapAndThrowMontage->ReadyForActivation();

}

void UQLGA_BombThrower::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	QL_GASLOG(QLNetLog, Log, TEXT("3"));
	GrapAndThrowMontage = nullptr;
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_BOMB);
}

void UQLGA_BombThrower::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{	
	MontageJumpToSection(FName("Throw"));

	FGameplayTagContainer TargetTag(CHARACTER_ATTACK_HITCHECK);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->TryActivateAbilitiesByTag(TargetTag);
}

void UQLGA_BombThrower::OnCompletedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	if (IsLocallyControlled())
	{
		QL_GASLOG(QLNetLog, Log, TEXT("Current? %d %d"), ItemType, Player->GetInventoryCnt(ItemType));

		Player->ServerRPCRemoveItem(ItemType, Player->GetInventoryCnt(ItemType));
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UQLGA_BombThrower::OnInterruptedCallback()
{
	QL_GASLOG(QLNetLog, Log, TEXT("4"));
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

}
