// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SynthesisEngine.h"
#include "Components/ActorComponent.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "CameraPerturberComponent.generated.h"

UENUM(BlueprintType)
enum class ECameraPerturberMode : uint8
{
	FIXED_POSITIONS UMETA(DisplayName = "Fixed_Positions"),
	RANDOM_DOME UMETA(DisaplyName = "Random_Dome")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SYNTHESISENGINE_API UCameraPerturberComponent : public USceneComponent
{
	GENERATED_BODY()
private:
	bool b_isTrackingStarted;
	int index;
	TArray<USceneComponent*> CameraSockets;

	void ValidateRandomDomeProperties();
	void UpdateWithFixedPositions();
	void UpdateWithRandomPositions_Dome();

public:	
	static const float MINIMUM_RADIUS;

	// Sets default values for this component's properties
	UCameraPerturberComponent();
	UPROPERTY()
	UCameraComponent* CameraComponent;
	
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(EditAnywhere, Category = SmoothTrack)
	bool b_EnableSmoothTrack;

	UPROPERTY(EditAnywhere, Category = SmoothTrack)
	float SmoothTrackAngleThreshold = 40.0f;

	UPROPERTY(EditAnywhere, Category = SmoothTrack)
	float SmoothTrackSpeed = 0.5f;

	UPROPERTY(EditAnywhere)
	ECameraPerturberMode Mode;

	UPROPERTY(EditAnywhere, Category = RandomDome)
	bool b_DrawDebugSphere;

	UPROPERTY(EditAnywhere, Category = RandomDome)
	float SmallRadius;
	
	UPROPERTY(EditAnywhere, Category = RandomDome)
	float BigRadius;

	UPROPERTY(EditAnywhere, Category = RandomDome)
	float UpperHeight;

	UPROPERTY(EditAnywhere, Category = RandomDome)
	float LowerHeight;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
public:	
	void OnChangedEnableProperty(bool IsEnable);

	void Init(USkeletalMeshComponent * SkeletalMeshComponent, UCameraComponent * CameraComponent);
	void Update();
		
};
