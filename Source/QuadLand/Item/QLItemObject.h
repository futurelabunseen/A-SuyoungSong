// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLItemObject.generated.h"

UENUM()
enum class EItemType
{
	Weapon,
	Potion,
	None
};

UCLASS()
class QUADLAND_API AQLItemObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLItemObject();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Type)
	EItemType ItemType;

};
