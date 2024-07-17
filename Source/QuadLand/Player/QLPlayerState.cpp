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
        //ï¿½ï¿½ï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ + RemainigAmmoCnt 
        float MaxAmmo = WeaponStatInfo->GetMaxAmmoCnt() + RemainingAmmoCnt;
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetMaxAmmoCntAttribute(), MaxAmmo); //ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ö´ï¿½ Ammo ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®
    }
}

void AQLPlayerState::BulletWaste(float DiscardedCount)
{
    if (HasAuthority())
    {
        //ï¿½ï¿½ï¿½ï¿½ ï¿½Ö´ï¿½ ï¿½ï¿½ï¿½ï¿½ + RemainigAmmoCnt 
        float MaxAmmo = WeaponStatInfo->GetMaxAmmoCnt() - DiscardedCount;
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetMaxAmmoCntAttribute(), MaxAmmo); //ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ö´ï¿½ Ammo ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®
    }
}

void AQLPlayerState::SetWeaponStat(const UQLWeaponStat* Stat)
{
    if (HasAuthority()&& Stat && ASC)
    {
        //ï¿½ï¿½ï¿½ï¿½ Baseï¿½ï¿½..
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
        //ï¿½ï¿½ï¿½ï¿½ Baseï¿½ï¿½..
        ASC->SetNumericAttributeBase(UQLAS_PlayerStat::GetHealthAttribute(), NewHp);
    }

}
void AQLPlayerState::AddStaminaStat(float Stamina)
{
    if (CoolTimer.IsValid())
    {
        //ÄðÅ¸ÀÓ Ãë¼Ò
        FGameplayTagContainer Tag(CHARACTER_STATE_NOTRUN);
        ASC->RemoveLooseGameplayTags(Tag);
        GetWorld()->GetTimerManager().ClearTimer(CoolTimer);
    }

    if (HasAuthority() && ASC)
    {
        //ï¿½ï¿½ï¿½ï¿½ Baseï¿½ï¿½.. -> ï¿½Ï´ï¿½ ï¿½Ì·ï¿½ï¿½ï¿½ ï¿½Î°ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 
        FGameplayTagContainer Tag(STAT_STAMINA_STATUS_REDUCE);
        ASC->RemoveActiveEffectsWithTags(Tag);

    }
}
void AQLPlayerState::ResetWeaponStat(const UQLWeaponStat* Stat)
{
    if (HasAuthority() && Stat && ASC)
    {
        //ï¿½ï¿½ï¿½ï¿½ Baseï¿½ï¿½..
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDamageAttribute(), 10.0f); //Default Attack - ï¿½ï¿½Ã¸ï¿?ï¿½Ïµï¿½ï¿½Úµï¿½
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDistanceAttribute(), WeaponStatInfo->GetAttackDistance() - Stat->AttackDist);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAmmoCntAttribute(), WeaponStatInfo->GetAmmoCnt() - Stat->AmmoCnt); //ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö´ï¿½ ï¿½Ñ¾ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½!!!!)
    }
}

void AQLPlayerState::UseGlassesItem(float Time) //ï¿½ï¿½Ä£ï¿½ï¿½ï¿½ï¿½ Å¬ï¿½ï¿½ï¿½Ì¾ï¿½Æ®ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ç¾ï¿½ï¿½ï¿½ï¿?
{
    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ PCï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
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
    //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ÔµÇ¸ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½(ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½)
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

    //Playerï¿½ï¿½ QLPlayerHUDWidget ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ -> ï¿½Ì¸ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ø¾ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½Ìºï¿½ï¿½Î´ï¿½;;
    if (UpdateRemainingDelegate.IsBound())
    {
        UpdateRemainingDelegate.Execute(MaxAmmo);
    }
}

//Multicast È£ï¿½ï¿½
void AQLPlayerState::UpdateStorageWidget(FName InNickname, AQLLifestoneStorageBox* StorageBox)
{
    //MulticastRPC È£ï¿½ï¿½
    MulticastRPCUpdateStorageWidget(InNickname, StorageBox);
}

void AQLPlayerState::ServerRPCInitType_Implementation(int InGenderType, int InGemType)
{
    GenderType = InGenderType; //¸®ÇÃ¸®ÄÉÀÌ¼Ç Àü´Þ
    GemType = InGemType;
    UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

    if (DataManager)
    {
        LifeStoneClass = DataManager->GetLifeStoneClass(GemType);

        ClientRPCInitLifeStone(GemType);
        TObjectPtr<AQLPlayerState> PS = this;
        GetWorld()->GetTimerManager().SetTimerForNextTick //´ÙÀ½ Æ½¿¡¼­ PlayerState¿¡¼­ °¡Áö°í ÀÖ´Â Å¸ÀÔÀ» Àü´ÞÇÑ´Ù.
        (
            [PS]()
            {
                AQLPlayerController* PC = Cast<AQLPlayerController>(PS->GetOwner()); 

                if (PC)
                {
                    //ServerRPC¿¡¼­ InitÀ¸·Î °íÃÆ°Åµç? µÇˆdÁö È®ÀÎÇØºÁ
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
    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ PCï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
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
        AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //¼ÒÀ¯±ÇÀº PC°¡ °¡Áü

        if (PC)
        {
            PC->SettingDeathTime(); //½ÇÁ¦ ÇÃ·¹ÀÌ¾î°¡ Á×Àº ½Ã°£À» Ã¼Å©ÇÔ.
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
        //UI»ö»ó º¯°æ
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
    //¸¸¾à ÇöÀç Dead°¡ ºÎÂøµÇ¾î ÀÖ´Ù¸é ? 
    AQLCharacterBase* Player = Cast<AQLCharacterBase>(GetPawn());
    if (Player->GetIsDead() == false)
    {
        FGameplayTagContainer TargetTag(CHARACTER_STATE_DANGER);
        ASC->TryActivateAbilitiesByTag(TargetTag);
    }
    else
    {
        //Á×¾ú´Âµ¥ º¸¼®À» Ã£Àº °æ¿ìÀÇ ¼ö DangerÀ» ºÙÀÌ¸é, Dead¿¡¼­ Ã¼Å©ÇÏÁö¾ÊÀ»±î?
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
