// Fill out your copyright notice in the Description page of Project Settings.


#include "DataFlushManager.h"

FDataFlushManager::FDataFlushManager(AActor * Owner, UWorld* World, UCameraComponent* CameraComponent)
	:world(World), cameraComponent(CameraComponent), owner(Owner)
{
	dataID = 0;
	/*TargetJoints.Append({ TEXT("root"), TEXT("pelvis"),TEXT("spine_01"), TEXT("spine_02"), TEXT("spine_03"),
		TEXT("clavicle_l"), TEXT("upperarm_l"), TEXT("lowerarm_l"),
		TEXT("clavicle_r"), TEXT("upperarm_r"), TEXT("lowerarm_r"),
		TEXT("neck_01"), TEXT("head"),
		TEXT("thigh_l"), TEXT("calf_l"), TEXT("foot_l"),
		TEXT("thigh_r"), TEXT("calf_r"), TEXT("foot_r"),
		});*/
	
	FString VanillaMixamo[] = { TEXT("Hips"), TEXT("Spine"),TEXT("Spine1"), TEXT("Spine2"),
		TEXT("LeftShoulder"), TEXT("LeftArm"), TEXT("LeftForeArm"),
		TEXT("RightShoulder"), TEXT("RightArm"), TEXT("RightForeArm"),
		TEXT("Neck"), TEXT("Head"),
		TEXT("LeftUpLeg"), TEXT("LeftLeg"), TEXT("LeftFoot"),
		TEXT("RightUpLeg"), TEXT("RightLeg"), TEXT("RightFoot"),
	};
	TargetJoints.Append(VanillaMixamo, ARRAY_COUNT(VanillaMixamo));
}

FDataFlushManager::~FDataFlushManager()
{
}

EJointVisibility FDataFlushManager::VisibilityCheck(FVector location)
{
	FCollisionQueryParams collisionParam;
	//collisionParam.AddIgnoredActor(owner);
	FHitResult result;
	FVector start(cameraComponent->GetComponentLocation());

	if (world->LineTraceSingleByChannel(result, start, location, ECC_Visibility, collisionParam))
	{
		//UE_LOG(SynthesisEngine, Warning, TEXT("Occluded"));
		return EJointVisibility::OCCLUDED_BY_OBJECT;
	}
	else
	{
		//UE_LOG(SynthesisEngine, Warning, TEXT("Visible"));
		return EJointVisibility::VISIBLE;
	}
}

void FDataFlushManager::FlushToDataCocoFormat(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent* Mesh, UCameraComponent* CameraComponent)
{

	FString screenshotPath = *FString::Printf(TEXT("%s/%s/%s_%d.png"), *path, *LevelName, *ActorLabel, dataID);
	FString jsonPath = *FString::Printf(TEXT("%s/%s/%s_%d.json"), *path, *LevelName, *ActorLabel, dataID);
    UE_LOG(SynthesisEngine, Warning, TEXT("screenshot : %s"), *screenshotPath);
    UE_LOG(SynthesisEngine, Warning, TEXT("json : %s"), *jsonPath);
    
	FScreenshotRequest::RequestScreenshot(screenshotPath, false, false);
	FCocoAnnotation coco(this, world, Mesh, dataID);

	FString jsonstr;
	FJsonObjectConverter::UStructToJsonObjectString(coco, jsonstr);
	FFileHelper::SaveStringToFile(*jsonstr, *jsonPath);
    
    
	dataID++;
}

void FDataFlushManager::FlushToData(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent * Mesh, UCameraComponent* CameraComponent)
{
	FString screenshotPath = *FString::Printf(TEXT("%s/%s/%s_%d.png"), *path, *LevelName, *ActorLabel, dataID);
	FString jsonPath = *FString::Printf(TEXT("%s/%s/%s_%d.json"), *path, *LevelName, *ActorLabel, dataID);
	// lll

	FScreenshotRequest::RequestScreenshot(screenshotPath, false, false);
	
	TArray<FJoint> Joints;
	for (int j = 0; j < TargetJoints.Num(); j++)
	{
		FVector location = Mesh->GetSocketLocation(FName(*TargetJoints[j]));
		FJoint joint;
		joint.JointType = j;
		joint.x3D = location.X;
		joint.y3D = location.Y;
		joint.z3D = location.Z;

		//VisibilityCheck(location);
		Joints.Add(joint);
	}

	FAnnotation annotation;
	annotation.FrameNum = dataID;
	annotation.Joints = Joints;
	annotation.TComp = CameraComponent->GetComponentLocation();
	annotation.RComp = CameraComponent->GetComponentRotation();


	FString jsonstr;
	FJsonObjectConverter::UStructToJsonObjectString(annotation, jsonstr);
	FFileHelper::SaveStringToFile(*jsonstr, *jsonPath);
	
	dataID++;
}

void FDataFlushManager::OnChangedEnableProperty(bool IsEnable)
{
}
