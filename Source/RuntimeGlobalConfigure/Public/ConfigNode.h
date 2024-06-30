// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum class ENodeType : uint8
{
    NT_String,
    NT_Number,
    NT_Boolean,
    NT_Object,
    NT_Array
};

/**
 * 
 */
struct FConfigNode
{
    // 节点名称
    FString Name;

    // 节点类型
    ENodeType NodeType;

    // 节点值 (根据类型不同，可以是字符串、数字、布尔值等)
    FString Value;

    // 子节点
    TArray<FConfigNode> Children;

    // 构造函数
    FConfigNode()
        : Name(TEXT("")), NodeType(ENodeType::NT_String), Value(TEXT("")) {}

    // 带参数的构造函数
    FConfigNode(FString InName, ENodeType InNodeType, FString InValue)
        : Name(InName), NodeType(InNodeType), Value(InValue) {}

    // 拷贝构造函数
    FConfigNode(const FConfigNode& Other)
    {
        Name = Other.Name;
        NodeType = Other.NodeType;
        Value = Other.Value;
        Children = Other.Children;
    }

    // 赋值运算符
    FConfigNode& operator=(const FConfigNode& Other)
    {
        if (this != &Other)
        {
            Name = Other.Name;
            NodeType = Other.NodeType;
            Value = Other.Value;
            Children = Other.Children;
        }
        return *this;
    }

    bool operator==(const FConfigNode& Other) const
    {
        return Name == Other.Name &&
            NodeType == Other.NodeType &&
            Value == Other.Value &&
            Children == Other.Children;
    }
};