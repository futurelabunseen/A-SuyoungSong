// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QLSelectionData.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInterface.h"
#include "GameData/QLGemData.h"

UMaterialInterface* UQLSelectionData::GetMaterial(int Idx)
{
    return GemData[Idx]->GetMaterial();
}

UTexture2D* UQLSelectionData::GetTexture(int Idx)
{
    return  GemData[Idx]->GetTexture();
}
