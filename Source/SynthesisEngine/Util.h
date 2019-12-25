// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"

/**
 * 
 */
class SYNTHESISENGINE_API FUtil
{
private:
	FUtil();
	~FUtil();
public:
	static FString ExtractAlembicIdentifierFromName(const FString& Name);
	static FString ExtractGarmentIdentifierFromFullPath(const FString& FullPath);
	static FString ExtractMaterialCategoryFromFullPath(const FString& FullPath);
	static FString Util_GetFullPath(const FString& relativePath);
};
