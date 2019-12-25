// Fill out your copyright notice in the Description page of Project Settings.


#include "Util.h"

FUtil::FUtil()
{
}

FUtil::~FUtil()
{
}

///Input : a full path of Garments
///Output : a ID of Garment
///Ex) Input : 'Game/Garments/Sleeveless/Pointing_Mesh_0_99' Output : 'Sleeveless'
FString FUtil::ExtractGarmentIdentifierFromFullPath(const FString& FullPath)
{
	TArray<FString> Splited;
	FullPath.ParseIntoArray(Splited, TEXT("/"));
	return Splited[Splited.Num() - 2];
}

///Input : a full path of Materials
///Output : a Category of Material
///Ex) Input : 'Game/Materials/Fabric/Cotton' Output : 'Fabric'
FString FUtil::ExtractMaterialCategoryFromFullPath(const FString& FullPath)
{
	TArray<FString> Splited;
	FullPath.ParseIntoArray(Splited, TEXT("/"));
	return Splited[Splited.Num() - 2];
}

///Input : a name of Clothes USkeletalMesh or Clothes UAnimationAsset
///Output : a ID of Clothes
///Ex) Input : 'Pointing_0_99' Output : 'Pointing' / Input : 'Pointing_Animation_0_99' Output : 'Pointing'
FString FUtil::ExtractAlembicIdentifierFromName(const FString& Name)
{
	TArray<FString> Splited;
	Name.ParseIntoArray(Splited, TEXT("_"));
	return Splited[0];
}

///Input : a path begins with '/Game'
///Output : Absoulte path of input path. substitute '/Game'.
///Ex) Input : '/Game/Animation' Output : 'D:\\Unreal Project\\Contents\\Animation'
FString FUtil::Util_GetFullPath(const FString& relativePath)
{
	TArray<FString> Splited;
	relativePath.ParseIntoArray(Splited, TEXT("/"), false);
	if (Splited[1] != TEXT("Game"))
	{
		UE_LOG(SynthesisEngine, Error, TEXT("Relative Path should begin with '/Game' "));
		return FString("");
	}

	FString str(FPaths::GameContentDir());
	Splited.RemoveAt(0);
	Splited.RemoveAt(0);
	for (const auto& concat : Splited)
	{
		str = str / concat;
	}
	return str;
}