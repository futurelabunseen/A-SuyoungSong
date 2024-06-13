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
   
    //Event 등록한다 -> Equip을 가질때
    //Event 등록한다 -> Equip없을 때
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
        //현재 있는 개수 + RemainigAmmoCnt 
        float MaxAmmo = WeaponStatInfo->GetMaxAmmoCnt() + RemainingAmmoCnt;
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetMaxAmmoCntAttribute(), MaxAmmo); //현재 남아있는 Ammo 업데이트
    }
}

void AQLPlayerState::BulletWaste(float DiscardedCount)
{
    if (HasAuthority())
    {
        //현재 있는 개수 + RemainigAmmoCnt 
        float MaxAmmo = WeaponStatInfo->GetMaxAmmoCnt() - DiscardedCount;
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetMaxAmmoCntAttribute(), MaxAmmo); //현재 남아있는 Ammo 업데이트
    }
}

void AQLPlayerState::SetWeaponStat(const UQLWeaponStat* Stat)
{
    if (HasAuthority()&& Stat && ASC)
    {
        //원래 Base값..
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
        //원래 Base값..
        ASC->SetNumericAttributeBase(UQLAS_PlayerStat::GetHealthAttribute(), NewHp);
    }

}
void AQLPlayerState::AddStaminaStat(float Stamina)
{
    if (HasAuthority() && ASC)
    {
        //원래 Base값.. -> 일단 이렇게 두고 수정하는 방향으로 갈 예정 
        FGameplayTagContainer Tag(STAT_STAMINA_STATUS_REDUCE);
        ASC->RemoveActiveEffectsWithTags(Tag);

    }
}
void AQLPlayerState::ResetWeaponStat(const UQLWeaponStat* Stat)
{
    if (HasAuthority() && Stat && ASC)
    {
        //원래 Base값..
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDamageAttribute(), 10.0f); //Default Attack - 잠시만 하드코딩
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAttackDistanceAttribute(), WeaponStatInfo->GetAttackDistance() - Stat->AttackDist);
        ASC->SetNumericAttributeBase(UQLAS_WeaponStat::GetAmmoCntAttribute(), WeaponStatInfo->GetAmmoCnt() - Stat->AmmoCnt); //현재 가지고있는 총알은 변하지 않음 (대신 조건 부착, 총이 없다!!!!)
    }
}

void AQLPlayerState::UseGlassesItem(float Time) //이친구는 클라이언트에서 사용되어야함.
{
    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐
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
        AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner());

        if (PC && PC->IsLocalController())
        {
            ServerRPCInitType(GameInstance->GetGenderType(), GameInstance->GetGemMatType());
        }
    }
}



void AQLPlayerState::OnChangedStamina(const FOnAttributeChangeData& Data)
{
    float CurrentStamina = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC && PC->IsLocalController())
    {
        //Player의 QLPlayerHpBarWidget 가져옴
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedStaminaPercentage(CurrentStamina, GetMaxStamina());
    }
}

void AQLPlayerState::OnChangedMaxStamina(const FOnAttributeChangeData& Data)
{
    float CurrentMaxStamina = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC && PC->IsLocalController())
    {
        //Player의 QLPlayerHpBarWidget 가져옴
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedStaminaPercentage(GetStamina(), CurrentMaxStamina);
    }
}

void AQLPlayerState::OnChangedHp(const FOnAttributeChangeData& Data)
{
    float CurrentHP = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐
    
    if (PC && PC->IsLocalController())
    {
        //Player의 QLPlayerHpBarWidget 가져옴
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedHPPercentage(CurrentHP, GetMaxHealth());
    }
}

void AQLPlayerState::OnChangedMaxHp(const FOnAttributeChangeData& Data)
{
    //아이템 도입되면 시작할 예정(다음주)
    float MaxHp = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC && PC->IsLocalController())
    {
        //Player의 QLPlayerHUDWidget 가져옴 -> 이름변경해야할 각이보인다;;
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedHPPercentage(GetHealth(), MaxHp);
    }
}

void AQLPlayerState::OnChangedAmmoCnt(const FOnAttributeChangeData& Data)
{
    float CurrentAmmo = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC && PC->IsLocalController())
    {
        //Player의 QLPlayerHUDWidget 가져옴 -> 이름변경해야할 각이보인다;;
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedAmmoCnt(CurrentAmmo);
    }
}

void AQLPlayerState::OnChangedMaxAmmoCnt(const FOnAttributeChangeData& Data)
{
    float MaxAmmo = Data.NewValue;

    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC && PC->IsLocalController())
    {
        //Player의 QLPlayerHUDWidget 가져옴 -> 이름변경해야할 각이보인다;;
        UQLUserWidget* Widget = Cast<UQLUserWidget>(PC->GetPlayerUIWidget());
        Widget->ChangedRemainingAmmo(MaxAmmo);
    }
}

//Multicast 호출
void AQLPlayerState::UpdateStorageWidget(FName InNickname, AQLLifestoneStorageBox* StorageBox)
{
    //MulticastRPC 호출
    MulticastRPCUpdateStorageWidget(InNickname, StorageBox);
}

void AQLPlayerState::ServerRPCInitType_Implementation(int InGenderType, int InGemType)
{
    GenderType = InGenderType;
    GemType = InGemType;
    UQLGameInstance* GameInstance = Cast<UQLGameInstance>(GetWorld()->GetGameInstance());
    UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

    if (DataManager)
    {
        LifeStoneMaterial = DataManager->GemColor(GameInstance->GetGemMatType());
    }

    FTimerHandle InitPawnTimer;
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
        {
            AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

            QL_LOG(QLLog, Log, TEXT("Gender Type %d"), GenderType);
            if (PC)
            {
                PC->InitPawn(GenderType);
            }
        });


}

void AQLPlayerState::MulticastRPCUpdateStorageWidget_Implementation(FName InNickname, AQLLifestoneStorageBox* StorageBox)
{
    StorageBox->UpdateAlertPanel(InNickname);
}

void AQLPlayerState::ClientRPCConcealLifeStoneUI_Implementation()
{
    AQLPlayerController* PC = Cast<AQLPlayerController>(GetOwner()); //소유권은 PC가 가짐

    if (PC && bHasLifeStone)
    {
        PC->ConcealLifeStone();
    }
}

void AQLPlayerState::ServerRPCConcealLifeStone_Implementation(const FString &InNickname)
{
   
    //라인트레이스를 쏘아서 가장 가까이 있는 오브젝트를 체크한다.
    //그리고 그 오브젝트에게 현재 라이프 스톤을 저장한 플레이어 컨트롤러가 누구인지를 전달함.
    //현재 숨겨진 오브젝트를 저장한다.
    //서버에서만 적용
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
                //이름을 전달해야하거든? 이것 또한, 같은 인터페이스를 사용

                if (StorageBox->GetAlreadyHidden() == false)  //이미 숨겨져 있지 않다면, 연결
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
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
            {
                LifeStone->GetMesh()->SetMaterial(0, LifeStoneMaterial);
            });
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
    AQLCharacterBase* Player = Cast<AQLCharacterBase>(GetPawn());
    if (Player && Player->GetIsDead() == false)
    {
        FGameplayTagContainer TargetTag(CHARACTER_STATE_DEAD);
        ASC->TryActivateAbilitiesByTag(TargetTag);
    }
    if (HasAuthority())
    {
        if (Player)
        {
            Player->SetIsDead(!Player->GetIsDead());
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
    DOREPLIFETIME(AQLPlayerState, LifeStoneMaterial);
}
