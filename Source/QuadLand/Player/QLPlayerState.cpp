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
#include "Player/QLPlayerLifeStone.h"
#include "UI/QLUserWidget.h"
AQLPlayerState::AQLPlayerState()
{
    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    ASC->SetIsReplicated(true);
    ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    PlayerStatInfo = CreateDefaultSubobject<UQLAS_PlayerStat>(TEXT("PlayerStat"));
    WeaponStatInfo = CreateDefaultSubobject<UQLAS_WeaponStat>(TEXT("WeaponStat"));
    
    //Event ����Ѵ� -> Equip�� ������
    //Event ����Ѵ� -> Equip���� ��
    NetUpdateFrequency = 30.0f;

    //TagEvent - Delegates
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Dead);
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_WIN, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Win);
    bHasLifeStone = true; 
}

UAbilitySystemComponent* AQLPlayerState::GetAbilitySystemComponent() const
{
    return ASC;
}


void AQLPlayerState::SetAmmoStat(float RemainingAmmoCnt)
{
    if (HasAuthority() && RemainingAmmoCnt !=0.0f)
    {
        //���� �ִ� ���� + RemainigAmmoCnt 
        float MaxAmmo = WeaponStatInfo->GetMaxAmmoCnt() + RemainingAmmoCnt;
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetMaxAmmoCntAttribute(), MaxAmmo); //���� �����ִ� Ammo ������Ʈ
    }
}

void AQLPlayerState::SetWeaponStat(const UQLWeaponStat* Stat)
{
    //UAbilitySystemComponent *TargetASC = WeaponInfo->GetAbilitySystemComponent();
    if (HasAuthority()&& Stat && ASC)
    {
        //���� Base��..
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDamageAttribute(), Stat->Damage);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDistanceAttribute(), Stat->AttackDist);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAmmoCntAttribute(), Stat->AmmoCnt);
    }

}
void AQLPlayerState::BeginPlay()
{
    Super::BeginPlay();


    if (ASC)
    {
        HealthChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(PlayerStatInfo->GetHealthAttribute()).AddUObject(this, &AQLPlayerState::OnChangedHp);
        MaxHealthChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(PlayerStatInfo->GetMaxHealthAttribute()).AddUObject(this, &AQLPlayerState::OnChangedMaxHp);
        AmmoChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(WeaponStatInfo->GetCurrentAmmoAttribute()).AddUObject(this, &AQLPlayerState::OnChangedAmmoCnt);
        AmmoChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(WeaponStatInfo->GetMaxAmmoCntAttribute()).AddUObject(this, &AQLPlayerState::OnChangedMaxAmmoCnt);
    }
}

void AQLPlayerState::OnChangedHp(const FOnAttributeChangeData& Data)
{
    float CurrentHP = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

    if (PC && PC->IsLocalController())
    {
        //Player�� QLPlayerHpBarWidget ������
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedHPPercentage(CurrentHP, GetMaxHealth());
        
        QL_LOG(QLNetLog, Log, TEXT("%lf %lf"), CurrentHP, GetMaxHealth());
    }
}

void AQLPlayerState::OnChangedMaxHp(const FOnAttributeChangeData& Data)
{
    //������ ���ԵǸ� ������ ����(������)
    float MaxHp = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

    if (PC && PC->IsLocalController())
    {
        //Player�� QLPlayerHUDWidget ������ -> �̸������ؾ��� ���̺��δ�;;
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedHPPercentage(GetHealth(), MaxHp);
    }
}

void AQLPlayerState::OnChangedAmmoCnt(const FOnAttributeChangeData& Data)
{
    float CurrentAmmo = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

    if (PC && PC->IsLocalController())
    {
        //Player�� QLPlayerHUDWidget ������ -> �̸������ؾ��� ���̺��δ�;;
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedAmmoCnt(CurrentAmmo);
    }
}

void AQLPlayerState::OnChangedMaxAmmoCnt(const FOnAttributeChangeData& Data)
{
    float MaxAmmo = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

    if (PC && PC->IsLocalController())
    {
        //Player�� QLPlayerHUDWidget ������ -> �̸������ؾ��� ���̺��δ�;;
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedRemainingAmmo(MaxAmmo);
    }
}

void AQLPlayerState::ServerRPCPutLifeStone_Implementation()
{
    QL_LOG(QLNetLog, Log, TEXT("HasLifeStone"));
    if (bHasLifeStone)
    {
        FVector Location = GetPawn()->GetActorLocation(); //Possessed Pawn Position
        FActorSpawnParameters Params;
        Params.Owner = this;
        LifeStone = GetWorld()->SpawnActor<AQLPlayerLifeStone>(Location, FRotator::ZeroRotator, Params);
     
        FRepMovement Movement;
        Movement.Location = LifeStone->GetActorLocation();
        LifeStone->SetReplicatedMovement(Movement);


        bHasLifeStone = false;
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

float AQLPlayerState::GetCurrentAmmoCnt()
{
    return WeaponStatInfo->GetCurrentAmmo();
}

float AQLPlayerState::GetMaxAmmoCnt()
{
    return WeaponStatInfo->GetMaxAmmoCnt();
}

void AQLPlayerState::Win(const FGameplayTag CallbackTag, int32 NewCount)
{
    bIsWin = !bIsWin;
    QL_LOG(QLNetLog, Log, TEXT("Current Win %d"),bIsWin);
}

void AQLPlayerState::Dead(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (bIsDead == false)
    {
        FGameplayTagContainer TargetTag(CHARACTER_STATE_DEAD);
        ASC->TryActivateAbilitiesByTag(TargetTag);
    }
    bIsDead = !bIsDead;
    
    QL_LOG(QLNetLog, Log, TEXT("Current Dead %d"),bIsDead);
}

void AQLPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AQLPlayerState, bIsDead);
    DOREPLIFETIME(AQLPlayerState, bIsWin);
    DOREPLIFETIME(AQLPlayerState, bHasLifeStone);
    DOREPLIFETIME(AQLPlayerState, LifeStone);

}


