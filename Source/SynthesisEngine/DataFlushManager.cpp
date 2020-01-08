// Fill out your copyright notice in the Description page of Project Settings.


#include "DataFlushManager.h"

FDataFlushManager::FDataFlushManager(AActor * Owner, UWorld* World, UCameraComponent* CameraComponent, APostProcessVolume* PostProcessVolume)
	:world(World), cameraComponent(CameraComponent), owner(Owner), postProcessVolume(PostProcessVolume)
{
	dataID = 0;
}

FDataFlushManager::~FDataFlushManager()
{
}

EJointVisibility FDataFlushManager::VisibilityCheck(FVector location, float threshold)
{
	FCollisionQueryParams collisionParam;
	FHitResult result;
	FVector start(cameraComponent->GetComponentLocation());
    
    const APlayerController* const PlayerController = Cast<const APlayerController>(UGameplayStatics::GetPlayerController(world, 0));

    FVector2D ScreenLocation;
    PlayerController->ProjectWorldLocationToScreen(location, ScreenLocation);
    
    int32 ScreenWidth = 0;
    int32 ScreenHeight = 0;
    PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);

    int32 ScreenX = (int32)ScreenLocation.X;
    int32 ScreenY = (int32)ScreenLocation.Y;

    if(!(ScreenX >= 0 && ScreenY >= 0 && ScreenX < ScreenWidth && ScreenY < ScreenHeight))
    {
        return EJointVisibility::OUT_OF_SIGHT;
    }
    
	if (world->LineTraceSingleByChannel(result, start, location, ECC_Visibility, collisionParam))
	{
        FVector hitPoint = result.Location;
        
        if((hitPoint-location).Size() < threshold)
        {
//            UE_LOG(SynthesisEngine, Warning, TEXT("Visible"));
            return EJointVisibility::VISIBLE;
        }
//		UE_LOG(SynthesisEngine, Warning, TEXT("Occluded"));
        USkeletalMesh * temp = Cast<USkeletalMesh> (result.GetActor()->GetComponentByClass(USkeletalMesh::StaticClass()));
        if(temp != NULL)
        {
            EJointVisibility::OCCLUDED_BY_HUMAN;
        }
		return EJointVisibility::OCCLUDED_BY_OBJECT;
	}
	else
	{
//		UE_LOG(SynthesisEngine, Warning, TEXT("None hit, Visible"));
		return EJointVisibility::VISIBLE;
	}
}

void FDataFlushManager::EnablePostProcessVolume(bool enable)
{
    postProcessVolume->bEnabled = enable;
}

void FDataFlushManager::FlushToDataCocoFormat(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent* Mesh, UCameraComponent* CameraComponent)
{
    FString fileName = FString::Printf(TEXT("%012d"),dataID);
	FString screenshotPath = *FString::Printf(TEXT("%s/%s/%s.png"), *path, *LevelName, *fileName);
	FString jsonPath = *FString::Printf(TEXT("%s/%s/%s.json"), *path, *LevelName, *fileName);
    
	FScreenshotRequest::RequestScreenshot(screenshotPath, false, false);
	FCocoAnnotation coco(this, world, Mesh, dataID);

	FString jsonstr;
	FJsonObjectConverter::UStructToJsonObjectString(coco, jsonstr);
	FFileHelper::SaveStringToFile(*jsonstr, *jsonPath);
}

void FDataFlushManager::FlushToDataCocoFormat_MASK(FString path, FString LevelName, FString ActorLabel, USkeletalMeshComponent* Mesh, UCameraComponent* CameraComponent)
{
    if(!postProcessVolume->bEnabled)
    {
        UE_LOG(SynthesisEngine, Warning, TEXT("PostProcessVolume is disabled. Please turn on before flushing mask"));
    }
    FString fileName = FString::Printf(TEXT("%012d"),dataID);
    FString maskPath = *FString::Printf(TEXT("%s/%s/%s_m.png"), *path, *LevelName, *fileName);
    
    FScreenshotRequest::RequestScreenshot(maskPath, false, false);
    
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
