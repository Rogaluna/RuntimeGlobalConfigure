// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConfigNode.generated.h"

UENUM(BlueprintType)
enum class ENodeType : uint8
{
    // 基础类型
    NT_String UMETA(DisplayName = "String"),
    NT_Int UMETA(DisplayName = "Int"),
    NT_Float UMETA(DisplayName = "Float"),
    NT_Bool UMETA(DisplayName = "Bool"),
    // 对象类型
    NT_Array UMETA(DisplayName = "Array"),
    NT_Object UMETA(DisplayName = "Object")
};

/**
 * 
 */
UCLASS()
class UConfigNode : public UObject
{
    GENERATED_BODY()

public:
    // 节点名称
    UPROPERTY(BlueprintReadWrite, Category="节点名")
    FString Name;
    // 节点类型
    UPROPERTY(BlueprintReadWrite, Category="节点名")
    ENodeType Type;
    // 节点值 (基础类型值)
    UPROPERTY(BlueprintReadWrite, Category="节点值")
    FString Value;
    // 子节点（对象类型值）
    UPROPERTY(BlueprintReadWrite, Category="子节点")
    TArray<UConfigNode*> Children;

    // 构造函数
    UConfigNode()
        : Name(TEXT("")) 
        , Value(TEXT(""))
        , Children(TArray<UConfigNode*>())
    {}

    // 带参数的构造函数
    UConfigNode(FString InName, ENodeType InNodeType, FString InValue = TEXT(""), TArray<UConfigNode*> InChildren = TArray<UConfigNode*>())
        : Name(InName)
        , Value(InValue)
        , Children(InChildren)
    {}
};