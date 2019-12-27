// Fill out your copyright notice in the Description page of Project Settings.


#include "DataFlushManager.h"

FDataFlushManager::FDataFlushManager(AActor * Owner, UWorld* World, UCameraComponent* CameraComponent)
	:world(World), cameraComponent(CameraComponent), owner(Owner)
{
	dataID = 0;
}

FDataFlushManager::~FDataFlushManager()
{
}

EJointVisibility FDataFlushManager::VisibilityCheck(FVector location, float threshold, FVector jointLocation)
{
	FCollisionQueryParams collisionParam;
	FHitResult result;
	FVector start(cameraComponent->GetComponentLocation());

	if (world->LineTraceSingleByChannel(result, start, location, ECC_Visibility, collisionParam))
	{
        FVector hitPoint = result.Location;
        if((hitPoint-jointLocation).Size() < threshold)
        {
            //UE_LOG(SynthesisEngine, Warning, TEXT("Visible"));
            return EJointVisibility::VISIBLE;
        }
		//UE_LOG(SynthesisEngine, Warning, TEXT("Occluded"));
        USkeletalMesh * temp = Cast<USkeletalMesh> (result.GetActor()->GetComponentByClass(USkeletalMesh::StaticClass()));
        if(temp != NULL)
        {
            EJointVisibility::OCCLUDED_BY_HUMAN;
        }
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

	FScreenshotRequest::RequestScreenshot(screenshotPath, false, false);
    FDummyAnnotation annotation(this, world, Mesh, CameraComponent, dataID);

	FString jsonstr;
	FJsonObjectConverter::UStructToJsonObjectString(annotation, jsonstr);
	FFileHelper::SaveStringToFile(*jsonstr, *jsonPath);
	dataID++;
}

void FDataFlushManager::OnChangedEnableProperty(bool IsEnable)
{
}
