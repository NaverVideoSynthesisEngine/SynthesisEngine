// Fill out your copyright notice in the Description page of Project Settings.


#include "SynthesisEngineInstance.h"
#include "PhotoRoomManager.h"

USynthesisEngineInstance::USynthesisEngineInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->InitPaths();
	this->InitRefers();
	this->InitProgressAndCursor();

	LevelIndex = 0;
	b_UpdateAuto = false;

	LevelsToLoad.Add(TEXT("EnvironmentSet_Toy"));
	LevelsToLoad.Add(TEXT("Modern_Apartment_Toy"));
	LevelsToLoad.Add(TEXT("Demo_Toy"));
}

void USynthesisEngineInstance::InitPaths()
{
	if (ModelPath.IsEmpty())
	{
		ModelPath = FString(TEXT("/Game/Models/"));
	}
	if (AnimationPath.IsEmpty())
	{
		AnimationPath = FString(TEXT("/Game/Animations/Toy/FBX"));
	}
	if (ClothesAnimationPath.IsEmpty())
	{
		ClothesAnimationPath = FString(TEXT("/Game/Animations/Toy/Alembic"));
	}
	if (MaterialPath.IsEmpty())
	{
		MaterialPath = FString(TEXT("/Game/Materials"));
	}
}

void USynthesisEngineInstance::InitRefers()
{
	for (TObjectIterator<USkeletalMesh> Itr; Itr; ++Itr)
	{
		if ((*Itr)->GetPathName().Contains(ModelPath)) 
		{
			UE_LOG(SynthesisEngine, Warning, TEXT("Model : %s"), *((*Itr)->GetName()));
			AllModels.Add((*Itr));
		}

	}
	for (TObjectIterator<UAnimationAsset> Itr; Itr; ++Itr)
	{
		if ((*Itr)->GetPathName().Contains(AnimationPath))
		{
			UE_LOG(SynthesisEngine, Warning, TEXT("Animation : %s"), *((*Itr)->GetName()));
			AllAnimations.Add((*Itr));
		}
	}
	for (TObjectIterator<UMaterialInstance> Itr; Itr; ++Itr)
	{
		if ((*Itr)->GetPathName().Contains(MaterialPath))
		{
			//UE_LOG(SynthesisEngine, Warning, TEXT("Material : %s"), *((*Itr)->GetName()));
			AllMaterials.Add((*Itr));
		}
	}
}

void USynthesisEngineInstance::InitProgressAndCursor()
{
	if (EngineMode == EENGINE_MODE::RE_START)
	{
		/* */
		return;
	}
	Progress = new FProgress(AllModels, AllAnimations, AllMaterials, ClothesAnimationPath);
}

void USynthesisEngineInstance::RegisterPhotoRoomManager(APhotoRoomManager* PhotoRoomManager)
{
	this->PhotoRoomManager = PhotoRoomManager;
}

void USynthesisEngineInstance::ReleasePhotoRoomManager(APhotoRoomManager* PhotoRoomManager)
{
	if (this->PhotoRoomManager == PhotoRoomManager)
	{
		this->PhotoRoomManager = nullptr;
	}
}
