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
#include "HUD/QLHUD.h"
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
		//임시로, TArray를 생성해서, NearbyItems 자료구조에 삽입할 수 있도록 한다.
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
		InventoryItem[ItemId] += ItemCnt; //없으면 0을 리턴
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
		return; //얘는 사용할 수 없어;;
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

	//아이템을 보관하고 있는 Manager 가져온다.

	//이부분 새로 생성해서 전달하도록 변경 했음.
	UQLItemData* ItemData = DataManager->GetItem(InItemId);

	IQLGetItemStat* ItemStat = Cast<IQLGetItemStat>(ItemData);

	if (DataManager == nullptr)
	{
		return;
	}
	int32 ItemCnt = --InventoryItem[InItemId]; //하나 사용

	if (ItemStat)
	{
		switch (InItemId)
		{
		case EItemType::StaminaRecoveryItem:
			PS->AddStaminaStat(ItemStat->GetStat());
			MulticastRPCItemMotion(InItemId);
			//모션을 넣고싶음..
			break;
		case EItemType::HPRecoveryItem:
			PS->AddHPStat(ItemStat->GetStat());
			break;
		case EItemType::DiscoveryItem:
			PS->UseGlassesItem(ItemStat->GetStat());
			break;
		}
	}
	ClientRPCRemoveItem(InItemId, ItemCnt); //클라랑 서버랑 개수 일치
}

void UQLInventoryComponent::ClientRPCRemoveItem_Implementation(EItemType InItemId, int32 ItemCnt)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	AQLHUD* HUD = Cast<AQLHUD>(PC->GetHUD());
	if (PC == nullptr || HUD == nullptr)
	{
		return;
	}

	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	UQLItemData* ItemData = DataManager->GetItem(InItemId);

	InventoryItem[InItemId] = ItemCnt;
	ItemData->CurrentItemCnt = ItemCnt;

	if (InItemId == EItemType::Bomb && InventoryItem[InItemId] <= 0)
	{
		HUD->UpdateEquipBombUI(false);
	}
	PC->UpdateItemEntry(ItemData, ItemCnt);
}


void UQLInventoryComponent::AddInventoryByDraggedItem(EItemType InItemId)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	AQLHUD* HUD = Cast<AQLHUD>(PC->GetHUD());
	if (PC == nullptr || HUD == nullptr)
	{
		return;
	}

	if (!HasAuthority())
	{
		//현재 개수 
		AddItem(InItemId, NearbyItems[InItemId].Num());
	}

	///UI변경
	if (InItemId == EItemType::Bomb)
	{
		HUD->UpdateEquipBombUI(true);
	}
	////실제로 아이템이 있는지 검사하기 위해서 서버에게 요청해야함;

	AQLHUD* LocalHUD = Cast<AQLHUD>(PC->GetHUD());
	if (LocalHUD)
	{
		LocalHUD->BlinkBag();
	}
	ServerRPCAddInventoryByDraggedItem(InItemId);
	NearbyItems[InItemId].Empty(); //주변에 있는 ItemId에 대해서 개수를 모두 없앤다. -> Why? Inventory안에 넣었으니깐.
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
		PS->SetAmmoStat(ItemStat->GetStat() * NearbyItems[ItemId].Num()); //총알의 개수 * 현재 주운 총알 개수 그리고 Empty로 리셋시켜준다.
	}

	AddItem(ItemId, NearbyItems[ItemId].Num());

	NearbyItems[ItemId].Empty(); //모두 없앤다.
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

void UQLInventoryComponent::AddGroundByDraggedItem(EItemType ItemId)
{
	AQLPlayerController* PC = GetController<AQLPlayerController>();
	AQLHUD* HUD = Cast<AQLHUD>(PC->GetHUD());
	if (PC == nullptr || HUD == nullptr)
	{
		return;
	}
	if (!HasAuthority())
	{
		if (InventoryItem.Find(ItemId))
		{
			InventoryItem[ItemId] = 0; //땅에 버리게 되면, 모든 아이템이 버려지기 때문에 0을 유지
		}
		else
		{
			return;
		}
	}
	//UI변경
	if (ItemId == EItemType::Bomb)
	{
		HUD->UpdateEquipBombUI(false);
	}
	//Server RPC 전송 -> Server 아이템 생성 및 아이템 조정 
	ServerRPCAddGroundByDraggedItem(ItemId);
}

void UQLInventoryComponent::ServerRPCAddGroundByDraggedItem_Implementation(EItemType ItemId)
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
		InventoryItem[ItemId] = 0;
		UE_LOG(LogTemp, Warning, TEXT("Current Server Item Cnt %d"), RemainingItemCnt);
	}
	else
	{
		return;
	}

	UQLDataManager* DataManager = UGameInstance::GetSubsystem<UQLDataManager>(GetWorld()->GetGameInstance());

	//아이템을 보관하고 있는 Manager 가져온다.

	//이부분 새로 생성해서 전달하도록 변경 했음.
	//Data매니저가 가지고 있는 초기값 만큼 Stat을 없앰
	for (int i = 0; i < RemainingItemCnt; i++)
	{
		FVector Location = Character->GetActorLocation();
		FActorSpawnParameters Params;
		Params.Owner = Character;
		AQLItemBox* GroundItem = GetWorld()->SpawnActor<AQLItemBox>(DataManager->GetItemBoxClass(ItemId), Location, FRotator::ZeroRotator, Params);
		GroundItem->PlaceOnTheGround();

		if (ItemId == EItemType::Ammo)
		{
			IQLGetItemStat* AmmoStatData = CastChecked<IQLGetItemStat>(DataManager->GetItem(ItemId));
			AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
			if (PS == nullptr)
			{
				return;
			}
			PS->BulletWaste(AmmoStatData->GetStat()); //땅에 버릴 때 
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
		ItemData->CurrentItemCnt = GetInventoryCnt(ItemId); //서버는 이미 이전에 증가되었음.
	}
	PC->UpdateItemEntry(ItemData, ItemData->CurrentItemCnt);

	AQLHUD* LocalHUD = Cast<AQLHUD>(PC->GetHUD());
	if (LocalHUD)
	{
		LocalHUD->BlinkBag();
	}
}

void UQLInventoryComponent::ClientRPCRollbackInventory_Implementation(EItemType InItemId, int32 ItemCnt)
{
	if (InventoryItem.Find(InItemId))
	{
		InventoryItem[InItemId] -= ItemCnt;
	}
}