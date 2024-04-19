// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerState.h"
#include "GameData/QLWeaponStat.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "Player/QLPlayerController.h"

#include "AbilitySystemComponent.h"
#include "GameplayTag/GamplayTags.h"
#include "QuadLand.h"
#include "Net/UnrealNetwork.h"
#include "UI/QLUserWidget.h"

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

    //TagEvent - Delegates
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Dead);
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_WIN, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Win);
    //HUD - Delegates
    HealthChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(PlayerStatInfo->GetHealthAttribute()).AddUObject(this, &AQLPlayerState::UpdateHp);
    MaxHealthChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(PlayerStatInfo->GetMaxHealthAttribute()).AddUObject(this, &AQLPlayerState::UpdateMaxHp);
    AmmoChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(WeaponStatInfo->GetAmmoCntAttribute()).AddUObject(this, &AQLPlayerState::UpdateAmmoCnt);

}

UAbilitySystemComponent* AQLPlayerState::GetAbilitySystemComponent() const
{
    return ASC;
}

void AQLPlayerState::SetWeaponStat(const UQLWeaponStat* Stat)
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
void AQLPlayerState::BeginPlay()
{
    Super::BeginPlay();
}

void AQLPlayerState::UpdateHp(const FOnAttributeChangeData& Data)
{
    float CurrentHP = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC)
    {
        //Player의 QLPlayerHpBarWidget 가져옴
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->UpdateHPPercentage(CurrentHP, GetMaxHealth());
    }
}

void AQLPlayerState::UpdateMaxHp(const FOnAttributeChangeData& Data)
{
    //아이템 도입되면 시작할 예정(다음주)
    float MaxHp = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC)
    {
        //Player의 QLPlayerHUDWidget 가져옴 -> 이름변경해야할 각이보인다;;
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->UpdateHPPercentage(GetHealth(), MaxHp);
    }
}

void AQLPlayerState::UpdateAmmoCnt(const FOnAttributeChangeData& Data)
{
    float CurrentAmmo = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC)
    {
        //Player의 QLPlayerHUDWidget 가져옴 -> 이름변경해야할 각이보인다;;
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->UpdateAmmo(CurrentAmmo);
    }

}


float AQLPlayerState::GetHealth()
{
    return PlayerStatInfo->GetHealth();
}

float AQLPlayerState::GetMaxHealth()
{
    return PlayerStatInfo->GetMaxHealth();
}

float AQLPlayerState::GetAmmoCnt()
{
    return WeaponStatInfo->GetAmmoCnt();
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


