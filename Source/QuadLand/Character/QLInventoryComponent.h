// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "GameData/QLItemType.h"
#include "QLInventoryComponent.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLInventoryComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UQLInventoryComponent(const FObjectInitializer& ObjectInitializer);
	
	int GetInventoryCnt(EItemType ItemType);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, Meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, int32> InventoryItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, Meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, TObjectPtr<class UAnimMontage>> ItemMontage;

	uint8 bIsSetVisibleInventory : 1;

public:
	void AddItem(EItemType ItemId, int32 ItemCnt);
	void UseItem(EItemType ItemId); //아이템을 사용
	void AddInventoryByDraggedItem(EItemType ItemId, int32 ItemCnt);
	void AddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt);

public:

	UFUNCTION(NetMulticast,Unreliable)
	void MulticastRPCItemMotion(EItemType ItemId);

	//RPC Section - Remove
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerRPCRemoveItem(EItemType ItemId, int32 ItemCnt);

	UFUNCTION(Client, Reliable)
	void ClientRPCRemoveItem(class UQLItemData* Item, int32 ItemCnt);

public:
	//RPC Section - Add
	UFUNCTION(Server, Reliable)
	void ServerRPCAddInventoryByDraggedItem(EItemType ItemId, int32 ItemCnt);

	UFUNCTION(Server, Reliable)
	void ServerRPCAddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt);

	UFUNCTION(Client, Reliable)
	void ClientRPCAddItem(class UQLItemData* Item, int32 ItemCnt);


	UFUNCTION(Client, Reliable)
	void ClientRPCRollbackInventory(EItemType ItemId, int32 ItemCnt);

	friend class AQLCharacterPlayer;
};
