// Universal TTS Plugin MeoPlay Copyright (C) 2020 MeoPlay <contact@meoplay.com> All Rights Reserved.

#include "UniversalTTS.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Async/Async.h"

#if PLATFORM_ANDROID
#include <Android/AndroidApplication.h>
#include <Android/AndroidJNI.h>
#endif

#if PLATFORM_ANDROID
static JNIEnv* JavaEnv;
static jmethodID AndroidThunkJava_AndroidTTSInit;
static jmethodID AndroidThunkJava_AndroidTTSSpeech;
#endif

#if PLATFORM_IOS
#include "IOS/IOSAppDelegate.h"
#include "IOS/IOSView.h"
#include "IOS/IOSAppDelegate.h"
#import "AVFoundation/AVFoundation.h"
#endif



#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#pragma warning(push)
#pragma warning(disable: 4191)
#pragma warning(disable: 4996)
#ifndef DeleteFile
#define DeleteFile DeleteFileW
#endif
#ifndef MoveFile
#define MoveFile MoveFileW
#endif
#ifndef LoadString
#define LoadString LoadStringW
#endif
#ifndef InterlockedDecrement
#define InterlockedDecrement _InterlockedDecrement
#endif
#ifndef InterlockedIncrement
#define InterlockedIncrement _InterlockedIncrement
#endif
#ifndef GetMessage
#define GetMessage GetMessageW
#endif

#define _ATL_APARTMENT_THREADED
#include <atlbase.h>

extern CComModule _Module;
#include <atlcom.h>

#include <sapi.h>
#pragma warning (disable:4996)
#include <sphelper.h>

#include <codecvt>
#include <vector>
#undef DeleteFile
#undef MoveFile
#undef LoadString
#undef InterlockedDecrement
#undef InterlockedIncrement
#undef GetMessage
#pragma warning(pop)
#include "Windows/HideWindowsPlatformTypes.h"




#endif

#if PLATFORM_WINDOWS
std::wstring utf8_to_utf16(const std::string& utf8)
{
	std::vector<unsigned long> unicode;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned long uni;
		size_t todo;
		bool error = false;
		unsigned char ch = utf8[i++];
		if (ch <= 0x7F)
		{
			uni = ch;
			todo = 0;
		}
		else if (ch <= 0xBF)
		{
			throw std::logic_error("not a UTF-8 string");
		}
		else if (ch <= 0xDF)
		{
			uni = ch & 0x1F;
			todo = 1;
		}
		else if (ch <= 0xEF)
		{
			uni = ch & 0x0F;
			todo = 2;
		}
		else if (ch <= 0xF7)
		{
			uni = ch & 0x07;
			todo = 3;
		}
		else
		{
			throw std::logic_error("not a UTF-8 string");
		}
		for (size_t j = 0; j < todo; ++j)
		{
			if (i == utf8.size())
				throw std::logic_error("not a UTF-8 string");
			ch = utf8[i++];
			if (ch < 0x80 || ch > 0xBF)
				throw std::logic_error("not a UTF-8 string");
			uni <<= 6;
			uni += ch & 0x3F;
		}
		if (uni >= 0xD800 && uni <= 0xDFFF)
			throw std::logic_error("not a UTF-8 string");
		if (uni > 0x10FFFF)
			throw std::logic_error("not a UTF-8 string");
		unicode.push_back(uni);
	}
	std::wstring utf16;
	for (i = 0; i < unicode.size(); ++i)
	{
		unsigned long uni = unicode[i];
		if (uni <= 0xFFFF)
		{
			utf16 += (wchar_t)uni;
		}
		else
		{
			uni -= 0x10000;
			utf16 += (wchar_t)((uni >> 10) + 0xD800);
			utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
		}
	}
	return utf16;
}
#endif

void UUniversalTTS::TTSInitLib()
{
#if PLATFORM_ANDROID
	UE_LOG(LogTemp, Warning, TEXT("TTSInitLib"));


	JavaEnv = FAndroidApplication::GetJavaEnv();

	if (JavaEnv)
	{
		AndroidThunkJava_AndroidTTSInit = FJavaWrapper::FindMethod(JavaEnv, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_AndroidTTSInit", "()V", false);
		AndroidThunkJava_AndroidTTSSpeech = FJavaWrapper::FindMethod(JavaEnv, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_AndroidTTSSpeech", "(Ljava/lang/String;FLjava/lang/String;)V", false);
	}


	if (!AndroidThunkJava_AndroidTTSInit || !JavaEnv) {
		UE_LOG(LogTemp, Warning, TEXT("Error val TTSInitLib %d %d "), AndroidThunkJava_AndroidTTSInit == NULL, JavaEnv == NULL);
		return;
	}

	
	FJavaWrapper::CallVoidMethod(JavaEnv, FJavaWrapper::GameActivityThis, AndroidThunkJava_AndroidTTSInit);

#endif
}

void UUniversalTTS::TTSGetAllToken()
{
	TArray<FString> OutString;
#if PLATFORM_WINDOWS
	TFuture<void> Result = Async(EAsyncExecution::Thread, [=]() {
		CComPtr<ISpObjectToken>        cpVoiceToken;
		CComPtr<IEnumSpObjectTokens>   cpEnum;
		CComPtr<ISpVoice>              cpvoice;
		ULONG                          ulCount = 0;
		// Enumerate the installed voices.
		if (SUCCEEDED(SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum))) {
			// 依次获取每个token并朗读字符串
			while (SUCCEEDED(cpEnum->Next(1, &cpVoiceToken, NULL)) && cpVoiceToken != NULL) {
				CComPtr<ISpDataKey> attributes;
				if (S_OK == cpVoiceToken->OpenKey(L"Attributes", &attributes)) {

					WCHAR* languageWCHAR;
					WCHAR* NameWCHAR;
					if (S_OK == attributes->GetStringValue(L"Language", &languageWCHAR) && S_OK == attributes->GetStringValue(L"Name", &NameWCHAR)) {
						UE_LOG(LogTemp, Error, TEXT("语言是啥--->>>%s,名称是啥--->>>%s"), languageWCHAR, NameWCHAR);
					}
				}
			}
			cpEnum.Release();        // 释放pSpEnumTokens接口
		}
	});
#endif
}

bool UUniversalTTS::TTSFindBestToken(FString Name)
{
	FString TmpName;
	TmpName = L"Name=" + Name;
	CComPtr<ISpObjectToken> cpToken;
	if (SUCCEEDED(SpFindBestToken(SPCAT_VOICES, *TmpName,NULL, &cpToken )) || cpToken != NULL) {
		return true;
	}
	return false;
}

void UUniversalTTS::TTSSpeechWindows(const FString& speech, const FString& Name, int speechRateWindows)
{
	bool showDebugLog = false;
	FString speechlambda = speech;
	FString Namelambda = Name;
	Namelambda = L"Name=" + Name;
	TFuture<void> Result = Async(EAsyncExecution::Thread, [=]() {
		CComPtr<ISpObjectToken>        cpVoiceToken;
		CComPtr<IEnumSpObjectTokens>   cpEnum;
		CComPtr<ISpVoice>              cpvoice;
		ULONG                          ulCount = 0;

		HRESULT hr = S_OK;
		// Create the voice.
		hr = cpvoice.CoCreateInstance(CLSID_SpVoice);



		bool foundMatchingLanguage = false;

		CComPtr<ISpObjectToken> cpToken;
		if (!Namelambda.IsEmpty()) {
			if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("windows TTSSpeech using not default %s"), *Namelambda);

			SpFindBestToken(SPCAT_VOICES, *Namelambda, NULL, &cpToken);
			// SpFindBestToken(SPCAT_VOICES, L"", L"", &cpToken);
		}
		else {
			if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("windows TTSSpeech using default"));
			if (SUCCEEDED(hr))
			{
				// Enumerate the installed voices.
				hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
			}

			if (SUCCEEDED(hr))
			{
				// Get the number of voices.
				hr = cpEnum->GetCount(&ulCount);
			}

			//SpFindBestToken(SPCAT_VOICES, L"", L"", &cpToken);
			SpFindBestToken(SPCAT_VOICES, L"", L"", &cpToken);
			bool didFindTTS = false;

			while (SUCCEEDED(hr) && ulCount--)
			{
				cpVoiceToken.Release();

				if (SUCCEEDED(hr))
				{
					hr = cpEnum->Next(1, &cpVoiceToken, NULL);
					cpToken = cpVoiceToken;
				}


				WCHAR* description;
				if (S_OK == SpGetDescription(cpVoiceToken, &description))
				{
					UE_LOG(LogTemp, Warning, TEXT("Available voice: %s"), description);
				}
				else
				{
					return;
				}


				CComPtr<ISpDataKey> attributes;
				if (S_OK != cpVoiceToken->OpenKey(L"Attributes", &attributes))
					return;

				WCHAR* languageWCHAR;
				if (S_OK != attributes->GetStringValue(L"Language", &languageWCHAR))
					return;

				LCID lcidlanguage = wcstol(languageWCHAR, NULL, 16);
				WCHAR tmpwchar[LOCALE_NAME_MAX_LENGTH * 3];
				LPWSTR languagelocale = &(tmpwchar[0]);

				LCIDToLocaleName(lcidlanguage, languagelocale, LOCALE_NAME_MAX_LENGTH, LOCALE_ALLOW_NEUTRAL_NAMES);
				FString languagestrhex(languageWCHAR);
				FString languagestrlocale(languagelocale);


				if (Name == languagestrlocale) {
					if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("windows TTSSpeech found matching language break loop %s "), *languagestrlocale);
					didFindTTS = true;
					break;
				}
			}
			// default voice if no matching found
			if (didFindTTS == false) {
					SpFindBestToken(SPCAT_VOICES, L"", L"", &cpToken);
			}
		}


		if (cpvoice == NULL || cpToken == NULL) {
			if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("windows TTSSpeech cpvoice or cpToken null"));
			return;
		}

		cpvoice->SetVoice(cpToken);
		char* text = TCHAR_TO_UTF8(*speechlambda);

		//UTF-8 to UTF-16 
		std::string sourceutf8(text);
		//std::wstring_convert<std::codecvt_utf8_utf16<__int16>, __int16> convert;
		//std::u16string dest = convert.from_bytes(sourceutf8);

		//std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
		//auto p = reinterpret_cast<const char *>(sourceutf8.data());
		//wide_string dest = convert.from_bytes(p, p + sourceutf8.size());

		std::wstring utf16str = utf8_to_utf16(sourceutf8);

		if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("char16_t size %d"), sizeof(char16_t));


		wchar_t* wtext = (wchar_t*)malloc(sizeof(wchar_t) * utf16str.size() * 2 + 10);//(wchar_t*)malloc(sizeof(wchar_t) * strlen(text) * 2 + 10
		memset(wtext, 0, sizeof(wchar_t) * utf16str.size() * 2 + 10);
		memcpy(wtext, utf16str.data(), sizeof(wchar_t) * utf16str.size());
		//size_t outSize;
		//mbstowcs_s(&outSize, wtext, strlen(text) * 2 + 10, text, strlen(text) + 1);
		LPWSTR ptr = wtext;

		cpvoice->SetRate(speechRateWindows);
		hr = cpvoice->Speak(ptr, SPF_DEFAULT, NULL);
		free(wtext);
		cpToken.Release();

		});

}

void UUniversalTTS::TTSSpeech(const FString& speech, const FString& languageChoosed, float speechRateAndroid, float speechRateIOS, int speechRateWindows)
{
	bool showDebugLog = false;
#if PLATFORM_WINDOWS

	FString speechlambda = speech;
	FString languagelambda = languageChoosed;
	TFuture<void> Result = Async(EAsyncExecution::Thread, [=]() {
		CComPtr<ISpObjectToken>        cpVoiceToken;
		CComPtr<IEnumSpObjectTokens>   cpEnum;
		CComPtr<ISpVoice>              cpvoice;
		ULONG                          ulCount = 0;

		HRESULT hr = S_OK;
		// Create the voice.
		hr = cpvoice.CoCreateInstance(CLSID_SpVoice);

	

		bool foundMatchingLanguage = false;

		CComPtr<ISpObjectToken> cpToken;
		if (languagelambda == "default") {
			if(showDebugLog) UE_LOG(LogTemp, Warning, TEXT("windows TTSSpeech using default"));
			SpFindBestToken(SPCAT_VOICES, L"", L"Name=MicrosoftKangkang", &cpToken);
			// SpFindBestToken(SPCAT_VOICES, L"", L"", &cpToken);
		}
		else {
			if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("windows TTSSpeech using not default %s"), *languagelambda);
			
			if (SUCCEEDED(hr))
			{
				// Enumerate the installed voices.
				hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);
			}

			if (SUCCEEDED(hr))
			{
				// Get the number of voices.
				hr = cpEnum->GetCount(&ulCount);
			}
			
			//SpFindBestToken(SPCAT_VOICES, L"", L"", &cpToken);
			SpFindBestToken(SPCAT_VOICES, L"", L"", &cpToken);
			bool didFindTTS = false;

			while (SUCCEEDED(hr) && ulCount--)
			{
				cpVoiceToken.Release();

				if (SUCCEEDED(hr))
				{
					hr = cpEnum->Next(1, &cpVoiceToken, NULL);
					cpToken = cpVoiceToken;
				}
		

				WCHAR *description;
				if (S_OK == SpGetDescription(cpVoiceToken, &description))
				{
					UE_LOG(LogTemp, Warning, TEXT("Available voice: %s"), description);
				}
				else
				{
					return ;
				}


				CComPtr<ISpDataKey> attributes;
				if (S_OK != cpVoiceToken->OpenKey(L"Attributes", &attributes))
					return ;

				WCHAR *languageWCHAR;
				if (S_OK != attributes->GetStringValue(L"Language", &languageWCHAR))
					return ;
				
				LCID lcidlanguage = wcstol(languageWCHAR, NULL, 16);
				WCHAR tmpwchar[LOCALE_NAME_MAX_LENGTH * 3];
				LPWSTR languagelocale = &(tmpwchar[0]);
				
				LCIDToLocaleName(lcidlanguage, languagelocale, LOCALE_NAME_MAX_LENGTH, LOCALE_ALLOW_NEUTRAL_NAMES);
				FString languagestrhex(languageWCHAR);
				FString languagestrlocale(languagelocale);
				

				if (languageChoosed == languagestrlocale) {
					if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("windows TTSSpeech found matching language break loop %s "),*languagestrlocale);
					didFindTTS = true;
					break;
				}
			}
			// default voice if no matching found
			if (didFindTTS == false) {
			//	SpFindBestToken(SPCAT_VOICES, L"language=804", L"Name=Microsoft Kangkang", &cpToken);

				// SpFindBestToken(SPCAT_VOICES, L"", L"", &cpToken);
			}
		}

	
		if (cpvoice == NULL || cpToken == NULL) {
			if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("windows TTSSpeech cpvoice or cpToken null"));
			return;
		}

		cpvoice->SetVoice(cpToken);
		char *text = TCHAR_TO_UTF8(*speechlambda);
		
		//UTF-8 to UTF-16 
		std::string sourceutf8(text);
		//std::wstring_convert<std::codecvt_utf8_utf16<__int16>, __int16> convert;
		//std::u16string dest = convert.from_bytes(sourceutf8);
		
		//std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
		//auto p = reinterpret_cast<const char *>(sourceutf8.data());
		//wide_string dest = convert.from_bytes(p, p + sourceutf8.size());

		std::wstring utf16str = utf8_to_utf16(sourceutf8);

		if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("char16_t size %d"), sizeof(char16_t));
	

		wchar_t *wtext = (wchar_t*)malloc(sizeof(wchar_t) * utf16str.size() * 2 + 10);//(wchar_t*)malloc(sizeof(wchar_t) * strlen(text) * 2 + 10
		memset(wtext, 0, sizeof(wchar_t) * utf16str.size() * 2 + 10);
		memcpy(wtext, utf16str.data(), sizeof(wchar_t) * utf16str.size());
		//size_t outSize;
		//mbstowcs_s(&outSize, wtext, strlen(text) * 2 + 10, text, strlen(text) + 1);
		LPWSTR ptr = wtext;

		cpvoice->SetRate(speechRateWindows);
		hr = cpvoice->Speak(ptr, SPF_DEFAULT, NULL);
		free(wtext);
		cpToken.Release();
	
	});
	
#endif
#if PLATFORM_IOS

		AVSpeechSynthesizer *synthesizer = [[AVSpeechSynthesizer alloc] init];
		AVSpeechUtterance *utterance = [[AVSpeechUtterance alloc] initWithString:speech.GetNSString()];
		utterance.rate = speechRateIOS;
		
		if (languageChoosed.Equals(TEXT("default"), ESearchCase::CaseSensitive)) {
			utterance.voice = [AVSpeechSynthesisVoice voiceWithLanguage: languageChoosed.GetNSString()];
		}

		[synthesizer speakUtterance : utterance];
		synthesizer = nil;
		
#endif

#if PLATFORM_ANDROID
	if (!AndroidThunkJava_AndroidTTSSpeech || !JavaEnv) {
		if (showDebugLog) UE_LOG(LogTemp, Warning, TEXT("Error val TTSSpeech %d %d "), AndroidThunkJava_AndroidTTSInit == NULL, JavaEnv == NULL);
		return;
	}
	
	jstring local = JavaEnv->NewStringUTF(TCHAR_TO_UTF8(*speech));
	jstring result = (jstring)JavaEnv->NewGlobalRef(local);
	JavaEnv->DeleteLocalRef(local);

	jfloat jnispeechRateAndroid = speechRateAndroid;

	jstring languagejni = JavaEnv->NewStringUTF(TCHAR_TO_UTF8(*languageChoosed));
	jstring result2 = (jstring)JavaEnv->NewGlobalRef(languagejni);
	JavaEnv->DeleteLocalRef(languagejni);

	FJavaWrapper::CallVoidMethod(JavaEnv, FJavaWrapper::GameActivityThis, AndroidThunkJava_AndroidTTSSpeech, result, jnispeechRateAndroid, result2);
	
#endif


}
