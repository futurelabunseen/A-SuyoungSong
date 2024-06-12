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
protected:

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = Ready)
	uint8 bIsReady : 1;
};
