// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/QLAT_SweepTrace.h"
#include "GA/TA/QLTA_TraceResult.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystemComponent.h"

UQLAT_SweepTrace::UQLAT_SweepTrace()
{
	bSimulatedTask = true; //Simulated ȣ���� ����..
}

UQLAT_SweepTrace* UQLAT_SweepTrace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class AQLTA_TraceResult> TargetActorClass, FVector& SocketPos)
{
	UQLAT_SweepTrace* NewTask = NewAbilityTask<UQLAT_SweepTrace>(OwningAbility);
	NewTask->CurrentSocketPos = SocketPos;
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void UQLAT_SweepTrace::Activate()
{
	Super::Activate();
	
	SpawnAndInitTargetActor(); //���͸� ���� - GetWorld() Deferred
	FinishSpawnning(); //�ʱ�ȭ ����

	SetWaitingOnAvatar(); //���� �Լ�, ������ ������?
	//OnCompletedCallback();
}

void UQLAT_SweepTrace::SpawnAndInitTargetActor()
{
	TargetActor = Cast<AQLTA_TraceResult>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass,FTransform::Identity,nullptr,nullptr,ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	
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
		TargetActor->StartTargeting(Ability); //Ability�� ���� Ÿ���� ����

		TargetActor->ConfirmTargeting(); //���⼭ �½�ũ ���� Ÿ���� ����
	}
}

void UQLAT_SweepTrace::OnDestroy(bool AbilityEnded)
{
	if (TargetActor)
	{
		TargetActor->Destroy(); //Ÿ���� ����
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

void UQLAT_SweepTrace::InitSimulatedTask(UGameplayTasksComponent& InGameplayTasksComponent)
{
	Super::InitSimulatedTask(InGameplayTasksComponent);
	UE_LOG(LogTemp, Log, TEXT("���� �����?"));
}
