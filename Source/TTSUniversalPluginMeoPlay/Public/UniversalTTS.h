// Universal TTS Plugin MeoPlay Copyright (C) 2020 MeoPlay <contact@meoplay.com> All Rights Reserved.


#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/Texture2D.h"
#include "UniversalTTSType.h"

#if PLATFORM_ANDROID

#include <Android/AndroidApplication.h>
#include <Android/AndroidJNI.h>

#endif

#include "UniversalTTS.generated.h"


UCLASS()
class TTSUNIVERSALPLUGINMEOPLAY_API UUniversalTTS : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		
public:

		UFUNCTION(BlueprintCallable, Category = "UniversalTTS")
		static void TTSSpeech(const FString& speech, const FString& language = "default", float speechRateAndroid = 1.0f, float speechRateIOS = 0.3f, int speechRateWindows = 1);
				
		UFUNCTION(BlueprintCallable, Category = "UniversalTTS")
		static void TTSInitLib();

		UFUNCTION(BlueprintCallable, Category = "UniversalTTS")
		static bool TTSFindBestToken(FString Name);

		UFUNCTION(BlueprintCallable, Category = "UniversalTTS")
		static void SpeechToText(const FString& FileName,FString& Text);

};
//获取所有的Token
UCLASS()
class TTSUNIVERSALPLUGINMEOPLAY_API USpeakToken : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

		DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneStringResult,const TArray<FTokens>&,Tokens);

public:
	UPROPERTY(BlueprintAssignable)
		FOneStringResult OnSuccess;


	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Category = "UniversalTTS"))
		static USpeakToken* TTSGetAllToken(UObject* WorldContextObject) {
		USpeakToken* Node = NewObject<USpeakToken>(WorldContextObject);
		return Node;
	}

protected:
	virtual void Activate();
};

//转换成语音流数据
UCLASS()
class TTSUNIVERSALPLUGINMEOPLAY_API USpeakStream : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

		DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneStringResult,USoundWave*, SpeakStream);

public:
	UPROPERTY(BlueprintAssignable)
		FOneStringResult OnSuccess;
		FString Speech;
		FString Name;
		int SpeechRateWindows;


	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Category = "UniversalTTS"))
		static USpeakStream* TTSSpeechWindows(UObject* WorldContextObject,FString SpeechText,FString Names, int SpeechRate = 1) {
		USpeakStream* Node = NewObject<USpeakStream>(WorldContextObject);
		Node->Speech = SpeechText;
		Node->Name = Names;
		Node->SpeechRateWindows = SpeechRate;
		return Node;
	}

protected:
	virtual void Activate();
};