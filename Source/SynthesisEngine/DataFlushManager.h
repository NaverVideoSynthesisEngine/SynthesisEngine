// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Engine/PostProcessVolume.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Core/Public/Templates/SharedPointer.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
//#include "DrawDebugHelpers.h"
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


class SYNTHESISENGINE_API FDataFlushManager
{
private:
	int dataID;

	UWorld* world;
	UCameraComponent* cameraComponent;
	AActor * owner;
    APostProcessVolume* postProcessVolume;
public:
	FDataFlushManager(AActor * Owner, UWorld * World, UCameraComponent* CameraComponent, APostProcessVolume* PostProcessVolume);
	~FDataFlushManager();
    
    EJointVisibility VisibilityCheck(FVector location, float threshold);
    void EnablePostProcessVolume(bool enable);
	void FlushToDataCocoFormat(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent* Mesh, UCameraComponent* CameraComponent);
    void FlushToDataCocoFormat_MASK(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent* Mesh, UCameraComponent* CameraComponent);
	void FlushToData(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent * Mesh, UCameraComponent* CameraComponent);
	void OnChangedEnableProperty(bool IsEnable);
};

USTRUCT()
struct FDummyAnnotation
{
GENERATED_BODY()
private:
    static FString Skeleton(int index)
    {
        FString VanillaMixamo[] = { TEXT("Hips"), TEXT("Spine"),TEXT("Spine1"), TEXT("Spine2"),
            TEXT("LeftShoulder"), TEXT("LeftArm"), TEXT("LeftForeArm"),
            TEXT("RightShoulder"), TEXT("RightArm"), TEXT("RightForeArm"),
            TEXT("Neck"), TEXT("Head"),
            TEXT("LeftUpLeg"), TEXT("LeftLeg"), TEXT("LeftFoot"),
            TEXT("RightUpLeg"), TEXT("RightLeg"), TEXT("RightFoot"),
        };
        return VanillaMixamo[index];
    }
    static int SkeletonNum()
    {
        return 18;
    }
public:
    UPROPERTY(EditAnywhere)
    int FrameNum;
    UPROPERTY(EditAnywhere)
    TArray<FJoint> Joints;
    UPROPERTY(EditAnywhere)
    FVector TComp;
    UPROPERTY(EditAnywhere)
    FRotator RComp;
    
    FDummyAnnotation() {}
    FDummyAnnotation(FDataFlushManager * FlushManager, UWorld * World, USkeletalMeshComponent* Mesh, UCameraComponent* Camera, int dataID)
    {
        TArray<FJoint> Joints;
        for (int j = 0; j < SkeletonNum(); j++)
        {
            FVector location = Mesh->GetSocketLocation(FName(*Skeleton(j)));
            FJoint joint;
            joint.JointType = j;
            joint.x3D = location.X;
            joint.y3D = location.Y;
            joint.z3D = location.Z;

            Joints.Add(joint);
        }

        this->FrameNum = dataID;
        this->Joints = Joints;
        this->TComp = Camera->GetComponentLocation();
        this->RComp = Camera->GetComponentRotation();
    }
};

USTRUCT()
struct FCocoAnnotation
{
GENERATED_BODY()
private:
    static float CocoSkeletonThreshold(int index) {
        float threshold[] = { 10.0f, 10.0f, 10.0f,
            10.0f, 10.0f,
            10.0f, 15.0f, 10.0f,
            10.0f, 15.0f, 10.0f,
            10.0f, 10.0f, 10.0f,
            10.0f, 10.0f, 10.0f,
        };
        return threshold[index];
    }
    static FString CocoSkeleton(int index) {
      FString skeleton[] = { TEXT("Nose"), TEXT("LeftEye"),TEXT("RightEye"),
          TEXT("LeftEar"), TEXT("RightEar"),
          TEXT("LeftArm"), TEXT("LeftForeArm"), TEXT("LeftHand"),
          TEXT("RightArm"), TEXT("RightForeArm"), TEXT("RightHand"),
          TEXT("LeftUpLeg"), TEXT("LeftLeg"), TEXT("LeftFoot"),
          TEXT("RightUpLeg"), TEXT("RightLeg"), TEXT("RightFoot"),
      };
    /*
        "nose", Nose
        "left_eye", LeftEye
        "right_eye", RightEye
        "left_ear", LeftEar
        "right_ear", RightEar
        "left_shoulder", LeftArm
        "right_shoulder", RightArm
        "left_elbow", LeftForeArm
        "right_elbow", RightForeArm
        "left_wrist", LeftHand
        "right_wrist", RightHand
        "left_hip", LeftUpLeg
        "right_hip", RightUpLeg
        "left_knee", LeftLeg
        "right_knee", RightLeg
        "left_ankle", LeftFoot
        "right_ankle" RightFoot
    */
        
      return skeleton[index];
    }
    static int CocoSkeletonNum(){
        return 17;
    }
    
    static int ConvertToCocoVisibility(EJointVisibility visibility)
    {
        switch(visibility)
        {
            case EJointVisibility::VISIBLE :
                return 2;
            case EJointVisibility::OCCLUDED_BY_HUMAN:
            case EJointVisibility::OCCLUDED_BY_OBJECT:
                return 1;
            case EJointVisibility::OUT_OF_SIGHT:
                return 0;
        }
        UE_LOG(SynthesisEngine, Warning, TEXT("FlushToData :: Error on Coco Format"));
        return -1;
    }
public:
    UPROPERTY(EditAnywhere)
    int stencil_value;
    UPROPERTY(EditAnywhere)
    int num_keypoints;
    UPROPERTY(EditAnywhere)
    TArray<int> keypoints;
    
    FCocoAnnotation(){};
    FCocoAnnotation(FDataFlushManager * FlushManager, UWorld * World, USkeletalMeshComponent* Mesh, int dataID)
    {
        for (int j = 0; j < CocoSkeletonNum(); j++)
        {
            FVector location = Mesh->GetSocketLocation(FName(*CocoSkeleton(j)));
            FVector2D screenCoord;
            UGameplayStatics::GetPlayerController(World, 0)->ProjectWorldLocationToScreen(location, screenCoord, true);

            this->keypoints.Add((int)screenCoord.X);
            this->keypoints.Add((int)screenCoord.Y);
            this->keypoints.Add(this->ConvertToCocoVisibility(FlushManager->VisibilityCheck(location, CocoSkeletonThreshold(j))));
        }
        this->num_keypoints = CocoSkeletonNum();
    }
};
