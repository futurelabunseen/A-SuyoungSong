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
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void StartPlay() override;

	virtual void SpawnAI() override;
private:

	void GameStart();

	//�������� GameMode ���� �����ؼ� ���� �ð��� �����Ѵ�.
	void GameEnd();

	UPROPERTY(EditAnywhere, Category = Classes)
	TSubclassOf<class AQLAISpawner> AISpawnerClass;
};
