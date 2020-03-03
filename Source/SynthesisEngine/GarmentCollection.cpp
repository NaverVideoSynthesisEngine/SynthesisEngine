// Fill out your copyright notice in the Description page of Project Settings.


#include "GarmentCollection.h"

//TODO ���� �ϳ��� Human Animation�� ���ؼ� �������� Clothes�� ����ϴ� �κ��� ����
FGarmentCollection::FGarmentCollection(FString BasePath)
	: BasePath(BasePath), GarmentIndex(0), CombinationIndex(0)
{
}
FGarmentCollection::FGarmentCollection(FString BasePath, FString SkeletalMeshID, FString AnimationID)
	: BasePath(BasePath), GarmentIndex(0), CombinationIndex(0)
{
}

FGarmentCollection::~FGarmentCollection()
{
	//TODO : ? TArray �ȿ� �ִ� F  (C++ Class)�� ���� ���� release�ؾ��ϳ�?
}
void FGarmentCollection::Empty()
{
	LoadedClothesMesh.Empty();
	LoadedClothesAnimation.Empty();
	LoadedGarments.Empty();
	LoadedCombinations.Empty();
	CombinationIndex = 0;
	GarmentIndex = 0;

	CurrentClothesMesh = NULL;
	CurrentClothesAnimation = NULL;
}

void FGarmentCollection::Empty_MC()
{
	LoadedClothesMesh.Empty();
	LoadedClothesAnimation.Empty();
	LoadedGarments.Empty();
	LoadedCombinations.Empty();
	CombinationIndex = 0;
	GarmentIndex = 0;

	CurrentClothesMeshes.Empty();
	CurrentClothesAnimations.Empty();
}

bool FGarmentCollection::IsEmpty()
{
	return LoadedGarments.Num() == 0;
}

bool FGarmentCollection::IsLoadedProperly()
{
	return isLoadedProperly;
}

bool FGarmentCollection::IsNextClothesAvailable()
{
	return LoadedClothesMesh.IsValidIndex(GarmentIndex + 1);
}

bool FGarmentCollection::IsNextClothesAvailable_MC()
{
	return LoadedCombinations.IsValidIndex(CombinationIndex + 1);
}

bool FGarmentCollection::MoveToNextClothes()
{
	if (CurrentClothesMesh == NULL)
	{
		if (LoadedClothesMesh.IsValidIndex(0))
		{
			CurrentClothesMesh = LoadedClothesMesh[0];
			CurrentClothesAnimation = LoadedClothesAnimation[0];
			return true;
		}
		return false;
	}
	Garment* garment = GetGarmentByID(LoadedGarments, LoadedCombinations[CombinationIndex].GarmentIDs[0]);
	if (garment != nullptr)
	{
		if (garment->Meshes.IsValidIndex(GarmentIndex + 1))    //����ȭ �� ���� cloth ��������
		{
			GarmentIndex++;
			CurrentClothesMesh = garment->Meshes[GarmentIndex];
			CurrentClothesAnimation = garment->Animations[GarmentIndex];
			return true;
		}
		else if (LoadedCombinations.IsValidIndex(CombinationIndex + 1)) //���� combination���� �Ѿ��
		{
			GarmentIndex = 0;
			CombinationIndex++;
			garment = GetGarmentByID(LoadedGarments, LoadedCombinations[CombinationIndex].GarmentIDs[0]);
			CurrentClothesMesh = garment->Meshes[GarmentIndex];
			CurrentClothesAnimation = garment->Animations[GarmentIndex];
			return true;
		}
	}
	return false;
}

bool FGarmentCollection::MoveToNextClothes_MC()
{
	//UE_LOG(SynthesisEngine, Warning, TEXT("Next Cloth")); //Cloth Mesh�� ����� �� ���� ȣ��ȴ�. ����ȭ �Ǿ��ֵ�, �ƴϵ� ���ο� Mesh�� �ٲ�� ȣ��
	if (CurrentClothesAnimations.Num() < 1) //Init (when there is no clothes at all)
	{
		if (LoadedCombinations.IsValidIndex(0))
		{
			UpdateMeshAndAnimationUsingCombination(&CurrentClothesMeshes, &CurrentClothesAnimations, LoadedCombinations[CombinationIndex]);
			return true;
		}
		return false;
	}
	Garment* garment = GetGarmentByID(LoadedGarments, LoadedCombinations[CombinationIndex].GarmentIDs[0]);
	if (garment != nullptr)
	{
		if (garment->Meshes.IsValidIndex(GarmentIndex + 1)) //Move to next mesh within a Garment.
		{
			GarmentIndex++;
			UpdateMeshAndAnimationUsingCombination(&CurrentClothesMeshes, &CurrentClothesAnimations, LoadedCombinations[CombinationIndex]);
			return true;
		}
		else if (LoadedCombinations.IsValidIndex(CombinationIndex + 1)) //Move to next combination
		{
			GarmentIndex = 0;
			CombinationIndex++;
			UpdateMeshAndAnimationUsingCombination(&CurrentClothesMeshes, &CurrentClothesAnimations, LoadedCombinations[CombinationIndex]);
			return true;
		}
	}
	return false;
}

bool FGarmentCollection::MoveToNextClothes_MC(bool& isCombinationChanged)
{
	if (CurrentClothesAnimations.Num() < 1) //Init (when there is no clothes at all)
	{
		if (LoadedCombinations.IsValidIndex(0))
		{
			UpdateMeshAndAnimationUsingCombination(&CurrentClothesMeshes, &CurrentClothesAnimations, LoadedCombinations[CombinationIndex]);
			isCombinationChanged = false;
			return true;
		}
		return false;
	}
	Garment* garment = GetGarmentByID(LoadedGarments, LoadedCombinations[CombinationIndex].GarmentIDs[0]);
	if (garment != nullptr)
	{
		if (garment->Meshes.IsValidIndex(GarmentIndex + 1)) //Move to next mesh within a Garment.
		{
			GarmentIndex++;
			UpdateMeshAndAnimationUsingCombination(&CurrentClothesMeshes, &CurrentClothesAnimations, LoadedCombinations[CombinationIndex]);
			isCombinationChanged = false;
			return true;
		}
		else if (LoadedCombinations.IsValidIndex(CombinationIndex + 1)) //Move to next combination
		{
			GarmentIndex = 0;
			CombinationIndex++;
			UpdateMeshAndAnimationUsingCombination(&CurrentClothesMeshes, &CurrentClothesAnimations, LoadedCombinations[CombinationIndex]);
			isCombinationChanged = true;
			return true;
		}
	}
	return false;
}

USkeletalMesh* FGarmentCollection::GetCurrentClothesMesh() //Will be deprecated
{
	return CurrentClothesMesh;
}

UAnimationAsset* FGarmentCollection::GetCurrentClothesAnimation() //Will be deprecated
{
	return CurrentClothesAnimation;
}

bool FGarmentCollection::WillGarmentCombinationBeChanged()
{
	if (LoadedCombinations.Num() == 0)
	{
		UE_LOG(SynthesisEngine, Error, TEXT("There is no garment settings."));
		return false;
	}
	Garment* garment = GetGarmentByID(LoadedGarments, LoadedCombinations[CombinationIndex].GarmentIDs[0]);
	if (garment != nullptr)
	{
		return CurrentClothesAnimations.Num() >= 1 && !garment->Meshes.IsValidIndex(GarmentIndex + 1);
		// true : Moving to next combination
		// false : Not moving to next combination, Just moving to next fragment. (ex. 00~50 -> 51~99)
	}
	else
	{
		UE_LOG(SynthesisEngine, Error, TEXT("Unable to determine garment set changing"));
		return false;
	}
}

void FGarmentCollection::ResetGarmentIndex()
{
	GarmentIndex = 0;
	if (LoadedCombinations.IsValidIndex(0))
	{
		UpdateMeshAndAnimationUsingCombination(&CurrentClothesMeshes, &CurrentClothesAnimations, LoadedCombinations[CombinationIndex]);
	}
}

TArray<USkeletalMesh*> FGarmentCollection::GetCurrentClothesMeshes()
{
	return CurrentClothesMeshes;
}

TArray<UAnimationAsset*> FGarmentCollection::GetCurrentClothesAnimations()
{
	return CurrentClothesAnimations;
}

void FGarmentCollection::LoadGarments(FString SkeletalMeshID, FString AnimationID)
{
	//    Empty();
	Empty_MC();
	LoadJsonSetting(SkeletalMeshID, &(this->LoadedCombinations));
	LoadAlembics(SkeletalMeshID, AnimationID, &(this->LoadedClothesMesh), &(this->LoadedClothesAnimation), &(this->LoadedGarments));
    if (LoadedClothesMesh.Num() != 0 && LoadedClothesAnimation.Num() != 0)
    {
        isLoadedProperly = true;
    }
    else
    {
        UE_LOG(SynthesisEngine, Warning, TEXT("No Clothes : Mesh = %s , Animation = %s"), *SkeletalMeshID, *AnimationID);
        isLoadedProperly = false;
    }
}

Garment* FGarmentCollection::GetGarmentByID(TArray<Garment>& TargetGarmentArray, FString GarmentID)
{
    Garment* garment = TargetGarmentArray.FindByPredicate([=](Garment& InItem)
    {
        return InItem.GarmentID == GarmentID;
    });
    return garment;
}

void FGarmentCollection::UpdateMeshAndAnimationUsingCombination(TArray<USkeletalMesh*> * TargetMeshes, TArray<UAnimationAsset*> * TargetAnimations, Combination Combination)
{
	TargetMeshes->Empty();
	TargetAnimations->Empty();
	for (const auto& id : Combination.GarmentIDs)
	{
		Garment* garment = GetGarmentByID(LoadedGarments, id);
		if (garment != NULL) //�� ã�� �� ���� (Garment�� �ֽô��� ���� ���)
		{
			TargetMeshes->Add(garment->Meshes[GarmentIndex]);
			TargetAnimations->Add(garment->Animations[GarmentIndex]);
		}
		else
		{
			UE_LOG(SynthesisEngine, Error, TEXT("Search Garment by ID failed - Target ID : %s"), (*id));
		}
	}
}

void FGarmentCollection::UpdateMeshAndAnimationUsingCombination(TArray<USkeletalMesh*> * TargetMeshes, TArray<UAnimationAsset*> * TargetAnimations, TArray<Garment>& TargetGarmentArray, Combination Combination)
{
    TargetMeshes->Empty();
    TargetAnimations->Empty();
    for (const auto& id : Combination.GarmentIDs)
    {
        Garment* garment = GetGarmentByID(TargetGarmentArray ,id);
        if (garment != NULL) //�� ã�� �� ���� (Garment�� �ֽô��� ���� ���)
        {
            for(int i = 0; i < garment->Meshes.Num(); i++)
            {
                TargetMeshes->Add(garment->Meshes[i]);
                TargetAnimations->Add(garment->Animations[i]);
            }
        }
        else
        {
            UE_LOG(SynthesisEngine, Error, TEXT("Search Garment by ID failed - Target ID : %s"), (*id));
        }
    }
}

bool FGarmentCollection::CheckValidation(TArray<USkeletalMesh*> Meshes, TArray<UAnimationAsset*> Animations)
{
	if (Meshes.Num() != Animations.Num())
	{
		UE_LOG(SynthesisEngine, Error, TEXT("Mismatch of Mesh Count and Animation Count"));
		return false;
	}

	for (int i = 0; i < Meshes.Num(); i++)
	{
		FString MeshIdentifier = FUtil::ExtractAlembicIdentifierFromName(Meshes[i]->GetName());
		FString AnimationIdentifier = FUtil::ExtractAlembicIdentifierFromName(Animations[i]->GetName());
		if (MeshIdentifier != AnimationIdentifier)
		{
			UE_LOG(SynthesisEngine, Error, TEXT("Invalid Set appeared. Mesh : %s, Animation %s"), *(Meshes[i]->GetName()), *(Animations[i]->GetName()));
			return false;
		}
	}
	return true;
}

void FGarmentCollection::LoadJsonSetting(FString SkeletalMeshID, TArray<Combination> * LoadedCombinations)
{
	FString RelativeFullPath = FString::Printf(TEXT("/Game/Models/%s_Setting.json"), *SkeletalMeshID);
	FString FullPath = FUtil::Util_GetFullPath(RelativeFullPath);

	const FString JsonFilePath = FullPath;
	FString JsonString;

	FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FString Owner = JsonObject->GetStringField("Owner");
		//UE_LOG(SynthesisEngine, Warning, TEXT("Owner : %s"), *(Owner));
		TArray<TSharedPtr<FJsonValue>> Combinations = JsonObject->GetArrayField("Combinations");
		for (int i = 0; i < Combinations.Num(); i++)
		{
			TArray<TSharedPtr<FJsonValue>> Combination_Json = Combinations[i]->AsArray();
			Combination comb;
			//UE_LOG(SynthesisEngine, Warning, TEXT("Combination : "));
			for (int j = 0; j < Combination_Json.Num(); j++)
			{
				comb.GarmentIDs.Add(Combination_Json[j]->AsString());
				//UE_LOG(SynthesisEngine, Warning, TEXT("    %s"), *(Combination[j]->AsString()));
			}
			(*LoadedCombinations).Add(comb);
		}
	}
}

void FGarmentCollection::LoadAlembics(FString SkeletalMeshID, FString AnimationID, TArray<USkeletalMesh*> * LoadedClothesMesh, TArray<UAnimationAsset*> * LoadedClothesAnimation, TArray<Garment> * LoadedGarments)
{
	FString RelativeFullPath = FString::Printf(TEXT("%s/%s/%s"), *this->BasePath, *SkeletalMeshID, *AnimationID);
	FString FullPath = FUtil::Util_GetFullPath(RelativeFullPath);

	IFileManager& FileManager = IFileManager::Get();
	FString FullPathVerbose = FullPath / TEXT("*");

	TArray<FString> Folders;
	FileManager.FindFiles(Folders, *FullPathVerbose, false, true);

	for (const auto& Folder : Folders)
	{
		TArray<FString> Files;
		FileManager.FindFiles(Files, *(FullPath / Folder + "/*.*"), true, false);
		for (const auto& File : Files)
		{
			FStreamableManager AssetLoader;
			FString TargetAssetFullPath = (RelativeFullPath / Folder / FPaths::GetBaseFilename(File));
			FStringAssetReference AssetRef(TargetAssetFullPath + "." + FPaths::GetBaseFilename(File));
			auto Asset = AssetLoader.LoadSynchronous(AssetRef);

			UAnimSequence* Anim = Cast<UAnimSequence>(Asset);
			if (Anim != NULL)
			{
				(*LoadedClothesAnimation).Add(Anim);
				//UE_LOG(SynthesisEngine, Warning, TEXT("Anim Name : %s"), *(Asset->GetName()));
				continue;
			}
			USkeleton* Skel = Cast<USkeleton>(Asset);
			if (Skel != NULL)
			{
				//UE_LOG(SynthesisEngine, Warning, TEXT("Skel Name : %s"), *(Asset->GetName()));
				continue;
			}
			USkeletalMesh* Mesh = Cast<USkeletalMesh>(Asset);
			if (Mesh != NULL)
			{
				(*LoadedClothesMesh).Add(Mesh);
				//UE_LOG(SynthesisEngine, Warning, TEXT("Mesh Name : %s"), *(Asset->GetName()));
			}
			else
			{
				UE_LOG(SynthesisEngine, Error, TEXT("Only Anim, Skeleton, SkeletalMesh are allowed in this directory"));
			}
		}
	}
	if ((*LoadedClothesMesh).Num() != 0 && (*LoadedClothesAnimation).Num() != 0)
	{
		(*LoadedClothesMesh).Sort();
		(*LoadedClothesAnimation).Sort();

        CheckValidation(*LoadedClothesMesh, *LoadedClothesAnimation);

		int prev_index = 0;

		FString MeshIdentifier = FUtil::ExtractGarmentIdentifierFromFullPath((*LoadedClothesMesh)[0]->GetFullName());
		for (int i = 1; i < (*LoadedClothesMesh).Num(); i++)
		{
			if (MeshIdentifier != FUtil::ExtractGarmentIdentifierFromFullPath((*LoadedClothesMesh)[i]->GetFullName()))
			{				
				Garment* temp = GetGarmentByID((*LoadedGarments), MeshIdentifier);
				if (temp != nullptr)
				{
					for (int j = prev_index; j < i; j++)
					{
						temp->Meshes.Add((*LoadedClothesMesh)[j]);
						temp->Animations.Add((*LoadedClothesAnimation)[j]);
					}
				}
				else
				{
					Garment garment;
					garment.GarmentID = MeshIdentifier;
					for (int j = prev_index; j < i; j++)
					{
						garment.Meshes.Add((*LoadedClothesMesh)[j]);
						garment.Animations.Add((*LoadedClothesAnimation)[j]);
					}
					(*LoadedGarments).Add(garment);
					//UE_LOG(SynthesisEngine, Warning, TEXT("Garment Load : %s"), *MeshIdentifier);
				}
				
				MeshIdentifier = FUtil::ExtractGarmentIdentifierFromFullPath((*LoadedClothesMesh)[i]->GetFullName());
				prev_index = i;
			}
		}
		FString lastID = FUtil::ExtractGarmentIdentifierFromFullPath((*LoadedClothesMesh)[prev_index]->GetFullName());
		Garment* tempLast = GetGarmentByID((*LoadedGarments), lastID);
		if (tempLast != nullptr)
		{
			for (int j = prev_index; j < (*LoadedClothesMesh).Num(); j++)
			{
				tempLast->Meshes.Add((*LoadedClothesMesh)[j]);
				tempLast->Animations.Add((*LoadedClothesAnimation)[j]);
			}
		}
		else
		{
			Garment lastGarment;
			lastGarment.GarmentID = lastID;
			for (int j = prev_index; j < (*LoadedClothesMesh).Num(); j++)
			{
				lastGarment.Meshes.Add((*LoadedClothesMesh)[j]);
				lastGarment.Animations.Add((*LoadedClothesAnimation)[j]);
			}
			//UE_LOG(SynthesisEngine, Warning, TEXT("Garment Load : %s"), *lastGarment.GarmentID);
			(*LoadedGarments).Add(lastGarment);
		}
		
	}
}

/* The Array parameters of this function will be cleared. Be careful to call this function. */
void FGarmentCollection::GetRandomClothesMeshesAndAnimations(FString SkeletalMeshID, FString AnimationID,
TArray<USkeletalMesh*>& ClothesMeshes, TArray<UAnimationAsset*>& ClothesAnimations)
{
    ClothesMeshes.Empty();
    ClothesAnimations.Empty();
    
    TArray<Combination> combinations;
    TArray<USkeletalMesh*> clothesMeshes;
    TArray<UAnimationAsset*> clothesAnimations;
    TArray<Garment> garments;
    
    TArray<USkeletalMesh*> meshesToReturn;
    TArray<UAnimationAsset*> animationsToReturn;
    
    /* 1. Load Json Setting Locally */
    LoadJsonSetting(SkeletalMeshID, &(combinations));
    /* 1-1. There can be empty combinations */
    if(combinations.Num() == 0)
    {
        return;
    }
    
    /* 2. Load Actual Alembic files Locally */
    LoadAlembics(SkeletalMeshID, AnimationID, &(clothesMeshes), &(clothesAnimations), &(garments));
    /* 3. Pick Random Combination. (Combination is initialized on Step 1) And Get Garment */
    int index = rand() % combinations.Num();
    
    /* 4. UpdateMeshAndAnimationUsingCombination */
    UpdateMeshAndAnimationUsingCombination(&ClothesMeshes, &ClothesAnimations, garments, combinations[index]);
}
