// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalConfigBlueprintFuncLib.h"
#include "GlobalConfigures.h"

class UGlobalConfigures* UGlobalConfigBlueprintFuncLib::GetGlobalConfigures()
{
	return &UGlobalConfigures::Get();
}
