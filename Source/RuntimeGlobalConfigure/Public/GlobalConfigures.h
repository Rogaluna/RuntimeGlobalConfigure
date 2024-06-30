// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GlobalConfigures.generated.h"

struct PluginConfigs
{
    FString ConfigDir;
    TArray<FString> ConfigFiles;
    TArray<FString> SupportedFileTypes;

    PluginConfigs(const FString& InConfigDir, const TArray<FString>& InConfigFiles, const TArray<FString>& InSupportedFileTypes)
        : ConfigDir(InConfigDir), ConfigFiles(InConfigFiles), SupportedFileTypes(InSupportedFileTypes)
    {
    }
};

/**
 * 
 */
UCLASS()
class RUNTIMEGLOBALCONFIGURE_API UGlobalConfigures : public UObject
{
	GENERATED_BODY()
	
public:
    // 获取单例实例
    static UGlobalConfigures& Get();

    // 初始化
    void Initialize(const PluginConfigs& initConf);

private:
    static UGlobalConfigures* Instance;

    class FConfigNode RootNode;

    // 私有构造函数和析构函数
    UGlobalConfigures();
    ~UGlobalConfigures();

    void ParsingConfigurationFile(const FString& FileContent, const FString& Filename);

    // 成员变量
    bool bInitialized;
};
