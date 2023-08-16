// Universal TTS Plugin MeoPlay Copyright (C) 2020 MeoPlay <contact@meoplay.com> All Rights Reserved.



#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTTSUniversalPluginMeoPlayModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
