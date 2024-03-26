// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/TA/QLTA_SweepTraceResult.h"
#include "Abilities/GameplayAbility.h"
#include "Character/QLCharacterPlayer.h"
#include "Physics/QLCollision.h"
#include "DrawDebugHelpers.h"
AQLTA_SweepTraceResult::AQLTA_SweepTraceResult()
{
}

void AQLTA_SweepTraceResult::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void AQLTA_SweepTraceResult::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		//데이터 생성
		FGameplayAbilityTargetDataHandle TargetHandle = MakeTargetData();
		if (TargetDataReadyDelegate.IsBound())
		{
			TargetDataReadyDelegate.Broadcast(TargetHandle); //해당 값을 전달
		}
	}
}


FGameplayAbilityTargetDataHandle AQLTA_SweepTraceResult::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);

	const float AttackRadius = 15.0f;
	
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SweepTraceResult), false, Character); //식별자 

	FHitResult OutHitResult;

	bool bResult = GetWorld()->SweepSingleByChannel(
		OutHitResult,
		SocketLocation,
		SocketLocation,
		FQuat::Identity,
		CCHANNEL_QLACTION,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);
	
	FGameplayAbilityTargetDataHandle DataHandle;
	if (bResult)
	{
		FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
		DataHandle.Add(TargetData);
	}

#if ENABLE_DRAW_DEBUG
	FColor Color = bResult ? FColor::Green : FColor::Red;
	DrawDebugSphere(GetWorld(), SocketLocation, AttackRadius, 10.0f, Color, false, 5.0f);
#endif
	
	return DataHandle;
}
