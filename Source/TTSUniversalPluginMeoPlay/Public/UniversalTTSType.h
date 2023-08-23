// Copyright 2022 Dreamingpoet All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UniversalTTSType.generated.h"


/*TokenFrom SAPI*/
USTRUCT(BlueprintType)
struct TTSUNIVERSALPLUGINMEOPLAY_API FTokens
{
	GENERATED_USTRUCT_BODY()

	/*获取所有的Tokens*/
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Language;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

		void AddToken(FString language, FString name){
		Language = language;
		Name = name;
	}

};