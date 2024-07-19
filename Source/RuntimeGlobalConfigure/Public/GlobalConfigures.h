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
    /** 获取单例实例 */
    static UGlobalConfigures& Get();

    /** 初始化 */
    void Initialize();

public:
    UFUNCTION(BlueprintCallable, DisplayName="加载配置文件")
    void LoadConfigurationFile(const FString& FilePath);

    /**
     * 传入的参数必须是已经注册过的文件
     */
    UFUNCTION(BlueprintCallable, DisplayName="保存配置文件")
    bool WriteConfigNodeToFile(const FString& FilePath);

private:
    static UGlobalConfigures* Instance;

    TMap<FString, UConfigNode*> ConfigMappings;
    
    /** 私有构造函数和析构函数 */
    UGlobalConfigures();
    ~UGlobalConfigures();

    /**
     * 解析文件内容，返回解析的根节点
     */
    UConfigNode* ParseFileContent(const FString& FilePath);

    /**
     * 构造文件内容，返回构造的字符串
     */
    FString ConstructureFileContent(const UConfigNode* ConfigNode, const FString& FilePath);

    /** 解析JSON文件 */
    void ParseJsonString(const FString& FilePath, UConfigNode* RootNode);
    void ParseJsonObject(TSharedPtr<FJsonObject> JsonObject, UConfigNode* ParentNode);

    /** 构造JSON字符串 */
    void ConstructureJsonObject(const UConfigNode* Node, TSharedPtr<FJsonObject>& JsonObject);
    void ConstructureJsonString(const UConfigNode* Node, FString& JsonString);


    void PrintConfigNode(const UConfigNode* Node, int32 IndentLevel = 0);

private:
    /** 成员变量 */
    bool bInitialized;
};
