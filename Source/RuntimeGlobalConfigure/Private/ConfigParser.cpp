// Fill out your copyright notice in the Description page of Project Settings.


#include "ConfigParser.h"
#include "RuntimeGlobalConfigure.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "ConfigNode.h"


ConfigParser::ConfigParser()
{
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::TraverseConfigNode(const FConfigNode& Node, FNodeCallback Callback, int32 Depth /*= 0*/)
{
    Callback.ExecuteIfBound(Node, Depth);

    // 遍历子节点
    for (const FConfigNode& Child : Node.Children)
    {
        TraverseConfigNode(Child, Callback, Depth + 1);
    }
}

void ConfigParser::ParseJson(const FString& JsonString, FConfigNode& ParentNode)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        for (const auto& Pair : JsonObject->Values)
        {
            FConfigNode ChildNode(FString(Pair.Key), ENodeType::NT_String, TEXT(""));
            switch (Pair.Value->Type)
            {
            case EJson::String:
                ChildNode.NodeType = ENodeType::NT_String;
                ChildNode.Value = Pair.Value->AsString();
                break;
            case EJson::Number:
                ChildNode.NodeType = ENodeType::NT_Number;
                ChildNode.Value = FString::Printf(TEXT("%f"), Pair.Value->AsNumber());
                break;
            case EJson::Boolean:
                ChildNode.NodeType = ENodeType::NT_Boolean;
                ChildNode.Value = Pair.Value->AsBool() ? TEXT("true") : TEXT("false");
                break;
            case EJson::Object:
                ChildNode.NodeType = ENodeType::NT_Object;
                ParseJsonObject(Pair.Value->AsObject(), ChildNode);
                break;
            case EJson::Array:
                ChildNode.NodeType = ENodeType::NT_Array;
                ParseJsonArray(Pair.Value->AsArray(), ChildNode);
                break;
            default:
                break;
            }
            ParentNode.Children.Add(ChildNode);
        }
    }
}

void ConfigParser::ParseIni(const FString& IniString, FConfigNode& ParentNode)
{
    TArray<FString> Lines;
    IniString.ParseIntoArrayLines(Lines);

    FConfigNode* CurrentSection = &ParentNode;

    for (const FString& Line : Lines)
    {
        FString TrimmedLine = Line.TrimStartAndEnd();
        if (TrimmedLine.StartsWith(TEXT("[")) && TrimmedLine.EndsWith(TEXT("]")))
        {
            FString SectionName = TrimmedLine.Mid(1, TrimmedLine.Len() - 2);
            FConfigNode SectionNode(SectionName, ENodeType::NT_Object, TEXT(""));
            ParentNode.Children.Add(SectionNode);
            CurrentSection = &ParentNode.Children.Last();
        }
        else if (TrimmedLine.Contains(TEXT("=")))
        {
            FString Key, Value;
            if (TrimmedLine.Split(TEXT("="), &Key, &Value))
            {
                FConfigNode KeyNode(Key.TrimStartAndEnd(), ENodeType::NT_String, Value.TrimStartAndEnd());
                CurrentSection->Children.Add(KeyNode);
            }
        }
    }
}

void ConfigParser::ParseYaml(const FString& YamlString, FConfigNode& ParentNode)
{
    YAML::Node YamlObject = YAML::Load(TCHAR_TO_UTF8(*YamlString));
    ParseYamlNode(YamlObject, ParentNode);
}

void ConfigParser::ParseJsonObject(TSharedPtr<FJsonObject> JsonObject, FConfigNode& ParentNode)
{
    for (const auto& Pair : JsonObject->Values)
    {
        FConfigNode ChildNode(FString(Pair.Key), ENodeType::NT_String, TEXT(""));
        switch (Pair.Value->Type)
        {
        case EJson::String:
            ChildNode.NodeType = ENodeType::NT_String;
            ChildNode.Value = Pair.Value->AsString();
            break;
        case EJson::Number:
            ChildNode.NodeType = ENodeType::NT_Number;
            ChildNode.Value = FString::Printf(TEXT("%f"), Pair.Value->AsNumber());
            break;
        case EJson::Boolean:
            ChildNode.NodeType = ENodeType::NT_Boolean;
            ChildNode.Value = Pair.Value->AsBool() ? TEXT("true") : TEXT("false");
            break;
        case EJson::Object:
            ChildNode.NodeType = ENodeType::NT_Object;
            ParseJsonObject(Pair.Value->AsObject(), ChildNode);
            break;
        case EJson::Array:
            ChildNode.NodeType = ENodeType::NT_Array;
            ParseJsonArray(Pair.Value->AsArray(), ChildNode);
            break;
        default:
            break;
        }
        ParentNode.Children.Add(ChildNode);
    }
}

void ConfigParser::ParseJsonArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray, FConfigNode& ParentNode)
{
    for (const auto& Item : JsonArray)
    {
        FConfigNode ChildNode(TEXT(""), ENodeType::NT_String, TEXT(""));
        switch (Item->Type)
        {
        case EJson::String:
            ChildNode.NodeType = ENodeType::NT_String;
            ChildNode.Value = Item->AsString();
            break;
        case EJson::Number:
            ChildNode.NodeType = ENodeType::NT_Number;
            ChildNode.Value = FString::Printf(TEXT("%f"), Item->AsNumber());
            break;
        case EJson::Boolean:
            ChildNode.NodeType = ENodeType::NT_Boolean;
            ChildNode.Value = Item->AsBool() ? TEXT("true") : TEXT("false");
            break;
        case EJson::Object:
            ChildNode.NodeType = ENodeType::NT_Object;
            ParseJsonObject(Item->AsObject(), ChildNode);
            break;
        case EJson::Array:
            ChildNode.NodeType = ENodeType::NT_Array;
            ParseJsonArray(Item->AsArray(), ChildNode);
            break;
        default:
            break;
        }
        ParentNode.Children.Add(ChildNode);
    }
}

void ConfigParser::ParseYamlNode(const YAML::Node& YamlNode, FConfigNode& ParentNode)
{
    for (auto it = YamlNode.begin(); it != YamlNode.end(); ++it)
    {
        FString Key = FString(it->first.as<std::string>().c_str());
        const YAML::Node& ValueNode = it->second;

        FConfigNode ChildNode(Key, ENodeType::NT_String, TEXT(""));

        if (ValueNode.IsScalar())
        {
            ChildNode.NodeType = ENodeType::NT_String;
            ChildNode.Value = FString(ValueNode.as<std::string>().c_str());
        }
        else if (ValueNode.IsSequence())
        {
            ChildNode.NodeType = ENodeType::NT_Array;
            for (const auto& ArrayElement : ValueNode)
            {
                FConfigNode ArrayChildNode(TEXT(""), ENodeType::NT_String, TEXT(""));
                if (ArrayElement.IsScalar())
                {
                    ArrayChildNode.NodeType = ENodeType::NT_String;
                    ArrayChildNode.Value = FString(ArrayElement.as<std::string>().c_str());
                }
                else if (ArrayElement.IsMap() || ArrayElement.IsSequence())
                {
                    ParseYamlNode(ArrayElement, ArrayChildNode);
                }
                ChildNode.Children.Add(ArrayChildNode);
            }
        }
        else if (ValueNode.IsMap())
        {
            ChildNode.NodeType = ENodeType::NT_Object;
            ParseYamlNode(ValueNode, ChildNode);
        }

        ParentNode.Children.Add(ChildNode);
    }
}