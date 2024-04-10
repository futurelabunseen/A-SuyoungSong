// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/QLAT_LineTrace.h"
#include "GA/TA/QLTA_LineTraceResult.h"
#include "GA/TA/QLTA_TraceResult.h"
#include "AbilitySystemComponent.h"


UQLAT_LineTrace::UQLAT_LineTrace()
{
	
}

UQLAT_LineTrace* UQLAT_LineTrace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<AQLTA_LineTraceResult> InTargetActorClass)
{
    UQLAT_LineTrace * NewTask = NewAbilityTask<UQLAT_LineTrace>(OwningAbility);
	NewTask->TargetActorClass = InTargetActorClass;
    return NewTask;
}

void UQLAT_LineTrace::Activate()
{
	Super::Activate();

	SpawnAndInitTargetActor();
	FinishSpawnning();

	SetWaitingOnAvatar();
}

void UQLAT_LineTrace::OnDestroy(bool AbilityEnded)
{
	Super::OnDestroy(AbilityEnded);

	if (TargetActor)
	{
		TargetActor->Destroy(); //타게팅 종료
	}

}
void UQLAT_LineTrace::SpawnAndInitTargetActor()
{
	TargetActor = Cast<AQLTA_LineTraceResult>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));

	if (TargetActor)
	{
		TargetActor->SetSocketPos(GetAvatarActor()->GetActorLocation());
		TargetActor->TargetDataReadyDelegate.AddUObject(this, &UQLAT_LineTrace::OnCompletedCallback);
	}
}

void UQLAT_LineTrace::FinishSpawnning()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();

	if (ASC)
	{
		const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
		TargetActor->FinishSpawning(SpawnTransform);

		ASC->SpawnedTargetActors.Push(TargetActor);
		TargetActor->StartTargeting(Ability); //Ability에 대한 타게팅 시작

		TargetActor->ConfirmTargeting(); //여기서 태스크 없이 타게팅 시작
	}
}


void UQLAT_LineTrace::OnCompletedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	//델리게이트 호출
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCompleted.Broadcast(TargetDataHandle);
	}
	EndTask();
}
