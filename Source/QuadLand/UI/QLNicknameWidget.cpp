// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QLNicknameWidget.h"
#include "Components/TextBlock.h"

void UQLNicknameWidget::SetNickname(FString InNickname)
{
	TxtNickname->SetText(FText::FromString(InNickname));
}
