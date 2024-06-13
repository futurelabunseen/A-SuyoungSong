// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/QLGameInstance.h"
#include "GameData/QLDataManager.h"
#include "EnhancedInputSubsystems.h"

UQLGameInstance::UQLGameInstance()
{
	DataManager = CreateDefaultSubobject<UQLDataManager>(TEXT("DataManager"));
}

void UQLGameInstance::Init()
{
	Super::Init();

}
