// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Templates/SharedPointer.h"
#include "InstancedFoliageActor.h"
#include "FoliageConverter.generated.h"

UCLASS()
class SYNTHESISENGINE_API AFoliageConverter : public AActor
{
	GENERATED_BODY()
private:
    bool IsNearByConverter(TArray<FVector> Positions, FVector foliagePosition);
public:
    UPROPERTY(VisibleAnywhere)
    USceneComponent* SceneComponent;
    
    UPROPERTY(EditAnywhere, Category = Option)
    float Radius = 100.0f;
    
    UPROPERTY(EditAnywhere, Category = Option)
    int CustomStencilValueToApply = 4;
	// Sets default values for this actor's properties
	AFoliageConverter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
