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
	TArray<TObjectPtr<class UQLGemData>> GemData; //나중엔 TMap을 사용해서 상위클래스 - 하위클래스로 두거나, 인터페이스로 묶어서 관리

};
