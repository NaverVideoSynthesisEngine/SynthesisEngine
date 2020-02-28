// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Progress.h"
#include "NavigationSystem.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Runtime/Engine/Classes/Animation/AnimInstance.h"
#include "Runtime/Engine/Classes/Animation/AnimSequence.h"
#include "Runtime/Engine/Classes/Animation/AnimSingleNodeInstance.h"
#include "MultiPersonPerturberComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SYNTHESISENGINE_API UMultiPersonPerturberComponent : public USceneComponent
{
    GENERATED_BODY()
private:
    class APhotoRoom* Owner;
    FVector GetRandomLocationAroundMainCharacter();
    
    UNavigationSystemV1 * NavSystem;
    
    void SetGarment(int index, FVector location, FRotator rotation, USkeletalMesh* mesh, UAnimationAsset* anim, float pos);
    FString GetNextGarmentID(TArray<USkeletalMesh*> meshes, int index);
    void CalculateIndexOfValidMeshUsingPosition(TArray<USkeletalMesh*> meshes, TArray<UAnimationAsset*> anims, float pos, FString GarmentID, int& index, float& clothesPos);
    
public:
    UPROPERTY(EditAnywhere)
    float MaxRadius = 500.0f;
    
    UPROPERTY(EditAnywhere)
    float MinRadius = 100.0f;
    
    UPROPERTY(EditAnywhere)
    float ZAxisSnap = 12.0f;
    
    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* SkeletalMesh;
    
    UPROPERTY(VisibleAnywhere)
    TArray<USkeletalMeshComponent*> Garments;
    
	// Sets default values for this component's properties
	UMultiPersonPerturberComponent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    void Init(class APhotoRoom* Owner);
	void Update();
};
