// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QLSelectionData.generated.h"

/**
 * 
 */
USTRUCT()
struct FQLGender {
	
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Mesh)
	TSubclassOf<class AQLCharacterPlayer> Pawn;
	
	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<class USoundCue> MoanSound;
	
	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<class USoundCue> JumpSound;
};

UCLASS()
class QUADLAND_API UQLSelectionData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	TSubclassOf<class AQLPlayerLifeStone> GetLifeStoneClass(int Idx);
	class UTexture2D* GetTexture(int Idx);

	UPROPERTY(EditAnywhere, Category = Type)
	TArray<FQLGender> Gender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Color)
	TArray<TObjectPtr<class UQLGemData>> GemData; //���߿� TMap�� ����ؼ� ����Ŭ���� - ����Ŭ������ �ΰų�, �������̽��� ��� ����

};
