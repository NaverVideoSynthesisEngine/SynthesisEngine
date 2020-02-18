// Fill out your copyright notice in the Description page of Project Settings.


#include "Progress.h"

void FProgress::ResetModelIndex()
{
    CurrentSkeletalMesh = NULL;
    ModelIndex = 0;
}

void FProgress::ResetAnimationIndex()
{
	CurrentAnimation = NULL;
	AnimationIndex = 0;
}

void FProgress::ResetClothesIndex()
{
	CurrentClothesMesh = NULL;
	CurrentClothesAnimation = NULL;
	ClothesIndex = 0;
}

bool FProgress::IsGarmentsLoadedProperly()
{
	return LoadedGarments->IsLoadedProperly();
}

void FProgress::LoadGarments()
{
	FString SkeletonMeshID = CurrentSkeletalMesh->GetName();
	FString AnimationID = CurrentAnimation->GetName();
	LoadedGarments->LoadGarments(SkeletonMeshID, AnimationID);
}

void FProgress::Reset()
{
    ResetModelIndex();
    ResetAnimationIndex();
    ResetClothesIndex();
}

bool FProgress::IsNextModelAvailable()
{
	return AllModels.IsValidIndex(ModelIndex + 1);
}

bool FProgress::IsNextAnimationAvailable()
{
	return AllAnimations.IsValidIndex(AnimationIndex + 1);
}

bool FProgress::IsNextClothesAvailable()
{
	return LoadedGarments->IsNextClothesAvailable_MC();
}

bool FProgress::MoveToNextModel()
{
	if (CurrentSkeletalMesh == NULL)
	{
		if (AllModels.IsValidIndex(0))
		{
			CurrentSkeletalMesh = AllModels[0];
			ResetAnimationIndex();
			ResetClothesIndex();
			return true;
		}
		return false;
	}
	if (IsNextModelAvailable())
	{
		CurrentSkeletalMesh = AllModels[++ModelIndex];
		ResetAnimationIndex();
		ResetClothesIndex();
		return true;
	}
	return false;
}

bool FProgress::MoveToNextAnimation()
{
	if (CurrentAnimation == NULL)
	{
		if (AllAnimations.IsValidIndex(0))
		{
			CurrentAnimation = AllAnimations[0];
            USkeleton* Skeleton = CurrentSkeletalMesh->Skeleton;
            bool retargetable = Skeleton == CurrentAnimation->GetSkeleton();
            if(retargetable)
            {
                LoadGarments();
                ResetClothesIndex();
                return true;
            }
		}
        else
        {
            return false;
        }
	}
	while (IsNextAnimationAvailable())
	{
		CurrentAnimation = AllAnimations[++AnimationIndex];
		USkeleton* Skeleton = CurrentSkeletalMesh->Skeleton;
        bool retargetable = Skeleton == CurrentAnimation->GetSkeleton();
		if (retargetable)
		{
			LoadGarments();
			ResetClothesIndex();
			return true;
		}
	}
	return false;
}

bool FProgress::MoveToNextClothes()
{
	//    return LoadedGarments->MoveToNextClothes();
	return LoadedGarments->MoveToNextClothes_MC();
}

bool FProgress::MoveToNextClothes(bool& isCombinationChanged)
{
	return LoadedGarments->MoveToNextClothes_MC(isCombinationChanged);
}


USkeletalMesh* FProgress::GetCurrentSkeletalMesh()
{
	return CurrentSkeletalMesh;
}

UAnimationAsset* FProgress::GetCurrentAnimation()
{
	return CurrentAnimation;
}

USkeletalMesh* FProgress::GetCurrentClothesMesh()
{
	return LoadedGarments->GetCurrentClothesMesh();
}

UAnimationAsset* FProgress::GetCurrentClothesAnimation()
{
	return LoadedGarments->GetCurrentClothesAnimation();
}

TArray<USkeletalMesh*> FProgress::GetCurrentClothesMeshes()
{
	return LoadedGarments->GetCurrentClothesMeshes();
}

TArray<UAnimationAsset*> FProgress::GetCurrentClothesAnimations()
{
	return LoadedGarments->GetCurrentClothesAnimations();
}

bool FProgress::WillGarmentCombinationBeChanged()
{
	return LoadedGarments->WillGarmentCombinationBeChanged();
}

void FProgress::ResetGarmentIndex()
{
	LoadedGarments->ResetGarmentIndex();
}

UMaterialInstance* FProgress::GetRandomMaterial()
{
	int randomInt = rand() % AllMaterials.Num();
	return AllMaterials[randomInt];
}


FProgress::FProgress(TArray<USkeletalMesh*> AllModels, TArray<UAnimationAsset*> AllAnimations, TArray<UMaterialInstance*> AllMaterials, FString AlembicPath)
{
	if (AllModels.Num() < 1 || AllAnimations.Num() < 1 || AllMaterials.Num() < 1)
	{
		UE_LOG(SynthesisEngine, Error, TEXT("No available Models or No available Animations or No available Materials"));
	}

	this->AllModels = AllModels;
	this->AllAnimations = AllAnimations;
	this->AllMaterials = AllMaterials;
	this->LoadedGarments = new FGarmentCollection(AlembicPath);

	ModelIndex = 0;
	AnimationIndex = 0;

	CurrentSkeletalMesh = NULL;
	CurrentAnimation = NULL;

	for (const auto& material : AllMaterials)
	{
		FString category = FUtil::ExtractMaterialCategoryFromFullPath(material->GetFullName());
		CategoryMaterialSet * MatSet = CategorizedMaterials.FindByPredicate([=](const CategoryMaterialSet& InItem)
		{
			return InItem.Category == category;
		});
		if (MatSet)
		{
			MatSet->Materials.Add(material);
		}
		else 
		{
			CategoryMaterialSet newMatSat;
			newMatSat.Category = category;
			newMatSat.Materials.Add(material);
			CategorizedMaterials.Add(newMatSat);
		}
	}

	FString RelativeFullPath = FString("/Game/Materials/MaterialSetting.json");
	FString FullPath = FUtil::Util_GetFullPath(RelativeFullPath);

	const FString JsonFilePath = FullPath;
	FString JsonString;

	FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		TArray<TSharedPtr<FJsonValue>> Items = JsonObject->GetArrayField("Items");
		for (int i = 0; i < Items.Num(); i++)
		{
			GarmentMaterialSetting MatSet;

			TSharedPtr<FJsonObject> Object = Items[i]->AsObject();
			FString GarmentID = Object->GetStringField("ID");

			MatSet.GarmentID = GarmentID;
			//UE_LOG(SynthesisEngine, Warning, TEXT("ID : %s"), *GarmentID);

			TArray<TSharedPtr<FJsonValue>> MaterialCategoriesPerSlot = Object->GetArrayField("Slots");
			TArray<MaterialCategoryList> ListPerSlots;

			for (int j = 0; j < MaterialCategoriesPerSlot.Num(); j++)
			{
				TArray<TSharedPtr<FJsonValue>> Categories = MaterialCategoriesPerSlot[j]->AsArray();
				MaterialCategoryList MaterialCategories;
				//UE_LOG(SynthesisEngine, Warning, TEXT("Slot : %d"), j);
				for (int k = 0; k < Categories.Num(); k++)
				{
					//UE_LOG(SynthesisEngine, Warning, TEXT("    %s"), *Categories[k]->AsString());
					MaterialCategories.Categories.Add(Categories[k]->AsString());
				}
				ListPerSlots.Add(MaterialCategories);
			}

			MatSet.CategoryPerSlot = ListPerSlots;
			MaterialSetting.Add(MatSet);
		}
	}
}

UMaterialInstance* FProgress::GetRandomMaterialFromCategories(TArray<FString> Categories)
{
	TArray<UMaterialInstance*> AvailableMats;
	for (const auto& category : Categories)
	{
		CategoryMaterialSet * MatSet = CategorizedMaterials.FindByPredicate([=](const CategoryMaterialSet& InItem)
		{
			return InItem.Category == category;
		});
		if (MatSet != nullptr)
		{
			int randomInt = rand() % MatSet->Materials.Num();
			AvailableMats.Add(MatSet->Materials[randomInt]); //Pick a mat per a category, Pick random one later. 
		}
	}
	if (AvailableMats.Num() != 0)
	{
		int randomInt = rand() % AvailableMats.Num();
		return AvailableMats[randomInt];
	}
	UE_LOG(SynthesisEngine, Error, TEXT("Can not find valid material for these categories. please check MaterialSetting.json"));
	return GetRandomMaterial();
}

UMaterialInstance* FProgress::GetRandomMaterialFollowingSetting(FString GarmentID, int SlotIndex)
{
	//1. Search Garment on Setting file
	GarmentMaterialSetting * Setting = MaterialSetting.FindByPredicate([=](const GarmentMaterialSetting& InItem)
	{
		return InItem.GarmentID == GarmentID;
	});

	if (Setting == nullptr)
	{
		return GetRandomMaterial(); //If there is no setting for this Garment, Pick Random Mat from all mats;
	}

	//2. Get Available Material Category by SlotIndex;
	if (!Setting->CategoryPerSlot.IsValidIndex(SlotIndex))
	{
		UE_LOG(SynthesisEngine, Error, TEXT("Out of Range Index during Randomizing Material."));
		return GetRandomMaterial();
	}
	MaterialCategoryList AvailableCategories = Setting->CategoryPerSlot[SlotIndex];

	//3. Return Random Material which is in available category.
	return GetRandomMaterialFromCategories(AvailableCategories.Categories);
}


FProgress::~FProgress()
{
}

USkeletalMesh* FProgress::GetRandomSkeletalMesh()
{
    int randIndex = rand() % AllModels.Num();
    return AllModels[randIndex];
}

UAnimationAsset* FProgress::GetRandomHumanAnimation()
{
    int randIndex = rand() % AllAnimations.Num();
    return AllAnimations[randIndex];
}

