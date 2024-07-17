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
#include "Physics/QLCollision.h"
#include "Gimmick/QLLifestoneStorageBox.h"
#include "Character/QLCharacterBase.h"
#include "Character/QLCharacterPlayer.h"
#include "HUD/QLHUD.h"
#include "Player/QLLobbyPlayerState.h"
#include "Game/QLGameInstance.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameData/QLDataManager.h"

AQLPlayerState::AQLPlayerState()
{
    ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
    ASC->SetIsReplicated(true);
    ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    PlayerStatInfo = CreateDefaultSubobject<UQLAS_PlayerStat>(TEXT("PlayerStat"));
    WeaponStatInfo = CreateDefaultSubobject<UQLAS_WeaponStat>(TEXT("WeaponStat"));
   
    NetUpdateFrequency = 30.0f;

    //TagEvent - Delegates
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Dead);
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_WIN, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Win);
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_NOTRUN, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::CoolTimeStamina);

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

void AQLPlayerState::BulletWaste(float DiscardedCount)
{
    if (HasAuthority())
    {
        //���� �ִ� ���� + RemainigAmmoCnt 
        float MaxAmmo = WeaponStatInfo->GetMaxAmmoCnt() - DiscardedCount;
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetMaxAmmoCntAttribute(), MaxAmmo); //���� �����ִ� Ammo ������Ʈ
    }
}

void AQLPlayerState::SetWeaponStat(const UQLWeaponStat* Stat)
{
    if (HasAuthority()&& Stat && ASC)
    {
        //���� Base��..
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDamageAttribute(), Stat->Damage);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetMaxDamageAttribute(), Stat->Damage);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDistanceAttribute(), Stat->AttackDist);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackSpeedAttribute(), Stat->AttackSpeed);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAmmoCntAttribute(), Stat->AmmoCnt);
    }//0->

}
void AQLPlayerState::AddHPStat(float HP)
{
    if (HasAuthority() && ASC)
    {
        float NewHp = FMath::Clamp(GetHealth() + HP, 0.0f, GetMaxHealth());
        //���� Base��..
        ASC->SetNumericAttributeBase(UQLAS_PlayerStat::GetHealthAttribute(), NewHp);
    }

}
void AQLPlayerState::AddStaminaStat(float Stamina)
{
    if (CoolTimer.IsValid())
    {
        //��Ÿ�� ���
        FGameplayTagContainer Tag(CHARACTER_STATE_NOTRUN);
        ASC->RemoveLooseGameplayTags(Tag);
        GetWorld()->GetTimerManager().ClearTimer(CoolTimer);
    }

    if (HasAuthority() && ASC)
    {
        //���� Base��.. -> �ϴ� �̷��� �ΰ� �����ϴ� �������� �� ���� 
        FGameplayTagContainer Tag(STAT_STAMINA_STATUS_REDUCE);
        ASC->RemoveActiveEffectsWithTags(Tag);

    }
}
void AQLPlayerState::ResetWeaponStat(const UQLWeaponStat* Stat)
{
    if (HasAuthority() && Stat && ASC)
    {
        //���� Base��..
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDamageAttribute(), 10.0f); //Default Attack - ��ø�?�ϵ��ڵ�
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDistanceAttribute(), WeaponStatInfo->GetAttackDistance() - Stat->AttackDist);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAmmoCntAttribute(), WeaponStatInfo->GetAmmoCnt() - Stat->AmmoCnt); //���� �������ִ� �Ѿ��� ������ ���� (���?���� ����, ���� ����!!!!)
    }
}

void AQLPlayerState::UseGlassesItem(float Time) //��ģ���� Ŭ���̾�Ʈ���� ���Ǿ����?
{
    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����
    PC->ClientRPCShowLifestoneWidget(Time);
}

void AQLPlayerState::BeginPlay()
{
    Super::BeginPlay();

    if (ASC)
    {
        HealthChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(PlayerStatInfo->GetHealthAttribute()).AddUObject(this, &AQLPlayerState::OnChangedHp);
        MaxHealthChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(PlayerStatInfo->GetMaxHealthAttribute()).AddUObject(this, &AQLPlayerState::OnChangedMaxHp);
        AmmoChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(WeaponStatInfo->GetCurrentAmmoAttribute()).AddUObject(this, &AQLPlayerState::OnChangedAmmoCnt);
        MaxAmmoChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(WeaponStatInfo->GetMaxAmmoCntAttribute()).AddUObject(this, &AQLPlayerState::OnChangedMaxAmmoCnt);
        StaminaChangedDeleagteHandle = ASC->GetGameplayAttributeValueChangeDelegate(PlayerStatInfo->GetStaminaAttribute()).AddUObject(this, &AQLPlayerState::OnChangedStamina);
        MaxStaminaChangedDeleagteHandle= ASC->GetGameplayAttributeValueChangeDelegate(PlayerStatInfo->GetMaxStaminaAttribute()).AddUObject(this, &AQLPlayerState::OnChangedMaxStamina);
    }

    UQLGameInstance* GameInstance = Cast<UQLGameInstance>(GetWorld()->GetGameInstance());

    if (GameInstance)
    {
        ServerRPCInitType(GameInstance->GetGenderType(), GameInstance->GetGemMatType());
    }
}

void AQLPlayerState::ClientRPCInitLifeStone_Implementation(int InGemType)
{
    UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

    if (DataManager)
    {
        LifeStoneClass = DataManager->GetLifeStoneClass(InGemType);
    }
}


void AQLPlayerState::OnChangedStamina(const FOnAttributeChangeData& Data)
{
    float CurrentStamina = Data.NewValue;

    if (UpdateStaminaDelegate.IsBound())
    {
        UpdateStaminaDelegate.Execute(CurrentStamina, GetMaxStamina());
    }
}

void AQLPlayerState::OnChangedMaxStamina(const FOnAttributeChangeData& Data)
{
    float CurrentMaxStamina = Data.NewValue;
    
    if (UpdateStaminaDelegate.IsBound())
    {
        UpdateStaminaDelegate.Execute(GetStamina(), CurrentMaxStamina);
    }
}

void AQLPlayerState::OnChangedHp(const FOnAttributeChangeData& Data)
{

    float CurrentHP = Data.NewValue;

    if (UpdateHPDelegate.IsBound())
    {
        UpdateHPDelegate.Execute(CurrentHP, GetMaxHealth());
    }
}

void AQLPlayerState::OnChangedMaxHp(const FOnAttributeChangeData& Data)
{
    //������ ���ԵǸ� ������ ����(������)
    float MaxHp = Data.NewValue;

    if (UpdateHPDelegate.IsBound())
    {
        UpdateHPDelegate.Execute(GetHealth(), MaxHp);
    }
}

void AQLPlayerState::OnChangedAmmoCnt(const FOnAttributeChangeData& Data)
{
    float CurrentAmmo = Data.NewValue;
    if (UpdateAmmoDelegate.IsBound())
    {
        UpdateAmmoDelegate.Execute(CurrentAmmo);
    }
}

void AQLPlayerState::OnChangedMaxAmmoCnt(const FOnAttributeChangeData& Data)
{
    float MaxAmmo = Data.NewValue;

    //Player�� QLPlayerHUDWidget ������ -> �̸������ؾ��� ���̺��δ�;;
    if (UpdateRemainingDelegate.IsBound())
    {
        UpdateRemainingDelegate.Execute(MaxAmmo);
    }
}

//Multicast ȣ��
void AQLPlayerState::UpdateStorageWidget(FName InNickname, AQLLifestoneStorageBox* StorageBox)
{
    //MulticastRPC ȣ��
    MulticastRPCUpdateStorageWidget(InNickname, StorageBox);
}

void AQLPlayerState::ServerRPCInitType_Implementation(int InGenderType, int InGemType)
{
    GenderType = InGenderType; //���ø����̼� ����
    GemType = InGemType;
    UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

    if (DataManager)
    {
        LifeStoneClass = DataManager->GetLifeStoneClass(GemType);

        ClientRPCInitLifeStone(GemType);
        TObjectPtr<AQLPlayerState> PS = this;
        GetWorld()->GetTimerManager().SetTimerForNextTick //���� ƽ���� PlayerState���� ������ �ִ� Ÿ���� �����Ѵ�.
        (
            [PS]()
            {
                AQLPlayerController* PC = Cast<AQLPlayerController>(PS->GetOwner()); 

                if (PC)
                {
                    //ServerRPC���� Init���� ���ưŵ�? �ǈd�� Ȯ���غ�
                    PC->ServerRPCInitPawn(PS->GenderType);
                }
            }
        );
    }
}

void AQLPlayerState::MulticastRPCUpdateStorageWidget_Implementation(FName InNickname, AQLLifestoneStorageBox* StorageBox)
{
    StorageBox->UpdateAlertPanel(InNickname);
}

void AQLPlayerState::ClientRPCConcealLifeStoneUI_Implementation()
{
    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����
    AQLHUD* HUD = Cast<AQLHUD>(PC->GetHUD());

    if (HUD && bHasLifeStone)
    {
        HUD->ConcealLifeStone(false);
    }
    else
    {
        HUD->ConcealLifeStone(true);
    }
}

void AQLPlayerState::ServerRPCConcealLifeStone_Implementation(const FString &InNickname)
{
   
    FCollisionQueryParams Params(TEXT("DetectionItem"), false, this);
    FVector SearchLocation = GetPawn()->GetActorLocation();
    FHitResult NearbyItem;

    float SearchRange = 100.0f;
    //ItemDetectionSocket
    bool bResult = GetWorld()->SweepSingleByChannel(
        NearbyItem,
        SearchLocation,
        SearchLocation,
        FQuat::Identity,
        CCHANNEL_QLBOX,
        FCollisionShape::MakeSphere(SearchRange),
        Params
    );

    if (bResult)
    {
        AQLLifestoneStorageBox* StorageBox = Cast<AQLLifestoneStorageBox>(NearbyItem.GetActor());

        if (StorageBox)
        {
            if (bHasLifeStone)
            {
                if (StorageBox->GetAlreadyHidden() == false) 
                {
                    StorageBox->OnLifespanDelegate.BindUObject(this, &AQLPlayerState::SetDead);
                    StorageBox->OnLifestoneChangedDelegate.BindUObject(this, &AQLPlayerState::SetHasLifeStone);
                    StorageBox->OnUpdateAlertPanel.BindUObject(this, &AQLPlayerState::UpdateStorageWidget);
                }
            }
            StorageBox->ConcealLifeStone(FName(InNickname), bHasLifeStone);
        }
    }

}

void AQLPlayerState::ServerRPCPutLifeStone_Implementation()
{
    if (bHasLifeStone)
    {
        FVector Location = GetPawn()->GetActorLocation(); //Possessed Pawn Position
        FActorSpawnParameters Params;
        Params.Owner = this;
        LifeStone = GetWorld()->SpawnActor<AQLPlayerLifeStone>(LifeStoneClass,Location, FRotator::ZeroRotator, Params);
        LifeStone->InitPosition();
        ClientRPCConcealLifeStoneUI();
        bHasLifeStone = false;
    }
}

float AQLPlayerState::GetStamina()
{
    return PlayerStatInfo->GetStamina();
}

float AQLPlayerState::GetMaxStamina()
{
    return PlayerStatInfo->GetMaxStamina();
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

float AQLPlayerState::GetAmmoCnt()
{
    return WeaponStatInfo->GetAmmoCnt();
}


void AQLPlayerState::Win(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (bIsWin == false)
    {
        bIsWin = !bIsWin;
    }
}

void AQLPlayerState::Dead(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount == 1)
    {
        AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

        if (PC)
        {
            PC->SettingDeathTime(); //���� �÷��̾ ���� �ð��� üũ��.
        }
        AQLCharacterBase* Player = Cast<AQLCharacterBase>(GetPawn());
        if (Player)
        {
            Player->ServerRPCDead();
        }

    }

}

void AQLPlayerState::CoolTimeStamina(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount == 1)
    {
        //UI���� ����
        AQLPlayerController* PC = Cast<AQLPlayerController>(GetPlayerController());
        if (PC && PC->IsLocalController())
        {
            PC->CoolTimeStamina.ExecuteIfBound();
        }

        TObjectPtr<UAbilitySystemComponent> SourceASC = GetAbilitySystemComponent();
        GetWorld()->GetTimerManager().SetTimer(CoolTimer, FTimerDelegate::CreateLambda([SourceASC]()
            {
                if (SourceASC)
                {
                    FGameplayTagContainer Tag(CHARACTER_STATE_NOTRUN);

                    if (SourceASC->HasMatchingGameplayTag(CHARACTER_STATE_NOTRUN))
                    {
                        SourceASC->RemoveLooseGameplayTags(Tag);
                    }
                }
            }
        ), 10.0f, false);
    }
    else
    {
        AQLPlayerController* PC = Cast<AQLPlayerController>(GetPlayerController());
        if (PC && PC->IsLocalController())
        {
            PC->OnResetStamina.ExecuteIfBound();
        }
    }
}

void AQLPlayerState::SetDead()
{
    //���� ���� Dead�� �����Ǿ� �ִٸ� ? 
    AQLCharacterBase* Player = Cast<AQLCharacterBase>(GetPawn());
    if (Player->GetIsDead() == false)
    {
        FGameplayTagContainer TargetTag(CHARACTER_STATE_DANGER);
        ASC->TryActivateAbilitiesByTag(TargetTag);
    }
    else
    {
        //�׾��µ� ������ ã�� ����� �� Danger�� ���̸�, Dead���� üũ����������?
        ASC->AddLooseGameplayTag(CHARACTER_STATE_DANGER);
    }
}

void AQLPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AQLPlayerState, bIsWin);
    DOREPLIFETIME(AQLPlayerState, bHasLifeStone);
    DOREPLIFETIME(AQLPlayerState, GenderType);
    DOREPLIFETIME(AQLPlayerState, GemType);
}
