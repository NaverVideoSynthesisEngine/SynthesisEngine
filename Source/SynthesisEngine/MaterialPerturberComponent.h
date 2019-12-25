// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Progress.h"
#include "Util.h"
#include "Components/ActorComponent.h"
#include "MaterialPerturberComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SYNTHESISENGINE_API UMaterialPerturberComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	TArray<UMaterialInterface*> SkeletalOriginalMaterials;
	TArray<UMaterialInterface*> GarmentOriginalMaterials;

	USkeletalMeshComponent* SkeletalMesh;
	USkeletalMeshComponent* GarmentMesh;

	void GetMaterialList();
public:	
	UMaterialPerturberComponent();

protected:
	virtual void BeginPlay() override;

public:	
	void Init(USkeletalMeshComponent* SkeletalMesh, USkeletalMeshComponent* Garment);
	void OnChangedEnableProperty(bool IsEnable);
	void Update();
	
public: //New features : Multiple Clothes
	void Init(USkeletalMeshComponent* SkeletalMesh, TArray<USkeletalMeshComponent*> Garments);
	UPROPERTY()
	TArray<USkeletalMeshComponent*> GarmentMeshes;

private: //New features : Multiple Clothes
	TArray<USkeletalMesh*> CurrentClothesMeshes;
	void UpdateUsingMaterialSetting();
};
