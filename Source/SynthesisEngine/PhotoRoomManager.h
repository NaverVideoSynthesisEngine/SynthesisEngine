// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "PhotoRoom.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "PhotoRoomManager.generated.h"

UCLASS()
class SYNTHESISENGINE_API APhotoRoomManager : public APawn
{
	GENERATED_BODY()
	
private:
	class USynthesisEngineInstance* EngineInstance;
	TArray<APhotoRoom*> PhotoRoomInstances;
    
	int PhotoRoomIndex;
public:	
	// Sets default values for this actor's properties
	APhotoRoomManager();
    
    UPROPERTY(EditAnywhere)
    float AutoUpdateInterval = 0.3f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
	void Update();
    
    void ToggleAutoUpdate();
    
    void Skip3DModel();
    void Skip3DModelAnimation();
    void SkipClothes();
    
    void LoadNextLevel();
};
