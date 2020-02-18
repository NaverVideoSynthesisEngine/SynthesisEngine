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
    
    SkeletalMesh->SetSkeletalMesh(CurrentProgress->GetRandomSkeletalMesh());
    SkeletalMesh->PlayAnimation(randAnim, false);
    SkeletalMesh->SetPosition(FMath::RandRange(0.0f, maxTime));
    SkeletalMesh->Stop();
    SkeletalMesh->SetWorldLocation(location);
    SkeletalMesh->SetWorldRotation(randRot);
}
