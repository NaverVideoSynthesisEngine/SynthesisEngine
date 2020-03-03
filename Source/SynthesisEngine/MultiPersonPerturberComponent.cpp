// Fill out your copyright notice in the Description page of Project Settings.

#include "MultiPersonPerturberComponent.h"
#include "PhotoRoom.h"

// Sets default values for this component's properties
UMultiPersonPerturberComponent::UMultiPersonPerturberComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FVector UMultiPersonPerturberComponent::GetRandomLocationAroundMainCharacter()
{
    FNavLocation navLoc;
    NavSystem->GetRandomReachablePointInRadius(Owner->GetActorLocation(), MaxRadius, navLoc);
    while(FVector::Distance(navLoc.Location, Owner->GetActorLocation()) < MinRadius)
        NavSystem->GetRandomReachablePointInRadius(Owner->GetActorLocation(), MaxRadius, navLoc);
    
    navLoc.Location.Z -= ZAxisSnap; //Temporal
    return navLoc.Location;
}

// Called when the game starts
void UMultiPersonPerturberComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UMultiPersonPerturberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMultiPersonPerturberComponent::Init(class APhotoRoom* Owner)
{
    this->Owner = Owner;
    this->NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
    int stencilValue = Owner->GetUniqueCustomStencilValueAndIncreaseIt();
    
    SkeletalMesh->bRenderCustomDepth = true;
    SkeletalMesh->CustomDepthStencilValue = stencilValue;
    
    for(int i = 0 ; i < 5; i++)
    {
        Garments[i]->bRenderCustomDepth = true;
        Garments[i]->CustomDepthStencilValue = stencilValue;
    }
}

void UMultiPersonPerturberComponent::SetGarment(int index, FVector location, FRotator rotation, USkeletalMesh* mesh, UAnimationAsset* anim, float pos)
{
    if(!Garments.IsValidIndex(index))
    {
        UE_LOG(SynthesisEngine, Error, TEXT("MultiPerson has only 5 Garment slots"));
        return;
    }
    Garments[index]->SetWorldLocation(location);
    Garments[index]->SetWorldRotation(rotation);
    Garments[index]->SetSkeletalMesh(mesh);
    Garments[index]->PlayAnimation(anim, false);
    Garments[index]->GetSingleNodeInstance()->SetPosition(pos);
    Garments[index]->Stop();

}
FString UMultiPersonPerturberComponent::GetNextGarmentID(TArray<USkeletalMesh*> meshes, int index)
{
    /* Returns nullptr if there is no next or error.*/
    if(!meshes.IsValidIndex(index))
    {
        UE_LOG(SynthesisEngine, Error, TEXT("Input Index is invalid. Can't Get Next Garmnet ID"));
        return FString();
    }
    FString CurrentID = FUtil::ExtractGarmentIdentifierFromFullPath(meshes[index]->GetFullName());
    if(!meshes.IsValidIndex(index+1))
    {
        return FString();
    }
    for (int i = index+1 ; i < meshes.Num(); i++)
    {
        FString NextID = FUtil::ExtractGarmentIdentifierFromFullPath(meshes[i]->GetFullName());
        if(CurrentID != NextID)
        {
            return NextID;
        }
    }
    return FString();
}
void UMultiPersonPerturberComponent::CalculateIndexOfValidMeshUsingPosition(TArray<USkeletalMesh*> meshes, TArray<UAnimationAsset*> anims, float pos, FString GarmentID, int& index, float& clothesPos)
{
    /*
     Returns Index of 'meshes(anims)'
     The meshes[Index] is a mesh that should be used this iteration.
     The anims[Index] is a animation along with human animation position.
    */
    int i = 0;
    while(meshes.IsValidIndex(i) &&FUtil::ExtractGarmentIdentifierFromFullPath(meshes[i]->GetFullName()) != GarmentID)
    { i++; }
    
    if(!meshes.IsValidIndex(i))
    {
        UE_LOG(SynthesisEngine, Error, TEXT("The ID %s is not in TArray"), *GarmentID);
        return;
    }
    
    int j = i;
    for(; j < anims.Num(); j++)
    {
        if(GarmentID != FUtil::ExtractGarmentIdentifierFromFullPath(meshes[j]->GetFullName()))
        {
            /* In case 'pos' is bigger than total frame of all animations */
            index = j-1;
            clothesPos = anims[index]->GetMaxCurrentTime();
            return;
        }
        if(pos - anims[j]->GetMaxCurrentTime() > 0)
        {
            pos -= anims[j]->GetMaxCurrentTime();
        }
        else
        {
            index = j;
            clothesPos = pos;
            return;
        }
    }
    index = j-1;
    clothesPos = anims[index]->GetMaxCurrentTime();
}

void UMultiPersonPerturberComponent::Update()
{
    /* Human and Human Animation */
    USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
    FProgress* CurrentProgress = EngineInstance->Progress;
    
    FVector location = GetRandomLocationAroundMainCharacter();
    FRotator randRot = FRotator(0.0f, FMath::RandRange(0.0f, 180.0f), 0.0f);
    
    USkeletalMesh * randMesh = CurrentProgress->GetRandomSkeletalMesh();
    SkeletalMesh->SetSkeletalMesh(randMesh);
    
    UAnimationAsset * randAnim = CurrentProgress->GetRandomHumanAnimation(randMesh->Skeleton);
    float maxTime = randAnim->GetMaxCurrentTime();
    float randPos = FMath::RandRange(0.0f, maxTime);
    
    SkeletalMesh->PlayAnimation(randAnim, false);
    SkeletalMesh->SetWorldLocation(location);
    SkeletalMesh->SetWorldRotation(randRot);
    SkeletalMesh->SetPosition(randPos);
    SkeletalMesh->Stop();
    
    /* Garment and Garment Animation */
    TArray<USkeletalMesh*> randClothesMeshes;
    TArray<UAnimationAsset*> randClothesAnimations;
    CurrentProgress->GetRandomClothesMeshesAndAnimations(randMesh, randAnim, randClothesMeshes, randClothesAnimations);
    
    for(const auto & garment : Garments) /* Clear Previous Meshes*/
    {
        garment->SetSkeletalMesh(nullptr);
    }
    
    if(randClothesMeshes.Num() == 0 || randClothesAnimations.Num() == 0)
    {
        return;
    }
    
    float pos = SkeletalMesh->GetPosition();
    FString GarmentID = FUtil::ExtractGarmentIdentifierFromFullPath(*randClothesMeshes[0]->GetFullName());
    for(int i = 0; i < Garments.Num(); i++)
    {
        int meshIndex;
        float clothesPos;
        UMaterialInstance * randMat = CurrentProgress->GetRandomMaterial();
        
        CalculateIndexOfValidMeshUsingPosition(randClothesMeshes, randClothesAnimations, pos, GarmentID, meshIndex, clothesPos);
        SetGarment(i, location, randRot, randClothesMeshes[meshIndex], randClothesAnimations[meshIndex], clothesPos);
        for(int j = 0 ; j < Garments[i]->GetNumMaterials(); j++)
        {
            Garments[i]->SetMaterial(j, randMat);
        }
        
        GarmentID = GetNextGarmentID(randClothesMeshes, meshIndex);
        if(GarmentID.IsEmpty())
        {
            break;
        }
    }
}
