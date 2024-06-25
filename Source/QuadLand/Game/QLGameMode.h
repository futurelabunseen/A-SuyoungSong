// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Interface/QLAISpawnerInterface.h"
#include "QLGameMode.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLGameMode : public AGameMode , public IQLAISpawnerInterface
{
	GENERATED_BODY()
	
public:
	AQLGameMode();

	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void SpawnAI() override;

	bool GetPlayerState(FName Name) { return PlayerDieStatus[Name]; }

	UFUNCTION()
	void Dead(FName NonPlayerName);

	UFUNCTION()
	void GetWinner(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
	void Win();

	UFUNCTION()
	void AddPlayer(FName PlayerName);

	UFUNCTION()
	void SpawnPlayerPawn(APlayerController* Contoller,int Type);

protected:
	UPROPERTY(BlueprintReadOnly)
	int32 LivePlayerCount;

	class UAbilitySystemComponent* WinActorASC;
private:

	void GameStart();

	//질때마다 GameMode 에게 전달해서 죽은 시간을 측정한다.
	void GameEnd();

	UPROPERTY(EditAnywhere, Category = Classes)
	TSubclassOf<class AQLAISpawner> AISpawnerClass;

	UPROPERTY()
	TMap<FName, bool> PlayerDieStatus;


};
