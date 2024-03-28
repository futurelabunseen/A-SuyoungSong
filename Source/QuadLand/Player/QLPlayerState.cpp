// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerState.h"
#include "AbilitySystemComponent.h"
AQLPlayerState::AQLPlayerState()
{
    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    ASC->SetIsReplicated(true);
}


UAbilitySystemComponent* AQLPlayerState::GetAbilitySystemComponent() const
{
    return ASC;
}
