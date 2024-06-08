// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "QLLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AQLLobbyGameMode();
	void SetNickname(FString InNickname) { TmpNickname = InNickname; }
	FString GetNickname() { return TmpNickname; }
protected:

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Nickname, Meta = (AllowPrivateAccess = "true"))
	FString TmpNickname;

};
