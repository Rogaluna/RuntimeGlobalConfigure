// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalConfigures.h"
#include "RuntimeGlobalConfigure.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "ConfigParser.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"


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
    UConfigNode* RootNode = ParseFileContent(FilePath);
    ConfigMappings.Add(FilePath, RootNode);

    PrintConfigNode(RootNode);
}

bool UGlobalConfigures::WriteConfigNodeToFile(const FString& FilePath)
{
    const UConfigNode* RootNode = ConfigMappings.FindRef(FilePath);
    if (!RootNode)
    {
        return false;
    }

    // 获取文件内容
    const FString& FileContent = ConstructureFileContent(RootNode, FilePath);

    // 将内容字符串写入文件
    // 获取平台文件系统
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    // 检查文件夹是否存在，如果不存在则创建
    FString Directory = FPaths::GetPath(FilePath);
    if (!PlatformFile.DirectoryExists(*Directory))
    {
        PlatformFile.CreateDirectoryTree(*Directory);
    }

    // 将字符串写入文件
    if (FFileHelper::SaveStringToFile(FileContent, *FilePath))
    {
        UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("File Content Has Been Updated: %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogRuntimeGlobalConfigurePlugin, Error, TEXT("Can't Update File Content: %s"), *FilePath);
    }


    return false;
}

UConfigNode* UGlobalConfigures::ParseFileContent(const FString& FilePath)
{
    const FString& FileExt = FPaths::GetExtension(FilePath);
    UConfigNode* RootNode = NewObject<UConfigNode>();
    if (FileExt == "json")
    {
        ParseJsonString(FilePath, RootNode);
    }

    return RootNode;
}

FString UGlobalConfigures::ConstructureFileContent(const UConfigNode* ConfigNode, const FString& FilePath)
{
    const FString& FileExt = FPaths::GetExtension(FilePath);
    FString FileContent = "";
    if (FileExt == "json")
    {
        ConstructureJsonString(ConfigNode, FileContent);
    }

    return FileContent;
}

void UGlobalConfigures::ParseJsonString(const FString& FilePath, UConfigNode* RootNode)
{
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

void UGlobalConfigures::ConstructureJsonObject(const UConfigNode* Node, TSharedPtr<FJsonObject>& JsonObject)
{
    JsonObject = MakeShared<FJsonObject>();

    for (const UConfigNode* ChildNode : Node->Children)
    {
        if (ChildNode->Type == ENodeType::NT_Object)
        {
            TSharedPtr<FJsonObject> ChildJsonObject;
            ConstructureJsonObject(ChildNode, ChildJsonObject);
            JsonObject->SetObjectField(ChildNode->Name, ChildJsonObject);
        }
        else if (ChildNode->Type == ENodeType::NT_Array)
        {
            TArray<TSharedPtr<FJsonValue>> JsonArray;
            for (const UConfigNode* ArrayElementNode : ChildNode->Children)
            {
                if (ArrayElementNode->Type == ENodeType::NT_Object)
                {
                    TSharedPtr<FJsonObject> ElementJsonObject;
                    ConstructureJsonObject(ArrayElementNode, ElementJsonObject);
                    JsonArray.Add(MakeShared<FJsonValueObject>(ElementJsonObject));
                }
                else if (ArrayElementNode->Type == ENodeType::NT_Array)
                {
                    // 如果嵌套数组的话，可以递归处理
                    TSharedPtr<FJsonObject> ElementJsonObject;
                    ConstructureJsonObject(ArrayElementNode, ElementJsonObject);
                    JsonArray.Add(MakeShared<FJsonValueObject>(ElementJsonObject));
                }
                else
                {
                    switch (ArrayElementNode->Type)
                    {
                    case ENodeType::NT_String:
                        JsonArray.Add(MakeShared<FJsonValueString>(ArrayElementNode->Value));
                        break;
                    case ENodeType::NT_Int:
                        JsonArray.Add(MakeShared<FJsonValueNumber>(FCString::Atoi(*ArrayElementNode->Value)));
                        break;
                    case ENodeType::NT_Float:
                        JsonArray.Add(MakeShared<FJsonValueNumber>(FCString::Atof(*ArrayElementNode->Value)));
                        break;
                    case ENodeType::NT_Bool:
                        JsonArray.Add(MakeShared<FJsonValueBoolean>(ArrayElementNode->Value.ToLower() == TEXT("true")));
                        break;
                    default:
                        JsonArray.Add(MakeShared<FJsonValueNull>());
                        break;
                    }
                }
            }
            JsonObject->SetArrayField(ChildNode->Name, JsonArray);
        }
        else
        {
            switch (ChildNode->Type)
            {
            case ENodeType::NT_String:
                JsonObject->SetStringField(ChildNode->Name, ChildNode->Value);
                break;
            case ENodeType::NT_Int:
                JsonObject->SetNumberField(ChildNode->Name, FCString::Atoi(*ChildNode->Value));
                break;
            case ENodeType::NT_Float:
                JsonObject->SetNumberField(ChildNode->Name, FCString::Atof(*ChildNode->Value));
                break;
            case ENodeType::NT_Bool:
                JsonObject->SetBoolField(ChildNode->Name, ChildNode->Value.ToLower() == TEXT("true"));
                break;
            default:
                JsonObject->SetField(ChildNode->Name, MakeShared<FJsonValueNull>());
                break;
            }
        }
    }
}

void UGlobalConfigures::ConstructureJsonString(const UConfigNode* Node, FString& JsonString)
{
    TSharedPtr<FJsonObject> JsonObject;
    // 构造Json值
    ConstructureJsonObject(Node, JsonObject);
    if (JsonObject.IsValid())
    {
        // 序列化为字符串
        TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
        if (FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer))
        {
            Writer->Close();
        }
    }
}

void UGlobalConfigures::PrintConfigNode(const UConfigNode* Node, int32 IndentLevel /*= 0*/)
{
    if (!Node)
    {
        return;
    }

    FString Indent = FString::ChrN(IndentLevel, TEXT(' '));

    FString NodeInfo = FString::Printf(TEXT("%sName: %s, Type: %d, Value: %s"), *Indent, *Node->Name, static_cast<int32>(Node->Type), *Node->Value);
    UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("%s"), *NodeInfo);

    for (const UConfigNode* ChildNode : Node->Children)
    {
        PrintConfigNode(ChildNode, IndentLevel + 2);
    }
}

