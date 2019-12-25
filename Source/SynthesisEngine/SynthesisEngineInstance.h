// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Engine/GameInstance.h"
#include "Progress.h"
#include "Kismet/GameplayStatics.h"
#include "SynthesisEngineInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EENGINE_MODE : uint8
{
	START_FROM_SCRATCH UMETA(DisplayName = "StartFromScratch"),
	RE_START UMETA(DisaplyName = "ReStart")
};

UCLASS()
class SYNTHESISENGINE_API USynthesisEngineInstance : public UGameInstance
{
	GENERATED_BODY()
private :
    void InitPaths();
	void InitRefers();
	void InitProgressAndCursor();
    
    class APhotoRoomManager * PhotoRoomManager;
public :
	USynthesisEngineInstance(const FObjectInitializer& ObjectInitializer);

	void RegisterPhotoRoomManager(class APhotoRoomManager* PhotoRoomManager);
	void ReleasePhotoRoomManager(class APhotoRoomManager* PhotoRoomManager);
public :
	TArray<FName> LevelsToLoad;
	
	bool b_UpdateAuto;
	int LevelIndex;
public :
	UPROPERTY(VisibleAnywhere, Category = SynthesisEngine)
	TArray<USkeletalMesh*> AllModels;

	UPROPERTY(VisibleAnywhere, Category = SynthesisEngine)
	TArray<UAnimationAsset*> AllAnimations;

	UPROPERTY(VisibleAnywhere, Category = SynthesisEngine)
	TArray<UMaterialInstance*> AllMaterials;

	UPROPERTY(EditAnywhere, Category = SynthesisEngine)
	FString ModelPath;

	UPROPERTY(EditAnywhere, Category = SynthesisEngine)
	FString AnimationPath;
	
	UPROPERTY(EditAnywhere, Category = SynthesisEngine)
	FString MaterialPath;

	UPROPERTY(EditAnywhere, Category = SynthesisEngine)
	FString ClothesAnimationPath;

	UPROPERTY(EditAnywhere, Category = SynthesisEngine)
	EENGINE_MODE EngineMode;

	class FProgress* Progress;
    
};



