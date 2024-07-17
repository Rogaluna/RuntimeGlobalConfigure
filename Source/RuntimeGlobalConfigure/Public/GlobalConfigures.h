// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ConfigNode.h"
#include "GlobalConfigures.generated.h"

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
    void Initialize();

private:
    static UGlobalConfigures* Instance;

    TMap<FString, FConfigNode> ConfigMappings;

    // 私有构造函数和析构函数
    UGlobalConfigures();
    ~UGlobalConfigures();

    void ParsingConfigurationFile(const FString& FileContent, const FString& Filename);

    // 成员变量
    bool bInitialized;
};
