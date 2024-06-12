// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "QLLobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	int GetGenderType() { return Gender; }
	void SetGenderType(int InGender) { Gender = InGender; }
	int GetGemType() { return Gem; }
	void SetGemType(int InGem) { Gem = InGem; }
protected:

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Type, Meta = (AllowPrivateAccess = "true"))
	int Gender;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = Type, Meta = (AllowPrivateAccess = "true"))
	int Gem;


protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;


};
