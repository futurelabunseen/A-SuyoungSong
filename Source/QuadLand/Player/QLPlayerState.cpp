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
    //UAbilitySystemComponent *TargetASC = WeaponInfo->GetAbilitySystemComponent();

    if (HasAuthority()&& Stat && ASC)
    {
        //���� Base��..
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDamageAttribute(), Stat->Damage);
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
        QL_LOG(QLNetLog, Warning, TEXT("this?"));
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
    QL_LOG(QLNetLog, Warning, TEXT("this? %f"),Time);
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
}

void AQLPlayerState::OnChangedStamina(const FOnAttributeChangeData& Data)
{
    float CurrentStamina = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����

    if (PC && PC->IsLocalController())
    {
        //Player�� QLPlayerHpBarWidget ������
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedStaminaPercentage(CurrentStamina, GetMaxStamina());
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
        Widget->ChangedStaminaPercentage(GetStamina(), CurrentMaxStamina);
    }
}

void AQLPlayerState::OnChangedHp(const FOnAttributeChangeData& Data)
{
    float CurrentHP = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //�������� PC�� ����
    QL_LOG(QLNetLog, Log, TEXT("%lf %lf"), CurrentHP, GetMaxHealth());
    if (PC && PC->IsLocalController())
    {
        //Player�� QLPlayerHpBarWidget ������
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedHPPercentage(CurrentHP, GetMaxHealth());
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

//Multicast ȣ��
void AQLPlayerState::UpdateStorageWidget(FName Nickname, AQLLifestoneStorageBox* StorageBox)
{
    //MulticastRPC ȣ��
    MulticastRPCUpdateStorageWidget(Nickname, StorageBox);
}

void AQLPlayerState::MulticastRPCUpdateStorageWidget_Implementation(FName Nickname, AQLLifestoneStorageBox* StorageBox)
{
    StorageBox->UpdateAlertPanel(Nickname);
}

void AQLPlayerState::ServerRPCConcealLifeStone_Implementation()
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

#if ENABLE_DRAW_DEBUG
    FColor Color = bResult ? FColor::Green : FColor::Red;
    DrawDebugSphere(GetWorld(), SearchLocation, SearchRange, 10.0f, Color, false, 5.0f);
#endif

    if (bResult)
    {
        AQLLifestoneStorageBox* StorageBox = Cast<AQLLifestoneStorageBox>(NearbyItem.GetActor());

        if (StorageBox)
        {
            if (bHasLifeStone)
            {
                QL_LOG(QLNetLog, Log, TEXT("HasLifeStone"));

                //�̸��� �����ؾ��ϰŵ�? �̰� ����, ���� �������̽��� ���

                if (StorageBox->GetAlreadyHidden() == false)  //�̹� ������ ���� �ʴٸ�, ����
                {
                    StorageBox->OnLifespanDelegate.BindUObject(this, &AQLPlayerState::SetDead);
                    StorageBox->OnLifestoneChangedDelegate.BindUObject(this, &AQLPlayerState::SetHasLifeStone);
                    StorageBox->OnUpdateAlertPanel.BindUObject(this, &AQLPlayerState::UpdateStorageWidget);
                }
            }
            StorageBox->ConcealLifeStone(FName(GetName()));
        }
    }

}

void AQLPlayerState::ServerRPCPutLifeStone_Implementation()
{
    if (bHasLifeStone)
    {
        QL_LOG(QLNetLog, Log, TEXT("HasLifeStone"));
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
    if (bIsDead == false)
    {
        FGameplayTagContainer TargetTag(CHARACTER_STATE_DANGER);
        ASC->TryActivateAbilitiesByTag(TargetTag);
    }
    
    QL_LOG(QLNetLog, Log, TEXT("Current Dead %d %s"), bIsDead, *GetName());
}

void AQLPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AQLPlayerState, bIsDead);
    DOREPLIFETIME(AQLPlayerState, bIsWin);
    DOREPLIFETIME(AQLPlayerState, bHasLifeStone);
    DOREPLIFETIME(AQLPlayerState, LifeStone);

}


