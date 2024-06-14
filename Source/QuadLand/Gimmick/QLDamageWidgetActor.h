// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QLDamageWidgetActor.generated.h"

UCLASS()
class QUADLAND_API AQLDamageWidgetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQLDamageWidgetActor();
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Nickname, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UQLAttackDamageComponent> DamageWidgetComponent;
};
