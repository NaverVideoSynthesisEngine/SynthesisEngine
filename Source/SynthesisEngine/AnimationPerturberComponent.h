// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Progress.h"
#include "Components/SceneComponent.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimSequence.h"
#include "Runtime/Engine/Classes/Animation/AnimSingleNodeInstance.h"
#include "AnimationPerturberComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SYNTHESISENGINE_API UAnimationPerturberComponent : public USceneComponent
{
	GENERATED_BODY()
private:
	class APhotoRoom* Owner;

	USkeletalMesh* CurrentSkeletalMesh;
	UAnimationAsset* CurrentAnimationAsset;
	USkeletalMesh* CurrentClothesMesh;
	UAnimationAsset* CurrentClothesAnimationAsset;

	bool Check3DModel(FProgress* CurrentProgress);
	void Check3DModelAnimation(FProgress* CurrentProgress);
	void CheckClothes(FProgress* CurrentProgress);

	void ResetAnimationFields();
	void ResetClothesField();

	bool IsCurrentAnimationReachedEnd() const;
	bool IsCurrentClothesAnimationReachedEnd() const;

	bool NextFrame();
	bool NextFrameWithFixedFrameRate();
	bool BackFrame();

	float ClothAnimationOffset; //<0914> What is this?
private: //New features
	TArray<USkeletalMesh*> CurrentClothesMeshes;
	TArray<UAnimationAsset*> CurrentClothesAnimationAssets;

	void CheckClothes_MC(FProgress* CurrentProgress);
	void ResetClothesField_MC();

	bool NextFrame_MC();
	bool IsCurrentClothesAnimationReachedEnd_MC() const;

private: //New features 10.31 : Multiple Combination
	void ResetHumanAnimation();

public:
	// Sets default values for this component's properties
	UAnimationPerturberComponent();

	UPROPERTY()
	USkeletalMeshComponent* GarmentMesh;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMesh;

public: //New features ? : Skip
	UPROPERTY()
	TArray<USkeletalMeshComponent*> GarmentMeshes;

	void Skip3DModel();
	void Skip3DModelAnimation();
	void SkipClothes();

protected:

public:
	void OnChangedEnableProperty(bool IsEnable);
	void Init(USkeletalMeshComponent* SkeletalMesh, USkeletalMeshComponent* Garment); //For Single Clothes
	void Init(class APhotoRoom* Owner, USkeletalMeshComponent* SkeletalMesh, TArray<USkeletalMeshComponent*> Garments); //Multiple Clothes
	void Update();

public:
	DECLARE_EVENT(UAnimationPerturberComponent, FProcessDone)
	DECLARE_EVENT(UAnimationPerturberComponent, FHumanChangedEvent)
	DECLARE_EVENT(UAnimationPerturberComponent, FHumanAnimationChangedEvent)
	DECLARE_EVENT(UAnimationPerturberComponent, FClothChangedEvent)
	DECLARE_EVENT(UAnimationPerturberComponent, FGarmentCombinationChangedEvent)
	DECLARE_EVENT(UAnimationPerturberComponent, FGarmentCombinationIterationEvent)

	FProcessDone& OnProcessDone() { return ProcessDoneEvent; }
	FHumanChangedEvent& OnHumanChanged() { return HumanChangedEvent; }
	FHumanAnimationChangedEvent& OnHumanAnimationChanged() { return HumanAnimationChangedEvent; }
	FClothChangedEvent& OnClothChanged() { return ClothChangedEvent; }
	FGarmentCombinationChangedEvent& OnGarmentCombinationChanged() { return GarmentCombinationChangedEvent; }
	FGarmentCombinationIterationEvent& OnGarmentCombinationIteration() { return GarmentCombinationIterationEvent; }
private:
	FProcessDone ProcessDoneEvent;
	FHumanChangedEvent HumanChangedEvent;
	FHumanAnimationChangedEvent HumanAnimationChangedEvent;
	FClothChangedEvent ClothChangedEvent;
	FGarmentCombinationChangedEvent GarmentCombinationChangedEvent;
	FGarmentCombinationIterationEvent GarmentCombinationIterationEvent;
};
