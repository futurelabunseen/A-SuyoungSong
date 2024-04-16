// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#define CUR_LINE ANSI_TO_TCHAR(__FUNCTION__)


#define LOG_GASREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), CurrentActorInfo->AvatarActor.Get()->GetOwner()->GetRemoteRole()))
#define LOG_GASLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), CurrentActorInfo->AvatarActor.Get()->GetOwner()->GetLocalRole()))

#define LOG_SUBREMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))
#define LOG_SUBLOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))


#define LOG_REMOTEROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))
#define LOG_LOCALROLEINFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))

//언리얼식 문자열로 변경해서 Printf로 넘겨주어야함.. 그렇기 때문에 Printf - TEXT는 단짝, Printf로 만들어주는 이유는 QL_LOG를 TEXT로 입력받기 때문에 FString 으로 변환해주어야한다.

#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 

//ability - net mode
#define LOG_GASNETMODEINFO ((CurrentActorInfo->AvatarActor.Get()->GetNetMode() == ENetMode::NM_Client)? *FString::Printf(TEXT("CLIENT")) : ((CurrentActorInfo->AvatarActor.Get()->GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 

#define QL_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] [%s / %s] %s %s"),LOG_NETMODEINFO,LOG_LOCALROLEINFO,LOG_REMOTEROLEINFO, CUR_LINE, *FString::Printf(Format,##__VA_ARGS__))

#define QL_GASLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] [%s / %s] %s %s"),LOG_GASNETMODEINFO,LOG_GASLOCALROLEINFO, LOG_GASREMOTEROLEINFO, CUR_LINE, *FString::Printf(Format,##__VA_ARGS__))

#define QL_SUBLOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] [%s / %s] %s %s"),LOG_NETMODEINFO,LOG_SUBLOCALROLEINFO, LOG_SUBREMOTEROLEINFO, CUR_LINE, *FString::Printf(Format,##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(QLLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(QLNetLog,Log,All);