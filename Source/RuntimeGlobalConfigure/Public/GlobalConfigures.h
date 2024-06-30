// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ConfigNode.h"
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

    /**
     * 根节点的第一层装载了文件配置的根，其Name属性是它的文件名
     */
    UFUNCTION(BlueprintCallable, Category = "全局配置实例", DisplayName = "获取加载的配置文件名")
    TArray<FString> GetFirstLayerChildNames() const; 

    /**
     * 通过路径寻找配置文件，路径字符串起始必须为“/”
     * 当寻找到数组和对象类型的配置时，无法返回值
     */
    UFUNCTION(BlueprintCallable, Category = "全局配置实例", DisplayName = "获取配置值")
    bool FindConfigNodeByPath(const FString& Path, int32& OutNodeType, FString& OutNodeValue) const;

private:
    static UGlobalConfigures* Instance;

    FConfigNode RootNode;

    // 私有构造函数和析构函数
    UGlobalConfigures();
    ~UGlobalConfigures();

    void ParsingConfigurationFile(const FString& FileContent, const FString& Filename);

    // 成员变量
    bool bInitialized;
};
