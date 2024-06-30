// Copyright Epic Games, Inc. All Rights Reserved.

#include "RuntimeGlobalConfigure.h"
#include "GlobalConfigures.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FRuntimeGlobalConfigureModule"

DEFINE_LOG_CATEGORY(LogRuntimeGlobalConfigurePlugin);

void FRuntimeGlobalConfigureModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    FString ConfigFilePath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("RuntimeGlobalConfigure/Config/PluginConfig.ini"));
    if (FPaths::FileExists(ConfigFilePath))
    {
        //===============================================================================================//

        FString ConfigDir;
        if (GConfig->GetString(TEXT("UserSettings"), TEXT("ConfigDir"), ConfigDir, ConfigFilePath))
        {
            UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("ConfigDir: %s"), *ConfigDir);
        }

        TArray<FString> ConfigFiles;
        if (GConfig->GetArray(TEXT("UserSettings"), TEXT("ConfigFiles"), ConfigFiles, ConfigFilePath))
        {
            for (const FString& ConfigFile : ConfigFiles)
            {
                UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("OnlyIncludedConfigFile: %s"), *ConfigFile);
            }
        }
        
        TArray<FString> SupportedFileTypes;
        if (GConfig->GetArray(TEXT("PluginSettings"), TEXT("SupportedFileTypes"), SupportedFileTypes, ConfigFilePath))
        {
            for (const FString& FileType : SupportedFileTypes)
            {
                UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("SupportedFileType: %s"), *FileType);
            }
        }

        //===============================================================================================//

        UGlobalConfigures::Get().Initialize(PluginConfigs(ConfigDir, ConfigFiles, SupportedFileTypes));
    }
    else
    {
        UE_LOG(LogRuntimeGlobalConfigurePlugin, Error, TEXT("Config file not found: %s"), *ConfigFilePath);
    }
}

void FRuntimeGlobalConfigureModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRuntimeGlobalConfigureModule, RuntimeGlobalConfigure)