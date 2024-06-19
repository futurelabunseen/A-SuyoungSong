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
   
    //Event ����Ѵ� -> Equip�� ������
    //Event ����Ѵ� -> Equip���� ��
    NetUpdateFrequency = 30.0f;

    //TagEvent - Delegates
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_DEAD, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Dead);
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_WIN, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::Win);
    ASC->RegisterGameplayTagEvent(CHARACTER_STATE_NOTRUN, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AQLPlayerState::CoolTimeStamina);
    ASC->AddLooseGameplayTag(CHARACTER_EQUIP_NON);

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
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDamageAttribute(), 10.0f); //Default Attack - ��ø� �ϵ��ڵ�
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDistanceAttribute(), WeaponStatInfo->GetAttackDistance() - Stat->AttackDist);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAmmoCntAttribute(), WeaponStatInfo->GetAmmoCnt() - Stat->AmmoCnt); //���� �������ִ� �Ѿ��� ������ ���� (��� ���� ����, ���� ����!!!!)
    }
}

void AQLPlayerState::UseGlassesItem(float Time) //��ģ���� Ŭ���̾�Ʈ���� ���Ǿ����.
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

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

    if (PC && PC->IsLocalController())
    {
        //Player�� QLPlayerHpBarWidget ������
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        if (Widget)
        {
            Widget->ChangedStaminaPercentage(CurrentStamina, GetMaxStamina());
        }
    }
}

void AQLPlayerState::OnChangedMaxStamina(const FOnAttributeChangeData& Data)
{
    float CurrentMaxStamina = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

    if (PC && PC->IsLocalController())
    {
        //Player�� QLPlayerHpBarWidget ������
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        if (Widget)
        {
            Widget->ChangedStaminaPercentage(GetStamina(), CurrentMaxStamina);
        }
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
        if (Widget)
        {
            Widget->ChangedHPPercentage(CurrentHP, GetMaxHealth());
        }
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

        if (Widget)
        {
            Widget->ChangedHPPercentage(GetHealth(), MaxHp);
        }
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
        if (Widget)
        {
            Widget->ChangedAmmoCnt(CurrentAmmo);
        }
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
        if (Widget)
        {
            Widget->ChangedRemainingAmmo(MaxAmmo);

        }
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
    GenderType = InGenderType;
    GemType = InGemType;
    UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

    if (DataManager)
    {
        LifeStoneClass = DataManager->GetLifeStoneClass(GemType);

        ClientRPCInitLifeStone(GemType);
        TObjectPtr<AQLPlayerState> PS = this;
        GetWorld()->GetTimerManager().SetTimerForNextTick
        (
            [PS]()
            {
                AQLPlayerController* PC = Cast<AQLPlayerController>(PS->GetOwner()); //�������� PC�� ����

                if (PC)
                {
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

    if (PC && bHasLifeStone)
    {
        PC->ConcealLifeStone();
    }
}

void AQLPlayerState::ServerRPCConcealLifeStone_Implementation(const FString &InNickname)
{
   
    //����Ʈ���̽��� ��Ƽ� ���� ������ �ִ� ������Ʈ�� üũ�Ѵ�.
    //�׸��� �� ������Ʈ���� ���� ������ ������ ������ �÷��̾� ��Ʈ�ѷ��� ���������� ������.
    //���� ������ ������Ʈ�� �����Ѵ�.
    //���������� ����
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
        CCHANNEL_QLITEMACTION,
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
                //�̸��� �����ؾ��ϰŵ�? �̰� ����, ���� �������̽��� ���

                if (StorageBox->GetAlreadyHidden() == false)  //�̹� ������ ���� �ʴٸ�, ����
                {
                    StorageBox->OnLifespanDelegate.BindUObject(this, &AQLPlayerState::SetDead);
                    StorageBox->OnLifestoneChangedDelegate.BindUObject(this, &AQLPlayerState::SetHasLifeStone);
                    StorageBox->OnUpdateAlertPanel.BindUObject(this, &AQLPlayerState::UpdateStorageWidget);
                }
            }
            StorageBox->ConcealLifeStone(FName(InNickname));
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
    bIsWin = !bIsWin;
}

void AQLPlayerState::Dead(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount == 1)
    {
        AQLCharacterBase* Player = Cast<AQLCharacterBase>(GetPawn());
        if (Player && Player->GetIsDead() == false)
        {
            FGameplayTagContainer TargetTag(CHARACTER_STATE_DEAD);
            ASC->TryActivateAbilitiesByTag(TargetTag);
            
            AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

            if (PC)
            {
                PC->SettingDeathTime();
            }
            //�����
        }
        if (HasAuthority())
        {
            if (Player)
            {
                Player->SetIsDead(!Player->GetIsDead());
            }
        }
    }

}

void AQLPlayerState::CoolTimeStamina(const FGameplayTag CallbackTag, int32 NewCount)
{
    if (NewCount == 1)
    {
        GetWorld()->GetTimerManager().SetTimer(CoolTimer, FTimerDelegate::CreateLambda([&]()
            {
                FGameplayTagContainer Tag(CHARACTER_STATE_NOTRUN);
                ASC->RemoveLooseGameplayTags(Tag);
            }
        ), 10.0f, false);
    }
}

void AQLPlayerState::SetDead()
{
    AQLCharacterBase* Player = Cast<AQLCharacterBase>(GetPawn());
    if (Player->GetIsDead() == false)
    {
        FGameplayTagContainer TargetTag(CHARACTER_STATE_DANGER);
        ASC->TryActivateAbilitiesByTag(TargetTag);
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
