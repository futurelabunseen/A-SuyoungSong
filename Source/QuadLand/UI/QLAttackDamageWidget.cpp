// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLAttackDamageWidget.h"
#include "Components/TextBlock.h"

void UQLAttackDamageWidget::SetDamage(float Damage)
{
	FString ItemCntTxt = FString::Printf(TEXT("%2lf"), Damage);
	TxtAttackDamage->SetText(FText::FromString(ItemCntTxt));
}
