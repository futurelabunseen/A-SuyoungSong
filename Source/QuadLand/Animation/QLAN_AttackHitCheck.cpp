// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLAN_AttackHitCheck.h"
#include "Interface/AttackHitCheckInterface.h"
UQLAN_AttackHitCheck::UQLAN_AttackHitCheck()
{
}

FString UQLAN_AttackHitCheck::GetNotifyName_Implementation() const
{
	return TEXT("AttackHitCheck_Notify");
}

void UQLAN_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp,Animation,EventReference);

	UE_LOG(LogTemp, Warning, TEXT("Notify Check"));

	IAttackHitCheckInterface* AttackHitCheckActor = Cast<IAttackHitCheckInterface>(MeshComp->GetOwner());
	
	if (AttackHitCheckActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Character Actor is CharacterPlayer"));
		AttackHitCheckActor->AttackHitCheckUsingPunch(); //펀치 충돌 체크 
	}
}
