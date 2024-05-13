// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackHitCheckUsingBomb.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Character/QLCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Item/QLBomb.h"
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

	for (const auto& AttachedChild : ChildActors)
	{
		Bomb = Cast<AQLBomb>(AttachedChild);

		if (Bomb)
		{
			Bomb->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

			FVector OutVelocity = Character->GetCameraForward() * Bomb->GetAttackSpeed();
			Bomb->OnActorOverlapDelegate.BindUObject(this, &UQLGA_AttackHitCheckUsingBomb::OnCompletedCallback);
			Bomb->ThrowBomb(OutVelocity);
			break;
		}
	}
}

void UQLGA_AttackHitCheckUsingBomb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_AttackHitCheckUsingBomb::OnCompletedCallback()
{

	FCollisionQueryParams Params(SCENE_QUERY_STAT(DetectionPlayer), false);

	TArray<FHitResult> NearbyPlayers;
	//ItemDetectionSocket
	bool bResult = GetWorld()->SweepMultiByChannel(
		NearbyPlayers,
		Bomb->GetActorLocation(),
		Bomb->GetActorLocation(),
		FQuat::Identity,
		CCHANNEL_QLACTION,
		FCollisionShape::MakeSphere(Bomb->GetAttackRange()),
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

				if (EffectSpecHandle.IsValid())
				{
					float Damage = Bomb->GetDamage();
					UAbilitySystemComponent* TargetASC = Character->GetAbilitySystemComponent();
					EffectSpecHandle.Data->SetSetByCallerMagnitude(DATA_STAT_DAMAGE, Damage);

					QL_GASLOG(QLNetLog, Log, TEXT("Damage %lf"),Damage);

					TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
				}
			}
		}
	}

#if ENABLE_DRAW_DEBUG
	FColor Color = bResult ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), Bomb->GetActorLocation(), Bomb->GetAttackRange(), 10.0f, Color, false, 5.0f);
#endif
	Bomb->SetLifeSpan(3.0f); //¾ø¾îÁü.

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}