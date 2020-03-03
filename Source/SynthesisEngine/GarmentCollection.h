// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Util.h"
#include "Engine/StreamableManager.h"
#include "Animation/AnimSequence.h"
#include "Core/Public/HAL/FileManager.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
/**
 *
 */

class Combination
{
public:
	TArray<FString> GarmentIDs;
};

class Garment
{
public:
	//Following rules are always be kept.
	//1. The order of 'Meshes' and 'Animations' are always same. for example, Animation for 1st item of 'Meshes' is 1st item of 'Animations'
	//2. You can find GarmentID from Combination. 
	FString GarmentID;
	TArray<USkeletalMesh*> Meshes;
	TArray<UAnimationAsset*> Animations;
};

class SYNTHESISENGINE_API FGarmentCollection
{
private:
	FString BasePath;
	int GarmentIndex;
	bool isLoadedProperly;

	USkeletalMesh* CurrentClothesMesh;
	UAnimationAsset* CurrentClothesAnimation;
	TArray<USkeletalMesh*> LoadedClothesMesh;
	TArray<UAnimationAsset*> LoadedClothesAnimation;
	TArray<Garment> LoadedGarments;
	TArray<Combination> LoadedCombinations;

	bool CheckValidation(TArray<USkeletalMesh*> Meshes, TArray<UAnimationAsset*> Animations);
	void LoadJsonSetting(FString SkeletalMeshID, TArray<Combination> * LoadedCombinations);
	void LoadAlembics(FString SkeletalMeshID, FString AnimationID, TArray<USkeletalMesh*> * LoadedClothesMesh,
                      TArray<UAnimationAsset*> * LoadedClothesAnimation, TArray<Garment> * LoadedGarments);
    
    Garment* GetGarmentByID(TArray<Garment>& TargetGarmentArray, FString GarmentID);
private: // New features
	int CombinationIndex;
	TArray<USkeletalMesh*> CurrentClothesMeshes;
	TArray<UAnimationAsset*> CurrentClothesAnimations;

    void UpdateMeshAndAnimationUsingCombination(TArray<USkeletalMesh*> * TargetMeshes, TArray<UAnimationAsset*> * TargetAnimations, Combination Combination);
    void UpdateMeshAndAnimationUsingCombination(TArray<USkeletalMesh*> * TargetMeshes, TArray<UAnimationAsset*> * TargetAnimations, TArray<Garment>& TargetGarmentArray, Combination Combination);
public:
	//Load All kind of garments under specific human and specific animation. for example, Sleeveless and Training Pants animation which are recorded using 'Ybot' and 'Pointing'
	void LoadGarments(FString SkeletalMeshID, FString AnimationID);
	void Empty();
	bool IsEmpty();
	bool IsLoadedProperly();

	bool IsNextClothesAvailable();
	bool MoveToNextClothes();
	USkeletalMesh* GetCurrentClothesMesh();
	UAnimationAsset* GetCurrentClothesAnimation();

	FGarmentCollection(FString BasePath);
	FGarmentCollection(FString BasePath, FString SkeletalMeshID, FString AnimationID);
	~FGarmentCollection();

public: // New features
	void Empty_MC();
	bool IsNextClothesAvailable_MC();
	bool MoveToNextClothes_MC();
	// New feature : Multiple combinations
	bool MoveToNextClothes_MC(bool& isCombinationChanged);
	// New feature : Check Garment combination will be changed 
	bool WillGarmentCombinationBeChanged();
	// New feature : Iterate a garment combination with multiple camera angle
	void ResetGarmentIndex();
	
	TArray<USkeletalMesh*> GetCurrentClothesMeshes();
	TArray<UAnimationAsset*> GetCurrentClothesAnimations();

public: // New feature (02.20) /* for MultiPersonPerturberComponent */
    /* params:
        ClothesMeshes : Can be empty, Especially if there is no combination in json setting file.
        ClothesAnimations : Can be empty, Especially if there is no combination in json setting file.
     */
    void GetRandomClothesMeshesAndAnimations(FString SkeletalMeshID, FString AnimationID,
                                             TArray<USkeletalMesh*>& ClothesMeshes, TArray<UAnimationAsset*>& ClothesAnimations);
};
