// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "ConfigNode.h"
#include "yaml-cpp/yaml.h"

/**
 * 
 */
class ConfigParser
{
public:
	DECLARE_DELEGATE_TwoParams(FNodeCallback, const FConfigNode&, int32);

	ConfigParser();
	~ConfigParser();

public:

	static void TraverseConfigNode(const FConfigNode& Node, FNodeCallback Callback, int32 Depth = 0);
	
	static void ParseJson(const FString& JsonString, FConfigNode& ParentNode);
	static void ParseIni(const FString& IniString, FConfigNode& ParentNode);
	static void ParseYaml(const FString& YamlString, FConfigNode& ParentNode);

private:
	static void ParseJsonObject(TSharedPtr<FJsonObject> JsonObject, FConfigNode& ParentNode);
	static void ParseJsonArray(const TArray<TSharedPtr<FJsonValue>>& JsonArray, FConfigNode& ParentNode);
	static void ParseYamlNode(const YAML::Node& YamlNode, FConfigNode& ParentNode);
};
