// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#define CUR_LINE ANSI_TO_TCHAR(__FUNCTION__)
//언리얼식 문자열로 변경해서 Printf로 넘겨주어야함.. 그렇기 때문에 Printf - TEXT는 단짝, Printf로 만들어주는 이유는 QL_LOG를 TEXT로 입력받기 때문에 FString 으로 변환해주어야한다.
#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client)? *FString::Printf(TEXT("Client")) : ((GetNetMode() == ENetMode::NM_Standalone)? *FString::Printf(TEXT("Standalone")) : *FString::Printf(TEXT("Listen"))))
#define QL_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s/%s] %s"), LOG_NETMODEINFO,CUR_LINE, *FString::Printf(Format,##__VA_ARGS__))	

DECLARE_LOG_CATEGORY_EXTERN(QLLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(QLNetLog,Log,All);