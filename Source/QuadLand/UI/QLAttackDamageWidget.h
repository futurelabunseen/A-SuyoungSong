// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QLAttackDamageWidget.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLAttackDamageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetDamage(float Damage);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtAttackDamage;
};

