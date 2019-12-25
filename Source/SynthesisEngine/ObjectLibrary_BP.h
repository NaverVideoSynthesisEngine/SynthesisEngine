// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Animation/AnimSequence.h"
#include "Engine/ObjectLibrary.h"
#include "ObjectLibrary_BP.generated.h"

/**
 * 
 */
UCLASS()
class SYNTHESISENGINE_API UObjectLibrary_BP : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "CppBlueprintLibrary")
	static TArray<UObject*> LoadObjectLibrary(const FString& Path, TSubclassOf<UObject> ObjectClass);
	
	UFUNCTION(BlueprintCallable, Category = "CppBlueprintLibrary")
	static TArray<UAnimSequence*> LoadAllAnimationSequenceForSkeleton(USkeleton * Skeleton);
};
