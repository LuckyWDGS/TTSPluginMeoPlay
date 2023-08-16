// Universal TTS Plugin MeoPlay Copyright (C) 2020 MeoPlay <contact@meoplay.com> All Rights Reserved.


using System;
using System.IO;
using UnrealBuildTool;
using System.Collections.Generic;
//using Tools.DotNETCommon;

public class TTSUniversalPluginMeoPlay : ModuleRules
{
    public TTSUniversalPluginMeoPlay(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
			}
            );




        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );


        if (/*Target.Platform == UnrealTargetPlatform.Win32 ||*/ Target.Platform == UnrealTargetPlatform.Win64)
        {
            PrivateIncludePaths.AddRange(
            new string[] {
                "../ThirdParty/atlmfc/include"
            }
            );
        }

        /*if (Target.Platform == UnrealTargetPlatform.Win32)
        {
            string ThirdParty = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdParty, "atlmfc/lib/x86/atls.lib"));
        }*/
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string ThirdParty = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdParty, "atlmfc/lib/x64/atls.lib"));
        }

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicDependencyModuleNames.AddRange(new string[] { "Launch", "ApplicationCore" });
            string BuildPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(BuildPath, "MeoPlayUniversalTTS_AndroidAPL.xml"));
        }

        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            PrivateDependencyModuleNames.Add("IOSRuntimeSettings");

            PublicFrameworks.Add("AVFoundation");

            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("IOSPlugin", Path.Combine(PluginPath, "MeoPlayUniversalTTS_IOSAPL.xml"));
        }
    }
}
