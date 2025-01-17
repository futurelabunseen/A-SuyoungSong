// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_BombThrower.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EngineUtils.h"
#include "AttributeSet/QLAS_WeaponStat.h"
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
	bGrapBomb = false;
	bThrowBomb = false;
}

bool UQLGA_BombThrower::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	//캐릭터의 인벤토리에서 폭탄이 없으면 false
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	if (Player->GetInventoryCnt(ItemType) <= 0)
	{
		return false;
	}
	
	// Add additional conditions here
	return true;
}

void UQLGA_BombThrower::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if (bGrapBomb == false)
	{
		AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());

		if (Player && IsLocallyControlled())
		{
			USplineComponent* BombPath = Player->GetBombPath();
			if (BombPath)
			{
				BombPath->SetHiddenInGame(false);
			}
		}

		bGrapBomb = true;
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
	
	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
	if (Player)
	{
		Player->ThrowBomb = true;
		Player->StopAim();
	}

	if (Player->IsMontagePlaying(ThrowAnimMontage) == false)
	{
		AnimSpeedRate = 1.0f;
		GrapAndThrowMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ThrowAnimMontage"), ThrowAnimMontage, AnimSpeedRate, FName("Grap"));
		GrapAndThrowMontage->OnCompleted.AddDynamic(this, &UQLGA_BombThrower::OnCompletedCallback);
		GrapAndThrowMontage->OnInterrupted.AddDynamic(this, &UQLGA_BombThrower::OnInterruptedCallback);
		GrapAndThrowMontage->ReadyForActivation();
	}
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

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());

	UQLWeaponComponent* WeaponComp = Player->GetWeapon();
	if (WeaponComp)
	{
		WeaponComp->ResetBomb();
	}

	QL_GASLOG(QLLog, Log, TEXT("Grap And Throw Reset"));
	
	bGrapBomb = false;
	bThrowBomb = false;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UQLGA_BombThrower::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{	
	if (bThrowBomb == false)
	{
		MontageJumpToSection(FName("Throw"));

		AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());
		Player->GetInventory()->ServerRPCRemoveItem(ItemType, Player->GetInventoryCnt(ItemType));

		if (Player)
		{
			USplineComponent* BombPath = Player->GetBombPath();
			if (BombPath)
			{
				BombPath->SetHiddenInGame(true);
			}
		}

		bThrowBomb = true;
	}
}

void UQLGA_BombThrower::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	TrackDrawer = nullptr;
	GrapAndThrowMontage = nullptr;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();


	if (ASC->HasMatchingGameplayTag(CHARACTER_EQUIP_BOMB))
	{
		ASC->RemoveLooseGameplayTag(CHARACTER_EQUIP_BOMB);
	}

	FGameplayTagContainer Tag(CHARACTER_EQUIP_NON);
	if (ASC->HasMatchingGameplayTag(CHARACTER_EQUIP_NON) == false)
	{
		ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON);
	}

	AQLCharacterPlayer* Player = Cast<AQLCharacterPlayer>(GetActorInfo().AvatarActor.Get());

	UQLWeaponComponent* WeaponComp = Player->GetWeapon();
	if (WeaponComp)
	{
		WeaponComp->ResetBomb();
	}

	bGrapBomb = false;
	bThrowBomb = false;

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

