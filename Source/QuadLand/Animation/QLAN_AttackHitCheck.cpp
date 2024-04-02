// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/QLAN_AttackHitCheck.h"
#include "Interface/AttackHitCheckInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/GamplayTags.h"
#include "Engine/SkeletalMeshSocket.h"

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

	IAttackHitCheckInterface* AttackHitCheckActor = Cast<IAttackHitCheckInterface>(MeshComp->GetOwner());
	UE_LOG(LogTemp, Log, TEXT("Norify is running"));
	if (AttackHitCheckActor)
	{
		//AttackHitCheckActor->AttackHitCheckUsingPunch(); //펀치 충돌 체크 
		
		FName SocketName = *FString::Printf(TEXT("hand_%s"), *CurrentSectionName);

		UE_LOG(LogTemp, Log, TEXT("%s"), *SocketName.ToString());
		const USkeletalMeshSocket* ResultSocket = MeshComp->GetSocketByName(SocketName);
		FGameplayEventData Payload;

		Payload.OptionalObject = ResultSocket;
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), CHARACTER_ATTACK_HITCHECK, Payload);
	}
}
