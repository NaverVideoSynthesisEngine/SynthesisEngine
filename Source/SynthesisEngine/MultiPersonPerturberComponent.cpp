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
     2. GetGarmentByID의 LoadedGarment 의존성 때문에 현재 에러 생기는중 
     */
    float clothAnimationOffset = 0.0f;
    float pos = SkeletalMesh->GetPosition();
    
    for (int i = 0; i < randClothesMeshes.Num(); i++)
    {
        UE_LOG(SynthesisEngine, Error, TEXT("Rand Gar"));
        Garments[i]->SetWorldLocation(location);
        Garments[i]->SetWorldRotation(randRot);
        Garments[i]->SetSkeletalMesh(randClothesMeshes[i]);
        Garments[i]->PlayAnimation(randClothesAnimations[i], false);
        Garments[i]->GetSingleNodeInstance()->SetPosition(0.0f);
        Garments[i]->Stop();
    }
}
