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
	
	virtual void BeginPlay() override;
	int GetInventoryCnt(EItemType ItemType);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, Meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, int32> InventoryItem; //Ÿ��, ���� 
	//TArray < struct- Type, int, UI ��ġ >

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item, Meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, TObjectPtr<class UAnimMontage>> ItemMontage;
	uint8 bIsSetVisibleInventory : 1;

protected:
	//�ֺ� �������� ������ ����Ÿ��
	TMap<EItemType, TArray<TObjectPtr<AActor>>> NearbyItems; //�ֺ� �������� �����ϱ� ���� ���� Ÿ��.

	//Inventory Overlap
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);


	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	void AddItem(EItemType ItemId, int32 ItemCnt);
	void UseItem(EItemType ItemId); //�������� ���
	void AddInventoryByDraggedItem(EItemType ItemId);
	void AddGroundByDraggedItem(EItemType ItemId,int32 ItemCnt);

public:

	UFUNCTION(NetMulticast,Unreliable)
	void MulticastRPCItemMotion(EItemType ItemId);

	//RPC Section - Remove
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerRPCRemoveItem(EItemType ItemId, int32 ItemCnt);

	UFUNCTION(Client, Reliable)
	void ClientRPCRemoveItem(EItemType ItemId, int32 ItemCnt);

public:
	//RPC Section - Add
	UFUNCTION(Server, Reliable)
	void ServerRPCAddInventoryByDraggedItem(EItemType ItemId);

	UFUNCTION(Server, Reliable)
	void ServerRPCAddGroundByDraggedItem(EItemType ItemId, int32 ItemCnt);

	UFUNCTION(Client, Reliable)
	void ClientRPCAddItem(EItemType ItemId, int32 ItemCnt);


	UFUNCTION(Client, Reliable)
	void ClientRPCRollbackInventory(EItemType ItemId, int32 ItemCnt);

	friend class AQLCharacterPlayer;
};
