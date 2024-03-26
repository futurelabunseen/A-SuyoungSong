// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/QLAT_SweepTrace.h"
#include "GA/TA/QLTA_SweepTraceResult.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystemComponent.h"

UQLAT_SweepTrace::UQLAT_SweepTrace()
{
}

UQLAT_SweepTrace* UQLAT_SweepTrace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class AQLTA_SweepTraceResult> TargetActorClass, FVector& SocketPos)
{
	UQLAT_SweepTrace* NewTask = NewAbilityTask<UQLAT_SweepTrace>(OwningAbility);
	NewTask->CurrentSocketPos = SocketPos;
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void UQLAT_SweepTrace::Activate()
{
	Super::Activate();
	UE_LOG(LogTemp, Warning, TEXT("UQLAT_SweepTrace class is running"));
	
	SpawnAndInitTargetActor(); //액터만 생성 - GetWorld() Deferred
	FinishSpawnning(); //초기화 생성

	SetWaitingOnAvatar(); //지연 함수, 지연을 왜하지?
	//OnCompletedCallback();
}

void UQLAT_SweepTrace::SpawnAndInitTargetActor()
{
	TargetActor = Cast<AQLTA_SweepTraceResult>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass,FTransform::Identity,nullptr,nullptr,ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	
	if (TargetActor)
	{
		TargetActor->SetSocketPos(CurrentSocketPos);
		TargetActor->TargetDataReadyDelegate.AddUObject(this,&UQLAT_SweepTrace::OnCompletedCallback);

	}
}

void UQLAT_SweepTrace::FinishSpawnning()
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

void UQLAT_SweepTrace::OnDestroy(bool AbilityEnded)
{
	if (TargetActor)
	{
		TargetActor->Destroy(); //타게팅 종료
	}

	Super::OnDestroy(AbilityEnded);
}

void UQLAT_SweepTrace::OnCompletedCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCompleted.Broadcast(TargetDataHandle);
	}

	EndTask();
}
