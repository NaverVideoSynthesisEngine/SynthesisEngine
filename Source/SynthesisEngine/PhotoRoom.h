// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/PostProcessVolume.h"
#include "MaterialPerturberComponent.h"
#include "CameraPerturberComponent.h"
#include "DataFlushManager.h"
#include "Kismet/GameplayStatics.h"
#include "PhotoRoom.generated.h"

UENUM(BlueprintType)
enum class EUpdateProtocol : uint8
{
	UPDATE_EVERY_FRAME UMETA(DisplayName = "Update Every Frame"),
	UPDATE_CLOTH_IS_CHANGED UMETA(DisaplyName = "Update if cloth is changed"),
	UPDATE_COMBINATION_IS_CHANGED UMETA(DisaplyName = "Update if combination is changed"),
	UPDATE_COMBINATION_ITERATION UMETA(DisaplyName = "Update when combination is being iterated"),
	NONE UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class ECocoUpdatePhase : uint8
{
    /* Due to randome order of taking screenshot. We need to separate steps into five steps. */
	UPDATE_PERTURBERS UMETA(DisplayName = "Update perturbers"),
	FLUSH_IMAGE_AND_KEYPOINTS UMETA(DisplayName = "Flush image and keypoints data first"),
	ENABLE_POSTPROCESSVOLUME UMETA(DisplayName = "Enable PostProcessVolume"),
	FLUSH_MASK UMETA(DisplayName = "Flush mask image"),
	DISABLE_POSTPROCESSVOLUME UMETA(DisplayName = "Disable PostProcessVolume"),
};

enum class ETotalUpdatePhase : uint8
{
	/* Due to randome order of taking screenshot. We need to separate steps into five steps. */
	UPDATE_PERTURBERS UMETA(DisplayName = "Update perturbers"),
	FLUSH_IMAGE_AND_KEYPOINTS UMETA(DisplayName = "Flush image and keypoints data first"),
	ENABLE_POSTPROCESSVOLUME UMETA(DisplayName = "Enable PostProcessVolume"),
	FLUSH_MASK UMETA(DisplayName = "Flush stencil mask image"),
	DISABLE_POSTPROCESSVOLUME UMETA(DisplayName = "Disable PostProcessVolume"),
};

UCLASS()
class SYNTHESISENGINE_API APhotoRoom : public APawn
{
	GENERATED_BODY()

private:
	bool b_FirstUpdate = true;
	bool b_ShouldUpdate;
	int LateDataFlushingCount;
    
    ECocoUpdatePhase UpdatePhase_COCO;
	ETotalUpdatePhase UpdatePhase_TOTAL;

	int FixedCameraIndex = 0;
	int IterationIndex = 0;
    
    // For unique custom depth value;
    int MultiPerson_CustomDepthID = 0;

	TArray<USceneComponent*> CameraPoints;
public:
	// Sets default values for this pawn's properties
	APhotoRoom();

	UPROPERTY(EditAnywhere, Category = Debug)
	bool EnableCameraPerturber = true;
	UPROPERTY(EditAnywhere, Category = Debug)
	bool EnableAnimationPerturber = true;
	UPROPERTY(EditAnywhere, Category = Debug)
	bool EnableMaterialPerturber = true;
    UPROPERTY(EditAnywhere, Category = Debug)
    bool EnableMultiPersonPerturber = true;
	UPROPERTY(EditAnywhere, Category = Debug)
	bool EnableDataFlush = true;

	UPROPERTY(EditAnywhere, Category = Option)
	bool PerturbCameraAndMaterialOnStart = true;

	UPROPERTY(EditAnywhere, Category = Option)
	int IterationCount = 1;

	UPROPERTY(EditAnywhere, Category = Option)
	int LATE_DATA_FLUSHING_Frame_to_Skip = 2;
    
    UPROPERTY(EditAnywhere, Category = Option)
    int MultiPersonCount = 3;

	UPROPERTY(EditAnywhere, Category = Option)
	EUpdateProtocol MaterialPerturberUpdateProtocol = EUpdateProtocol::UPDATE_EVERY_FRAME;
	UPROPERTY(EditAnywhere, Category = Option)
	EUpdateProtocol CameraPerturberUpdateProtocol = EUpdateProtocol::UPDATE_EVERY_FRAME;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, Category = SynthesisEngine)
	class UAnimationPerturberComponent* AnimationPerturber;
	UPROPERTY(VisibleAnywhere, Category = SynthesisEngine)
	UCameraPerturberComponent* CameraPerturber;
	UPROPERTY(VisibleAnywhere, Category = SynthesisEngine)
	UMaterialPerturberComponent* MaterialPerturber;

	class FDataFlushManager* DataFlushManager;

public: //Multiple Clothes
	static const int GarmentSocketNumber;
	UPROPERTY(VisibleAnywhere)
	TArray<USkeletalMeshComponent*> Garments;
    
    static const int MultiPersonSocketNumber;
    UPROPERTY(VisibleAnywhere)
    TArray<class UMultiPersonPerturberComponent*> MultiPeople;

	static const FString WindowPath;
	static const FString MacPath;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
public:
	virtual void PostInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Update();
	void UpdateWithLateDataFlushing();
    void UpdateWithLateDataFlushing_COCOTEMP();
    void UpdateWithLateDataFlushing_MPITEMP();
	void UpdateWithLateDataFlushing_TOTAL();
	void UpdateWithLateDataFlushing_TOTAL_FIXEDCAMERA();
    
	void Skip3DModel();
	void Skip3DModelAnimation();
	void SkipClothes();

	bool CheckIteration();
    
    /* 2020.03.03 To Assign Uniqu Stencil Value to MultiPerson */
    int GetUniqueCustomStencilValueAndIncreaseIt();

public :
	DECLARE_EVENT(APhotoRoom, FProcessDone)
	FProcessDone& OnProcessDone() { return ProcessDoneEvent; }
	FProcessDone ProcessDoneEvent;
};
