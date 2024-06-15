// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QLSelectionData.h"
#include "Engine/Texture2D.h"
#include "GameData/QLGemData.h"
#include "Player/QLPlayerLifeStone.h"

TSubclassOf<class AQLPlayerLifeStone> UQLSelectionData::GetLifeStoneClass(int Idx)
{
    return GemData[Idx]->GetLifeStoneClass();
}

UTexture2D* UQLSelectionData::GetTexture(int Idx)
{
    return  GemData[Idx]->GetTexture();
}
