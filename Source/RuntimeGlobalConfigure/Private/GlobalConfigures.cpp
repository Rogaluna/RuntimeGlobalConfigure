// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalConfigures.h"
#include "RuntimeGlobalConfigure.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "ConfigParser.h"
#include "Json.h"
#include "JsonUtilities.h"


UGlobalConfigures* UGlobalConfigures::Instance = nullptr;

UGlobalConfigures& UGlobalConfigures::Get()
{
    if (!Instance)
    {
        Instance = NewObject<UGlobalConfigures>();
    }
    return *Instance;
}

void UGlobalConfigures::Initialize()
{
    if (!bInitialized)
    {
        
    }
}

UGlobalConfigures::UGlobalConfigures()
    : bInitialized(false)
{
    Initialize();
}

UGlobalConfigures::~UGlobalConfigures()
{

}

void UGlobalConfigures::LoadConfigurationFile(const FString& FilePath)
{
    const FString& Filename = FPaths::GetBaseFilename(FilePath);

    UConfigNode* RootNode = ParseFileContent(FilePath);
    ConfigMappings.Add(Filename, RootNode);

    PrintConfigNode(RootNode);
}

UConfigNode* UGlobalConfigures::ParseFileContent(const FString& FilePath)
{
    const FString& FileExt = FPaths::GetExtension(FilePath);
    UConfigNode* RootNode = NewObject<UConfigNode>();
    if (FileExt == "json")
    {
        ParseJsonFile(FilePath, RootNode);
    }

    return RootNode;
}

void UGlobalConfigures::ParseJsonFile(const FString& FilePath, UConfigNode* RootNode)
{
    // 确保文件存在
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
    {
        UE_LOG(LogRuntimeGlobalConfigurePlugin, Error, TEXT("Can't Find (%s) File"), *FilePath);
    }
    else
    {
        FString FileContent;
        TSharedPtr<FJsonObject> JsonObject;
        if (FFileHelper::LoadFileToString(FileContent, *FilePath))
        {
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContent);
            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
                UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("JSON Object Parse Success: %s"), *FilePath);
                ParseJsonObject(JsonObject, RootNode);
            }
            else
            {
                UE_LOG(LogRuntimeGlobalConfigurePlugin, Error, TEXT("JSON Object Parse Fail: %s"), *FilePath);
            }
        }
        else
        {
            UE_LOG(LogRuntimeGlobalConfigurePlugin, Error, TEXT("JSON File Load Fail: %s"), *FilePath);
        }
    }
}

void UGlobalConfigures::ParseJsonObject(TSharedPtr<FJsonObject> JsonObject, UConfigNode* ParentNode)
{
    for (const auto& Pair : JsonObject->Values)
    {
        UConfigNode* ChildNode = NewObject<UConfigNode>();
        ChildNode->Name = Pair.Key;

        if (Pair.Value->Type == EJson::String)
        {
            ChildNode->Type = ENodeType::NT_String;
            ChildNode->Value = Pair.Value->AsString();
        }
        else if (Pair.Value->Type == EJson::Number)
        {
            double Number = Pair.Value->AsNumber();
            if (Number == static_cast<int32>(Number))
            {
                ChildNode->Type = ENodeType::NT_Int;
                ChildNode->Value = FString::FromInt(static_cast<int32>(Number));
            }
            else
            {
                ChildNode->Type = ENodeType::NT_Float;
                ChildNode->Value = FString::SanitizeFloat(static_cast<float>(Number));
            }
        }
        else if (Pair.Value->Type == EJson::Boolean)
        {
            ChildNode->Type = ENodeType::NT_Bool;
            ChildNode->Value = Pair.Value->AsBool() ? TEXT("true") : TEXT("false");
        }
        else if (Pair.Value->Type == EJson::Array)
        {
            ChildNode->Type = ENodeType::NT_Array;
            const TArray<TSharedPtr<FJsonValue>>& Array = Pair.Value->AsArray();
            for (const TSharedPtr<FJsonValue>& Item : Array)
            {
                UConfigNode* ArrayElementNode = NewObject<UConfigNode>();
                if (Item->Type == EJson::String)
                {
                    ArrayElementNode->Type = ENodeType::NT_String;
                    ArrayElementNode->Value = Item->AsString();
                }
                else if (Item->Type == EJson::Number)
                {
                    double Number = Item->AsNumber();
                    if (Number == static_cast<int32>(Number))
                    {
                        ArrayElementNode->Type = ENodeType::NT_Int;
                        ArrayElementNode->Value = FString::FromInt(static_cast<int32>(Number));
                    }
                    else
                    {
                        ArrayElementNode->Type = ENodeType::NT_Float;
                        ArrayElementNode->Value = FString::SanitizeFloat(static_cast<float>(Number));
                    }
                }
                else if (Item->Type == EJson::Boolean)
                {
                    ArrayElementNode->Type = ENodeType::NT_Bool;
                    ArrayElementNode->Value = Item->AsBool() ? TEXT("true") : TEXT("false");
                }
                else if (Item->Type == EJson::Object)
                {
                    ArrayElementNode->Type = ENodeType::NT_Object;
                    ParseJsonObject(Item->AsObject(), ArrayElementNode);
                }
                ChildNode->Children.Add(ArrayElementNode);
            }
        }
        else if (Pair.Value->Type == EJson::Object)
        {
            ChildNode->Type = ENodeType::NT_Object;
            ParseJsonObject(Pair.Value->AsObject(), ChildNode);
        }

        ParentNode->Children.Add(ChildNode);
    }
}

void UGlobalConfigures::PrintConfigNode(const UConfigNode* Node, int32 IndentLevel /*= 0*/)
{
    if (!Node)
    {
        return;
    }

    // 创建缩进字符串
    FString Indent = FString::ChrN(IndentLevel, TEXT(' '));

    // 打印节点的名称和值
    FString NodeInfo = FString::Printf(TEXT("%sName: %s, Type: %d, Value: %s"), *Indent, *Node->Name, static_cast<int32>(Node->Type), *Node->Value);
    UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("%s"), *NodeInfo);

    // 递归打印子节点
    for (const UConfigNode* ChildNode : Node->Children)
    {
        PrintConfigNode(ChildNode, IndentLevel + 2);  // 缩进子节点
    }
}

