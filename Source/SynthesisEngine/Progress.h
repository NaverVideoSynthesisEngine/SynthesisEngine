// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "GarmentCollection.h"
#include "Util.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
/**
 *
 */

class MaterialCategoryList
{
public:
	TArray<FString> Categories;
};

class GarmentMaterialSetting
{
public:
	FString GarmentID;
	TArray<MaterialCategoryList> CategoryPerSlot;
};

class CategoryMaterialSet
{
public:
	FString Category;
	TArray<UMaterialInstance*> Materials;
};

class SYNTHESISENGINE_API FProgress
{
private:
	int ModelIndex;
	int AnimationIndex;
	int ClothesIndex;

	TArray<USkeletalMesh*> AllModels;
	TArray<UAnimationAsset*> AllAnimations;
	TArray<UMaterialInstance*> AllMaterials;
	class FGarmentCollection* LoadedGarments;

	USkeletalMesh* CurrentSkeletalMesh;
	UAnimationAsset* CurrentAnimation;
	USkeletalMesh* CurrentClothesMesh;
	UAnimationAsset* CurrentClothesAnimation;

    void ResetModelIndex();
	void ResetAnimationIndex();
	void ResetClothesIndex();
public:
	bool IsNextModelAvailable();
	bool IsNextAnimationAvailable();
	bool IsNextClothesAvailable();

	bool MoveToNextModel();
	bool MoveToNextAnimation();
	bool MoveToNextClothes();
	bool MoveToNextClothes(bool& isCombinationChanged);

	bool IsGarmentsLoadedProperly();
	void LoadGarments();
    void Reset();
    
	USkeletalMesh* GetCurrentSkeletalMesh();
	UAnimationAsset* GetCurrentAnimation();
	USkeletalMesh* GetCurrentClothesMesh();
	UAnimationAsset* GetCurrentClothesAnimation();

	UMaterialInstance* GetRandomMaterial();

	FProgress(TArray<USkeletalMesh*> AllModels, TArray<UAnimationAsset*> AllAnimations, TArray<UMaterialInstance*> AllMaterials, FString AlembicPath);
	~FProgress();

private: // New features : Material Category
	TArray<CategoryMaterialSet> CategorizedMaterials;
	TArray<GarmentMaterialSetting> MaterialSetting;

	UMaterialInstance* GetRandomMaterialFromCategories(TArray<FString> Categories);
public: // New features : Material Category
	UMaterialInstance* GetRandomMaterialFollowingSetting(FString GarmentID, int SlotIndex);

public: // New features
	TArray<USkeletalMesh*> GetCurrentClothesMeshes();
	TArray<UAnimationAsset*> GetCurrentClothesAnimations();

	//New feature : Iterate a garment combination with multiple camera angle
	bool WillGarmentCombinationBeChanged();
	void ResetGarmentIndex();

public: // New feature (02.18) /* for MultiPersonPerturberComponent */
    USkeletalMesh* GetRandomSkeletalMesh();
    UAnimationAsset* GetRandomHumanAnimation(USkeleton * Skeleton);
    void GetRandomClothesMeshesAndAnimations(USkeletalMesh * HumanMesh, UAnimationAsset * HumanAnimation,
                                             TArray<USkeletalMesh*>& ClothesMeshes,
                                             TArray<UAnimationAsset*>& ClothesAnimations);
};
