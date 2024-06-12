// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLLobbyPlayerState.h"
#include "Player/QLPlayerState.h"

void AQLLobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	UE_LOG(LogTemp, Log, TEXT("1"));
	if (PlayerState)
	{

		UE_LOG(LogTemp, Log, TEXT("2"));
		AQLPlayerState* PS = Cast<AQLPlayerState>(PlayerState);

		if (PS)
		{
			UE_LOG(LogTemp, Log, TEXT("3"));
			PS->SetType(Gender, Gem);
		}
	}
}
