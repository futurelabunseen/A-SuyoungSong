// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_BombThrower.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Character/QLCharacterPlayer.h"
#include "GA/AT/QLAT_TrackDrawer.h"
#include "GameplayTag/GamplayTags.h"
#include "GameData/QLDataManager.h"
#include "GameData/QLWeaponStat.h"
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
		UQLWeaponStat* Stat = Cast<UQLWeaponStat>(DataManager->GetItem(ItemType));
		FActorSpawnParameters Params;
		Params.Owner = Character;
		//Bomb - Actor »ý¼º

		Bomb = GetWorld()->SpawnActor<AQLBomb>(BombClass,Params);
		Bomb->SetAttackRange(Stat->AttackDist);
		Bomb->SetAttackSpeed(Stat->MaxSpeed);
		Bomb->SetDamage(Stat->Damage);
		Bomb->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Bomb"));
	}
	TrackDrawer = UQLAT_TrackDrawer::CreateTask(this);
	TrackDrawer->ReadyForActivation();

	
	AnimSpeedRate = 1.0f;
	GrapAndThrowMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PunchAnimMontage"), ThrowAnimMontage, AnimSpeedRate, FName("Grap"));
	GrapAndThrowMontage->OnCompleted.AddDynamic(this, &UQLGA_BombThrower::OnCompletedCallback);
	GrapAndThrowMontage->OnInterrupted.AddDynamic(this, &UQLGA_BombThrower::OnInterruptedCallback);
	GrapAndThrowMontage->ReadyForActivation();

}

void UQLGA_BombThrower::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GrapAndThrowMontage = nullptr;
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_BOMB);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UQLGA_BombThrower::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{	
	MontageJumpToSection(FName("Throw"));
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	Player->ServerRPCRemoveItem(ItemType, Player->GetInventoryCnt(ItemType));
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
	MulticastRPCAttackHitCheck();
}

void UQLGA_BombThrower::MulticastRPCAttackHitCheck_Implementation()
{
	QL_GASLOG(QLNetLog, Warning, TEXT("Multicast RPC"));
	FGameplayTagContainer TargetTag(CHARACTER_ATTACK_HITCHECK);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	ASC->TryActivateAbilitiesByTag(TargetTag);
}
