// Universal TTS Plugin MeoPlay Copyright (C) 2020 MeoPlay <contact@meoplay.com> All Rights Reserved.


#pragma once


#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"

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
		static void TTSGetAllToken();

		UFUNCTION(BlueprintCallable, Category = "UniversalTTS")
		static bool TTSFindBestToken(FString Name);

		UFUNCTION(BlueprintCallable, Category = "UniversalTTS")
			static void TTSSpeechWindows(const FString& speech, const FString& Name,int speechRateWindows = 1);





};