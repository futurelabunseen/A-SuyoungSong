// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackHitCheckUsingBomb.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/QLCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Item/QLBomb.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "EngineUtils.h"
#include "Physics/QLCollision.h"
#include "GameplayTag/GamplayTags.h"
#include "AbilitySystemComponent.h"
#include "QuadLand.h"
UQLGA_AttackHitCheckUsingBomb::UQLGA_AttackHitCheckUsingBomb()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UQLGA_AttackHitCheckUsingBomb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	QL_GASLOG(QLNetLog, Warning, TEXT("1"));
	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());

	TArray<AActor*> ChildActors;
	Character->GetAttachedActors(ChildActors,true);

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	for (const auto& AttachedChild : ChildActors)
	{
		Bomb = Cast<AQLBomb>(AttachedChild);

		if (Bomb!=nullptr)
		{
			Bomb->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			FVector OutVelocity = Character->GetCameraForward() * WeaponStat->GetAttackDistance();
			Bomb->OnActorOverlapDelegate.BindUObject(this, &UQLGA_AttackHitCheckUsingBomb::OnCompletedCallback);
			Bomb->ThrowBomb(OutVelocity);
			break;
		}
	}
}

void UQLGA_AttackHitCheckUsingBomb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	Bomb = nullptr;
}

void UQLGA_AttackHitCheckUsingBomb::OnCompletedCallback()
{

	FCollisionQueryParams Params(SCENE_QUERY_STAT(DetectionPlayer), false);
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	TArray<FHitResult> NearbyPlayers;
	//ItemDetectionSocket
	bool bResult = GetWorld()->SweepMultiByChannel(
		NearbyPlayers,
		Bomb->GetActorLocation(),
		Bomb->GetActorLocation(),
		FQuat::Identity,
		CCHANNEL_QLACTION,
		FCollisionShape::MakeSphere(WeaponStat->GetAttackDistance()),
		Params
	);
	
	if (bResult)
	{
		
		for (const auto& Player : NearbyPlayers)
		{
			AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(Player.GetActor());

			if (Character)
			{
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);

				FGameplayEffectContextHandle EffectContextHandle;
				EffectContextHandle.AddHitResult(Player);
				EffectContextHandle.AddOrigin(Bomb->GetActorLocation()); //Origin �߻� ��ġ�� �����Ѵ�.

				if (EffectSpecHandle.IsValid())
				{
					float Damage = Bomb->GetDamage();
					UAbilitySystemComponent* TargetASC = Character->GetAbilitySystemComponent();
					EffectSpecHandle.Data->SetContext(EffectContextHandle);
					TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);

				}
			}
		}
	}

	UAbilitySystemComponent *ASC = GetAbilitySystemComponentFromActorInfo();

	if (ASC)
	{
		FGameplayCueParameters CueParam;
		CueParam.Location = Bomb->GetActorLocation();

		//���� ASC�� �����ͼ� ExecuteGameplayCue ���� 
		ASC->ExecuteGameplayCue(GAMEPLAYCUE_EFFECT_FIREWALL, CueParam);
	}
#if ENABLE_DRAW_DEBUG
	FColor Color = bResult ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), Bomb->GetActorLocation(), WeaponStat->GetAttackDistance(), 10.0f, Color, false, 5.0f);
#endif
	Bomb->SetLifeSpan(1.5f); //������.
	
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}