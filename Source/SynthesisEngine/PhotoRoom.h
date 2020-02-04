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
	ENABLE_POSTPROCESSVOLUME_STENCIL UMETA(DisplayName = "Enable PostProcessVolume1"),
	FLUSH_MASK_STENCIL UMETA(DisplayName = "Flush stencil mask image"),
	DISABLE_POSTPROCESSVOLUME_STENCIL UMETA(DisplayName = "Disable PostProcessVolume1"),
	ENABLE_POSTPROCESSVOLUME_OCCLUSION UMETA(DisplayName = "Enable PostProcessVolume2"),
	CHANGE_FOLIAGE_SCALE1 UMETA(DisplayName = "Chage foliage actor's scale 0,0,0"),
	FLUSH_MASK_OCCLUSION1 UMETA(DisplayName = "Flush occlusion mask image1"),
	CHANGE_FOLIAGE_SCALE2 UMETA(DisplayName = "Chage foliage actor's scale 1,1,1"),
	FLUSH_MASK_OCCLUSION2 UMETA(DisplayName = "Flush occlusion mask image2"),
	DISABLE_POSTPROCESSVOLUME_OCCLUSION UMETA(DisplayName = "Disable PostProcessVolume2"),
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
	bool EnableDataFlush = true;

	UPROPERTY(EditAnywhere, Category = Option)
	bool PerturbCameraAndMaterialOnStart = true;
    
	UPROPERTY(EditAnywhere, Category = Option)
	int IterationCount = 1;

	UPROPERTY(EditAnywhere, Category = Option)
	int LATE_DATA_FLUSHING_Frame_to_Skip = 2;

	UPROPERTY(EditAnywhere, Category = Option)
	EUpdateProtocol MaterialPerturberUpdateProtocol = EUpdateProtocol::UPDATE_EVERY_FRAME;
	UPROPERTY(EditAnywhere, Category = Option)
	EUpdateProtocol CameraPerturberUpdateProtocol = EUpdateProtocol::UPDATE_EVERY_FRAME;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Garment;
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

public :
	DECLARE_EVENT(APhotoRoom, FProcessDone)
	FProcessDone& OnProcessDone() { return ProcessDoneEvent; }
	FProcessDone ProcessDoneEvent;
};
