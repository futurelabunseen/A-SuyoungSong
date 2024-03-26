// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "QLAN_AttackHitCheck.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLAN_AttackHitCheck : public UAnimNotify
{
	GENERATED_BODY()
	
public:

	UQLAN_AttackHitCheck();
	
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:

	UPROPERTY(EditAnywhere)
	FString CurrentSectionName;
};
