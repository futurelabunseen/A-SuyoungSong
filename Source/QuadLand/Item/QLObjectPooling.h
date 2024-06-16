// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QLObjectPooling.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class QUADLAND_API UQLObjectPooling : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQLObjectPooling();

	void InitializePool(class AQLBullet *Bullet);
	class AQLBullet* GetObject();
	void PutBackToPool(class AQLBullet* Bullet);
	bool IsPoolEmpty() { return PoolManager.Num() == 0; }
protected:

	UPROPERTY(EditAnywhere, Category = PoolMemory)
	TArray<TObjectPtr<class AQLBullet>> PoolManager;


};
