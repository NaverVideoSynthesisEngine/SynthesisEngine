// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "DataFlushManager.generated.h"
/**
 * 
 */

UENUM(BlueprintType)
enum class EJointVisibility : uint8
{
	OUT_OF_SIGHT  UMETA(DisplayName = "Out of view frustrum"),
	OCCLUDED_BY_HUMAN  UMETA(DisplayName = "Occluded by other character"),
	OCCLUDED_BY_OBJECT  UMETA(DisplayName = "Occluded by object"),
	VISIBLE  UMETA(DisplayName = "Visible")
};

USTRUCT()
struct FJoint
{
GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int JointType;
	UPROPERTY(EditAnywhere)
	float x3D;
	UPROPERTY(EditAnywhere)
	float y3D;
	UPROPERTY(EditAnywhere)
	float z3D;
};

USTRUCT()
struct FAnnotation
{
GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	int FrameNum;
	UPROPERTY(EditAnywhere)
	TArray<FJoint> Joints;
	UPROPERTY(EditAnywhere)
		FVector TComp;
	UPROPERTY(EditAnywhere)
		FRotator RComp;
};

USTRUCT()
struct FCocoAnnotation
{
GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<int> keypoints;
};

class SYNTHESISENGINE_API FDataFlushManager
{
private:
	TArray<FString> TargetJoints;
	int dataID;

	UWorld* world;
	UCameraComponent* cameraComponent;
	AActor * owner;

	int GetCocoVisibility(EJointVisibility visibility);
	EJointVisibility VisibilityCheck(FVector location);
public:
	FDataFlushManager(AActor * Owner, UWorld * World, UCameraComponent* CameraComponent);
	~FDataFlushManager();

	void FlushToDataCocoFormat(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent* Mesh, UCameraComponent* CameraComponent);
	void FlushToData(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent * Mesh, UCameraComponent* CameraComponent);
	void OnChangedEnableProperty(bool IsEnable);
};
