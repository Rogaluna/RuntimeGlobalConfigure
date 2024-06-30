// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GlobalConfigBlueprintFuncLib.generated.h"

/**
 * 
 */
UCLASS()
class RUNTIMEGLOBALCONFIGURE_API UGlobalConfigBlueprintFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "全局配置蓝图函数库", DisplayName = "获取全局配置实例")
	static class UGlobalConfigures* GetGlobalConfigures();
};
