// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "QLLobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AQLLobbyPlayerController();

	void SetIsReady(bool InReady) { bIsReady = InReady; }
	bool GetIsReady() { return bIsReady; }

	UFUNCTION(Server,Reliable)
	void ServerRPCReady(bool InReady);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCTimeout();
	
	UFUNCTION(BlueprintCallable)
	void SettingMesh(int Idx);


	UFUNCTION(Client,Reliable)

	void ClientRPCActivateTimeLimit();

	UFUNCTION(Client, Reliable)

	void ClientRPCShowWaitUI();

protected:

	virtual void BeginPlay() override;

	void CreateHUD();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TObjectPtr<class UUserWidget> HUD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	TArray<TObjectPtr<class USkeletalMesh>> Meshs;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Ready)
	uint8 bIsReady : 1;

};
