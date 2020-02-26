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
    
    SkeletalMesh->bRenderCustomDepth = true;
    SkeletalMesh->CustomDepthStencilValue = Owner->SkeletalMesh->CustomDepthStencilValue;
    
    for(int i = 0 ; i < 5; i++)
    {
        Garments[i]->bRenderCustomDepth = true;
        Garments[i]->CustomDepthStencilValue = Owner->Garments[i]->CustomDepthStencilValue;
    }
}

void UMultiPersonPerturberComponent::Update()
{
    USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
    FProgress* CurrentProgress = EngineInstance->Progress;
    
    FVector location = GetRandomLocationAroundMainCharacter();
    FRotator randRot = FRotator(0.0f, FMath::RandRange(0.0f, 180.0f), 0.0f);
    
    UAnimationAsset * randAnim = CurrentProgress->GetRandomHumanAnimation();
    float maxTime = randAnim->GetMaxCurrentTime();
    float randPos = FMath::RandRange(0.0f, maxTime);
    
    USkeletalMesh * randMesh = CurrentProgress->GetRandomSkeletalMesh();
    SkeletalMesh->SetSkeletalMesh(randMesh);
    SkeletalMesh->PlayAnimation(randAnim, false);
    SkeletalMesh->SetPosition(FMath::RandRange(0.0f, maxTime));
    SkeletalMesh->Stop();
    SkeletalMesh->SetWorldLocation(location);
    SkeletalMesh->SetWorldRotation(randRot);
    
    TArray<USkeletalMesh*> randClothesMeshes;
    TArray<UAnimationAsset*> randClothesAnimations;
    CurrentProgress->GetRandomClothesMeshesAndAnimations(randMesh, randAnim, randClothesMeshes, randClothesAnimations);
    
    /*
     1. clothAnimationOffset 으로 어떻게 파편화된 animation frame을 맞추는가?
        이거 그냥 랜덤하게 Meshes를 고르면 안됨. Human Position에 따라서 골라야 할듯
        - for 문으로 sort되어있는지 확인해보자
            - 잘 되어있음
        - Human Position, Postion Max / Animation Position Max 를 찍어보자
     2. GetGarmentByID의 LoadedGarment 의존성 때문에 현재 에러 생기는중 (해결)
     */
    float clothAnimationOffset = 0.0f;
    float pos = SkeletalMesh->GetPosition();
    
    /* Debug */
//    UE_LOG(SynthesisEngine, Error, TEXT("Human Animation Current : %f"), pos);
    int j = 0;
    float clothesPos = pos;
    for ( ; j < randClothesMeshes.Num() && (clothesPos - randClothesAnimations[j]->GetMaxCurrentTime()) > 0 ; j++)
    {
        clothesPos -= randClothesAnimations[j]->GetMaxCurrentTime();
    }
    
    Garments[0]->SetWorldLocation(location);
    Garments[0]->SetWorldRotation(randRot);
    
    Garments[0]->SetSkeletalMesh(randClothesMeshes[j]);
    Garments[0]->PlayAnimation(randClothesAnimations[j], false);
    Garments[0]->GetSingleNodeInstance()->SetPosition(clothesPos);
    Garments[0]->Stop();
    
//    for (int i = 0; i < randClothesMeshes.Num(); i++)
//    {
//
//    }
}
