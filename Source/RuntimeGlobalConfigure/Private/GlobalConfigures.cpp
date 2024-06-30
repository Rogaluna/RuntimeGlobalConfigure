// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalConfigures.h"
#include "RuntimeGlobalConfigure.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "ConfigNode.h"
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

void UGlobalConfigures::Initialize(const PluginConfigs& initConf)
{
    if (!bInitialized)
    {
        // 初始化逻辑
        IFileManager& FileManager = IFileManager::Get();
        const FString& ContentDir = FPaths::ProjectContentDir();

        // 获取Configs目录，如果Configs指向的目录不存在，则创建一个
        const FString& ConfigDir = ContentDir / initConf.ConfigDir;

#if WITH_EDITOR
        if (!FileManager.DirectoryExists(*ConfigDir))
        {
            UE_LOG(LogRuntimeGlobalConfigurePlugin, Warning, L"Directory \"%s\" does not exist!", *ConfigDir);
            if (FileManager.MakeDirectory(*ConfigDir, true))
            {
                UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, L"Directory \"%s\" has been created", *ConfigDir);
            }
            else
            {
                UE_LOG(LogRuntimeGlobalConfigurePlugin, Error, L"Directory \"%s\" cannot be created", *ConfigDir);
            }
        }
#endif

        // 获取允许的配置文件类型
        const TArray<FString>& SupportedFileTypes = initConf.SupportedFileTypes;
        // 找到所有符合类型的配置文件
        TArray<FString> AllFiles;
        for (const FString& FileType : SupportedFileTypes)
        {
            TArray<FString> FilesOfType;
            FileManager.FindFilesRecursive(FilesOfType, *ConfigDir, *FileType, true, false);
            AllFiles.Append(FilesOfType);
        }
        
        // 选择指定的配置文件
		TArray<FString> MatchingFiles;

        // 从配置文件中获取配置
		for (const FString& ConfigFilePath : AllFiles)
		{
            FString Filename = FPaths::GetCleanFilename(ConfigFilePath);
            UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("FilePath: %s"), *Filename);
            FString FileContent;
            if (FFileHelper::LoadFileToString(FileContent, *ConfigFilePath))
            {
                ParsingConfigurationFile(*FileContent, Filename);
            }
            else
            {
                UE_LOG(LogRuntimeGlobalConfigurePlugin, Error, TEXT("Cannot read file: %s"), *ConfigFilePath);
            }
        }
        

        UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("GlobalConfigures Singleton Initialized"));
        bInitialized = true;
    }
}

UGlobalConfigures::UGlobalConfigures()
    : bInitialized(false)
{

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
    RootNode.Children.AddUnique(Node);


	UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("Parsed content of %s:\n"), *Filename);
	//ConfigParser::FNodeCallback Callback;
	//Callback.BindLambda([](const FConfigNode& Node, int32 Depth)
	//	{
	//		FString Indent = TEXT("");
	//		for (int32 i = 0; i < Depth; ++i)
	//		{
	//			Indent += TEXT("  ");
	//		}
	//		UE_LOG(LogRuntimeGlobalConfigurePlugin, Log, TEXT("%sName: %s, Type: %d, Value: %s"), *Indent, *Node.Name, static_cast<int32>(Node.NodeType), *Node.Value);
	//	});
	//ConfigParser::TraverseConfigNode(Node, Callback);
}
