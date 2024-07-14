// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/QLInventoryComponent.h"
#include "Player/QLPlayerController.h"
#include "Player/QLPlayerState.h"
#include "Components/SphereComponent.h"
#include "Character/QLCharacterPlayer.h"
#include "GameData/QLDataManager.h"
#include "Interface/QLGetItemStat.h"
#include "GameData/QLItemData.h"
#include "Physics/QLCollision.h"
#include "UI/QLUserWidget.h"
#include "Item/QLItemBox.h"
#include "QuadLand.h"

UQLInventoryComponent::UQLInventoryComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsSetVisibleInventory = false;
}


void UQLInventoryComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	AQLItemBox* HitItem = Cast<AQLItemBox>(OtherActor);

	if (HitItem == nullptr)
	{
		return;
	}
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	UQLItemData* ItemData = CastChecked<UQLItemData>(HitItem->Stat);
	ItemData->CurrentItemCnt = 1;

	if (NearbyItems.Find(ItemData->ItemType))
	{
		NearbyItems[ItemData->ItemType].Push(OtherActor);
	}
	else
	{
		//�ӽ÷�, TArray�� �����ؼ�, NearbyItems �ڷᱸ���� ������ �� �ֵ��� �Ѵ�.
		TArray<TObjectPtr<AActor>> ActorArray;
		ActorArray.Add(OtherActor);
		NearbyItems.Add(ItemData->ItemType, ActorArray);
	}

	PC->UpdateNearbyItemEntry(ItemData);
}

void UQLInventoryComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AQLItemBox* HitItem = Cast<AQLItemBox>(OtherActor);

	if (HitItem == nullptr)
	{
		return;
	}
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	UQLItemData* ItemData = CastChecked<UQLItemData>(HitItem->Stat);

	QL_SUBLOG(QLLog, Log, TEXT("No Remove Actor, Current Cnt %d"), NearbyItems[ItemData->ItemType].Num());
	if (NearbyItems.Find(ItemData->ItemType))
	{
		NearbyItems[ItemData->ItemType].Remove(OtherActor);
	}

	QL_SUBLOG(QLLog, Log, TEXT("Remove Actor -1, Current Cnt %d"), NearbyItems[ItemData->ItemType].Num());

	PC->RemoveNearbyItemEntry(ItemData);
}

void UQLInventoryComponent::AddItem(EItemType ItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(ItemId))
	{
		InventoryItem[ItemId] += ItemCnt; //������ 0�� ����
	}
	else
	{
		InventoryItem.Add(ItemId, ItemCnt);
	}

	QL_SUBLOG(QLNetLog, Warning, TEXT("Item Add %d"), InventoryItem[ItemId]);
}

void UQLInventoryComponent::UseItem(EItemType ItemId)
{

	if (EItemType::Ammo == ItemId || EItemType::Bomb == ItemId)
	{
		return; //��� ����� �� ����;;
	}

	if (InventoryItem.Find(ItemId))
	{
		ServerRPCRemoveItem(ItemId, InventoryItem[ItemId]);
	}
}

int UQLInventoryComponent::GetInventoryCnt(EItemType ItemType)
{
	if (InventoryItem.Find(ItemType) == 0)
		return 0;
	return InventoryItem[ItemType];
}

bool UQLInventoryComponent::ServerRPCRemoveItem_Validate(EItemType ItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(ItemId) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Index is currently exceeded"));
		return false;
	}

	if (InventoryItem[ItemId] != ItemCnt)
	{
		UE_LOG(LogTemp, Warning, TEXT("The number of items does not match. %d %d"), InventoryItem[ItemId], ItemCnt);
		return false;
	}

	if (InventoryItem[ItemId] < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Use Item. %d %d"), InventoryItem[ItemId], ItemCnt);

		return false;
	}

	return true;
}

void UQLInventoryComponent::ServerRPCRemoveItem_Implementation(EItemType InItemId, int32 InItemCnt)
{
	AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
	if (PS == nullptr)
	{
		return;
	}


	if (InventoryItem[InItemId] <= 0)
	{
		return ;
	}
	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	//�������� �����ϰ� �ִ� Manager �����´�.

	//�̺κ� ���� �����ؼ� �����ϵ��� ���� ����.
	UQLItemData* ItemData = DataManager->GetItem(InItemId);

	IQLGetItemStat* ItemStat = Cast<IQLGetItemStat>(ItemData);

	if (DataManager == nullptr)
	{
		return;
	}
	int32 ItemCnt = --InventoryItem[InItemId]; //�ϳ� ���

	if (ItemStat)
	{
		switch (InItemId)
		{
		case EItemType::StaminaRecoveryItem:
			PS->AddStaminaStat(ItemStat->GetStat());
			MulticastRPCItemMotion(InItemId);
			//����� �ְ����..
			break;
		case EItemType::HPRecoveryItem:
			PS->AddHPStat(ItemStat->GetStat());
			break;
		case EItemType::DiscoveryItem:
			PS->UseGlassesItem(ItemStat->GetStat());
			break;
		}
	}
	ClientRPCRemoveItem(InItemId, ItemCnt); //Ŭ��� ������ ���� ��ġ
}

void UQLInventoryComponent::ClientRPCRemoveItem_Implementation(EItemType InItemId, int32 ItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	UQLItemData* ItemData = DataManager->GetItem(InItemId);

	InventoryItem[InItemId] = ItemCnt;
	ItemData->CurrentItemCnt = ItemCnt;

	if (InItemId == EItemType::Bomb && InventoryItem[InItemId] <= 0)
	{
		PC->UpdateEquipBombUI(false);
	}
	PC->UpdateItemEntry(ItemData, ItemCnt);
}


void UQLInventoryComponent::AddInventoryByDraggedItem(EItemType InItemId)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	if (!HasAuthority())
	{
		//���� ���� 
		AddItem(InItemId, NearbyItems[InItemId].Num());
	}

	///UI����
	if (InItemId == EItemType::Bomb)
	{
		PC->UpdateEquipBombUI(true);
	}
	////������ �������� �ִ��� �˻��ϱ� ���ؼ� �������� ��û�ؾ���;
	PC->BlinkBag();
	ServerRPCAddInventoryByDraggedItem(InItemId);
	NearbyItems[InItemId].Empty(); //�ֺ��� �ִ� ItemId�� ���ؼ� ������ ��� ���ش�. -> Why? Inventory�ȿ� �־����ϱ�.
}

void UQLInventoryComponent::ServerRPCAddInventoryByDraggedItem_Implementation(EItemType ItemId)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (PC == nullptr|| Character == nullptr)
	{
		return;
	}
	if (NearbyItems[ItemId].Num() == 0)
	{
		return;
	}

	for (const auto& NearbyItem : NearbyItems[ItemId])
	{
		NearbyItem->SetLifeSpan(0.3f);
	}

	if (ItemId == EItemType::Ammo)
	{
		AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
		if (PS == nullptr)
		{
			return;
		}
		UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());
		UQLItemData* Data = DataManager->GetItem(ItemId);
		IQLGetItemStat* ItemStat = CastChecked<IQLGetItemStat>(Data);
		PS->SetAmmoStat(ItemStat->GetStat() * NearbyItems[ItemId].Num()); //�Ѿ��� ���� * ���� �ֿ� �Ѿ� ���� �׸��� Empty�� ���½����ش�.
	}

	AddItem(ItemId, NearbyItems[ItemId].Num());

	NearbyItems[ItemId].Empty(); //��� ���ش�.
}

void UQLInventoryComponent::AddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt)
{
	if (!HasAuthority())
	{
		if (InventoryItem.Find(ItemId))
		{
			InventoryItem[ItemId] -= ItemCnt;
		}
		else
		{
			return;
		}
	}
	//UI����
	if (ItemId == EItemType::Bomb)
	{
		AQLPlayerController* PC = GetController<AQLPlayerController>();
		PC->UpdateEquipBombUI(false);
	}
	//Server RPC ���� -> Server ������ ���� �� ������ ���� 
	ServerRPCAddGroundByDraggedItem(ItemId, ItemCnt);
}

void UQLInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}
	if (Character->InventoryOverlap)
	{
		if (Character->InventoryOverlap->OnComponentBeginOverlap.IsBound() == false)
		{
			Character->InventoryOverlap->OnComponentBeginOverlap.AddDynamic(this, &UQLInventoryComponent::OnOverlapBegin);
		}
		if (Character->InventoryOverlap->OnComponentEndOverlap.IsBound() == false)
		{
			Character->InventoryOverlap->OnComponentEndOverlap.AddDynamic(this, &UQLInventoryComponent::OnOverlapEnd);
		}
	}
}

void UQLInventoryComponent::MulticastRPCItemMotion_Implementation(EItemType ItemId)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();
	if (Character == nullptr)
	{
		return;
	}

	switch (ItemId)
	{
		case EItemType::StaminaRecoveryItem:
			Character->PlayAnimMontage(ItemMontage[ItemId]); //Stand
		break;
	}
}


void UQLInventoryComponent::ServerRPCAddGroundByDraggedItem_Implementation(EItemType ItemId, int32 ItemCnt)
{
	AQLCharacterPlayer* Character = GetPawn<AQLCharacterPlayer>();

	if (Character == nullptr)
	{
		return;
	}
	int RemainingItemCnt = 0;
	if (InventoryItem.Find(ItemId))
	{
		RemainingItemCnt = InventoryItem[ItemId];
		InventoryItem[ItemId] -= ItemCnt;
		UE_LOG(LogTemp, Warning, TEXT("Current Server Item Cnt %d"), InventoryItem[ItemId]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error / is not valid %d"), ItemCnt);
		return;
	}

	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	//�������� �����ϰ� �ִ� Manager �����´�.

	//�̺κ� ���� �����ؼ� �����ϵ��� ���� ����.
	//Data�Ŵ����� ������ �ִ� �ʱⰪ ��ŭ Stat�� ����
	for (int i = 0; i < RemainingItemCnt; i++)
	{
		FVector Location = Character->GetActorLocation();
		FActorSpawnParameters Params;
		Params.Owner = Character;
		AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(DataManager->GetItemBoxClass(ItemId), Location, FRotator::ZeroRotator, Params);
		GroundItem->SetPhysics();

		if (ItemId == EItemType::Ammo)
		{
			IQLGetItemStat* AmmoStatData = CastChecked<IQLGetItemStat>(DataManager->GetItem(ItemId));
			AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
			if (PS == nullptr)
			{
				return;
			}
			PS->BulletWaste(AmmoStatData->GetStat()); //���� ���� �� 
		}
	}
}

void UQLInventoryComponent::ClientRPCAddItem_Implementation(EItemType ItemId, int32 ItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();

	if (PC == nullptr)
	{
		return;
	}

	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	UQLItemData* ItemData = DataManager->GetItem(ItemId);

	if (GetNetMode() == ENetMode::NM_Client)
	{
		ItemData->CurrentItemCnt = GetInventoryCnt(ItemId) + ItemCnt;
		AddItem(ItemId, ItemCnt);
	}
	else
	{
		ItemData->CurrentItemCnt = GetInventoryCnt(ItemId); //������ �̹� ������ �����Ǿ���.
	}
	PC->UpdateItemEntry(ItemData, ItemData->CurrentItemCnt);

	PC->BlinkBag();
}

void UQLInventoryComponent::ClientRPCRollbackInventory_Implementation(EItemType InItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(InItemId))
	{
		InventoryItem[InItemId] -= ItemCnt;
	}
}