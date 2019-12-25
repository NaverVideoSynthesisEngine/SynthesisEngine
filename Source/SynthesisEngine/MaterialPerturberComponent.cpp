// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialPerturberComponent.h"

// Sets default values for this component's properties
UMaterialPerturberComponent::UMaterialPerturberComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UMaterialPerturberComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMaterialPerturberComponent::Init(USkeletalMeshComponent* SkeletalMesh, USkeletalMeshComponent* Garment)
{
	this->SkeletalMesh = SkeletalMesh;
	this->GarmentMesh = Garment;

	this->SkeletalOriginalMaterials = SkeletalMesh->GetMaterials();
	this->GarmentOriginalMaterials = SkeletalMesh->GetMaterials();
}

void UMaterialPerturberComponent::Init(USkeletalMeshComponent* SkeletalMesh, TArray<USkeletalMeshComponent*> Garments)
{
	this->SkeletalMesh = SkeletalMesh;
	this->GarmentMeshes = Garments;

	this->SkeletalOriginalMaterials = SkeletalMesh->GetMaterials();
}

void UMaterialPerturberComponent::OnChangedEnableProperty(bool IsEnable)
{
	UE_LOG(SynthesisEngine, Warning, TEXT("bool changecd : %d"), IsEnable);
}

void UMaterialPerturberComponent::Update()
{
	//GetMaterialList();
	UpdateUsingMaterialSetting();
}

void UMaterialPerturberComponent::GetMaterialList()
{
	USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
	FProgress* CurrentProgress = EngineInstance->Progress;

	int SkeletalMeshMaterialSlotNum = SkeletalMesh->GetNumMaterials();
	int GarmentMeshMaterialSlotNum = GarmentMesh->GetNumMaterials();

	for (int i = 0; i < SkeletalMeshMaterialSlotNum; i++)
	{
		SkeletalMesh->SetMaterial(i, CurrentProgress->GetRandomMaterial());
	}
	for (int i = 0; i < GarmentMeshMaterialSlotNum; i++)
	{
		GarmentMesh->SetMaterial(i, CurrentProgress->GetRandomMaterial());
	}
}

void UMaterialPerturberComponent::UpdateUsingMaterialSetting()
{
	USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
	FProgress* CurrentProgress = EngineInstance->Progress;

	CurrentClothesMeshes = CurrentProgress->GetCurrentClothesMeshes();
	for (int i = 0; i < CurrentClothesMeshes.Num(); i++)
	{
		int GarmentMatSlotNum = GarmentMeshes[i]->GetNumMaterials();
		for (int j = 0; j < GarmentMatSlotNum; j++)
		{
			GarmentMeshes[i]->SetMaterial(j, CurrentProgress->GetRandomMaterialFollowingSetting(
				FUtil::ExtractGarmentIdentifierFromFullPath(CurrentClothesMeshes[i]->GetFullName()), j));
		}
	}
}