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
	UQLObjectPooling();

	void Init(AActor *InActor);
	class AActor* GetObject();
	void PutBackToPool(class AActor* InActor);
	bool IsPoolEmpty() { return PoolManager.Num() == 0; }
protected:

	UPROPERTY(EditAnywhere, Category = PoolMemory)
	TArray<TObjectPtr<class AActor>> PoolManager;


};
