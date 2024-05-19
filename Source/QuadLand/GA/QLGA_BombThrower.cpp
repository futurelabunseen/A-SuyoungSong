// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_BombThrower.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EngineUtils.h"

#include "Character/QLCharacterPlayer.h"
#include "Components/SplineComponent.h"
#include "Item/QLWeaponComponent.h"
#include "GA/AT/QLAT_TrackDrawer.h"
#include "GameplayTag/GamplayTags.h"
#include "GameData/QLDataManager.h"
#include "Character/QLInventoryComponent.h"
#include "GameData/QLWeaponStat.h"
#include "Item/QLBomb.h"
#include "QuadLand.h"


UQLGA_BombThrower::UQLGA_BombThrower():ItemType(EItemType::Bomb)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UQLGA_BombThrower::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());

	if (Player)
	{
		Player->GetBombPath()->SetHiddenInGame(false);
	}
}

void UQLGA_BombThrower::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (IsLocallyControlled())
	{
		TrackDrawer = UQLAT_TrackDrawer::CreateTask(this, DrawerStaticMesh);
		TrackDrawer->ReadyForActivation();
	}
	
	AnimSpeedRate = 1.0f;
	GrapAndThrowMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PunchAnimMontage"), ThrowAnimMontage, AnimSpeedRate, FName("Grap"));
	GrapAndThrowMontage->OnCompleted.AddDynamic(this, &UQLGA_BombThrower::OnCompletedCallback);
	GrapAndThrowMontage->OnInterrupted.AddDynamic(this, &UQLGA_BombThrower::OnInterruptedCallback);
	GrapAndThrowMontage->ReadyForActivation();

}

void UQLGA_BombThrower::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	TrackDrawer = nullptr;
	GrapAndThrowMontage = nullptr;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_BOMB);
	FGameplayTagContainer Tag(CHARACTER_EQUIP_NON);
	if (ASC->HasAnyMatchingGameplayTags(Tag) == false)
	{
		ASC->AddLooseGameplayTags(Tag);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UQLGA_BombThrower::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{	
	MontageJumpToSection(FName("Throw"));
	FGameplayTagContainer TargetTag(CHARACTER_ATTACK_HITCHECK);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->TryActivateAbilitiesByTag(TargetTag);

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	Player->GetInventory()->ServerRPCRemoveItem(ItemType, Player->GetInventoryCnt(ItemType));
	if (Player)
	{
		Player->GetBombPath()->SetHiddenInGame(true);
	}
	ServerRPCAttackHitCheck();
}

void UQLGA_BombThrower::OnCompletedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UQLGA_BombThrower::OnInterruptedCallback()
{
	bool bReplicateEndAbility = true;
	bool bWasCancelled = true;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}


void UQLGA_BombThrower::ServerRPCAttackHitCheck_Implementation()
{
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	Player->GetWeapon()->OnDestoryBomb.ExecuteIfBound();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayTagContainer TargetTag(CHARACTER_ATTACK_HITCHECK);
	ASC->TryActivateAbilitiesByTag(TargetTag);
}
