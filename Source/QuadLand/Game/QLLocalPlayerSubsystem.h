// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "QLLocalPlayerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	void SetNickname(FString InNickname) { TmpNickname = InNickname; }
	FString GetNickname() { return TmpNickname; }
protected:

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Nickname, Meta = (AllowPrivateAccess = "true"))
	FString TmpNickname;
};
