// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectLibrary_BP.h"

TArray<UObject*> UObjectLibrary_BP::LoadObjectLibrary(const FString& Path, TSubclassOf<UObject> ObjectClass)
{
	TArray<UObject*> Assets;

	UObjectLibrary * ObjectLibrary = UObjectLibrary::CreateLibrary(ObjectClass, false, GIsEditor);
	if (ObjectLibrary != nullptr)
	{
		ObjectLibrary->AddToRoot();
		FString NewPath = Path;
		int32 NumOfAssetDatas = ObjectLibrary->LoadAssetDataFromPath(NewPath);
		TArray<FAssetData> AssetDatas;
		ObjectLibrary->GetAssetDataList(AssetDatas);

		UObject * AssetTemp;
		for (int i = 0; i < AssetDatas.Num(); i++)
		{
			FAssetData& AssetData = AssetDatas[i];
			AssetTemp = AssetData.GetAsset();
			if (AssetTemp)
			{
				Assets.Add(AssetData.GetAsset());
			}
		}
	}
	return Assets;
}

TArray<UAnimSequence*> UObjectLibrary_BP::LoadAllAnimationSequenceForSkeleton(USkeleton * Skeleton)
{
	TArray<UAnimSequence*> AnimsArray;
	if (Skeleton == NULL) { return AnimsArray; }
	for (TObjectIterator<UAnimationAsset> Itr; Itr; ++Itr)
	{
		if (Skeleton->GetWorld() != (*Itr)->GetWorld()) { continue; }
		if (Skeleton == (*Itr)->GetSkeleton() && (*Itr)->IsA(UAnimSequence::StaticClass()))
		{
			AnimsArray.Add(Cast<UAnimSequence>(*Itr));
		}
	}
	return AnimsArray;
}
