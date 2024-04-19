// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerState.h"
#include "GameData/QLWeaponStat.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "QuadLand.h"
#include "Net/UnrealNetwork.h"
AQLPlayerState::AQLPlayerState()
{
    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    ASC->SetIsReplicated(true);
    ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    PlayerStatInfo = CreateDefaultSubobject<UQLAS_PlayerStat>(TEXT("PlayerStat"));
    WeaponStatInfo = CreateDefaultSubobject<UQLAS_WeaponStat>(TEXT("WeaponStat"));
    
    //Event 등록한다 -> Equip을 가질때
    //Event 등록한다 -> Equip없을 때
    NetUpdateFrequency = 30.0f;

    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Dead);
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_WIN, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Win);
}


UAbilitySystemComponent* AQLPlayerState::GetAbilitySystemComponent() const
{
    return ASC;
}

void AQLPlayerState::SetWeaponStat(class UQLWeaponStat* Stat)
{
    //UAbilitySystemComponent *TargetASC = WeaponInfo->GetAbilitySystemComponent();
    if (HasAuthority()&& Stat && ASC)
    {
        //원래 Base값..
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetMaxAmmoCntAttribute(), Stat->AmmoCnt);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAmmoCntAttribute(), Stat->AmmoCnt);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDamageAttribute(), Stat->Damage);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDistanceAttribute(), Stat->AttackDist);
    }

}

void AQLPlayerState::Win(const FGameplayTag CallbackTag, int32 NewCount)
{
    bIsWin = !bIsWin;
    QL_LOG(QLNetLog, Log, TEXT("Current Win %d"),bIsWin);
}

void AQLPlayerState::Dead(const FGameplayTag CallbackTag, int32 NewCount)
{
    bIsDead = !bIsDead;
    QL_LOG(QLNetLog, Log, TEXT("Current Dead %d"),bIsDead);
}

void AQLPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AQLPlayerState, bIsDead);
    DOREPLIFETIME(AQLPlayerState, bIsWin);
}




