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
    
}

UGlobalConfigures::UGlobalConfigures()
    : bInitialized(false)
{
    Initialize();
}

UGlobalConfigures::~UGlobalConfigures()
{

}

void UGlobalConfigures::ParsingConfigurationFile(const FString& FileContent, const FString& Filename)
{
    const FString& FileExt = FPaths::GetExtension(Filename);

    FConfigNode Node;

    if (FileExt == "ini")
    {
        ConfigParser::ParseIni(FileContent, Node);
    }
    else if (FileExt == "json")
    {
        ConfigParser::ParseJson(FileContent, Node);
    }
    else if (FileExt == "yaml")
    {
        ConfigParser::ParseYaml(FileContent, Node);
    }
    else if (FileExt == "yml")
    {
        ConfigParser::ParseYaml(FileContent, Node);
    }

    Node.Name = Filename;
    Node.NodeType = ENodeType::NT_Object;


	UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("Parsed content of %s:\n"), *Filename);
	ConfigParser::FNodeCallback Callback;
	Callback.BindLambda([](const FConfigNode& Node, int32 Depth)
		{
			FString Indent = TEXT("");
			for (int32 i = 0; i < Depth; ++i)
			{
				Indent += TEXT("  ");
			}
			UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("%sName: %s, Type: %d, Value: %s"), *Indent, *Node.Name, static_cast<int32>(Node.NodeType), *Node.Value);
		});
	ConfigParser::TraverseConfigNode(Node, Callback);
}
