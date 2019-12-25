// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPerturberComponent.h"


const float UCameraPerturberComponent::MINIMUM_RADIUS = 100.0;


// Sets default values for this component's properties
UCameraPerturberComponent::UCameraPerturberComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//bTickInEditor = true;
}

// Called when the game starts
void UCameraPerturberComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (Mode == ECameraPerturberMode::FIXED_POSITIONS)
	{
		GetChildrenComponents(true, CameraSockets);
		if (CameraSockets.Num() == 0)
			UE_LOG(SynthesisEngine, Error, TEXT("There is no Camera Sockets"));
		index = 0;
	}
	else if (Mode == ECameraPerturberMode::RANDOM_DOME )
	{
		ValidateRandomDomeProperties();
		if (b_DrawDebugSphere)
		{
			DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), SmallRadius, 32, FColor::Red, false, 3.0f);
			DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), BigRadius, 32, FColor::Blue, false, 3.0f);
		}
	}
}

void UCameraPerturberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (b_EnableSmoothTrack)
	{
		APlayerCameraManager* CamManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;

		FVector CamLocation = CameraComponent->GetComponentLocation();
		FVector CamForward = CameraComponent->GetForwardVector();
		FVector SkeletonLocation = SkeletalMeshComponent->GetComponentLocation();
		FVector CamToSkeleton = SkeletonLocation - CamLocation;

		FVector dir; float length;
		CamToSkeleton.ToDirectionAndLength(dir, length);

		float cos = FVector::DotProduct(CamForward, dir);
		if (cos < FMath::Cos(FMath::DegreesToRadians(SmoothTrackAngleThreshold)) && !b_isTrackingStarted)
		{
			b_isTrackingStarted = true;
		}

		if (b_isTrackingStarted)
		{
			FRotator CamRotation = CameraComponent->GetComponentRotation();
			FQuat CamToSkeletonQuat = UKismetMathLibrary::Conv_VectorToQuaterion(dir);

			FQuat slerp = FQuat::Slerp(CamRotation.Quaternion(), CamToSkeletonQuat, DeltaTime * SmoothTrackSpeed);
			CameraComponent->SetWorldRotation(FRotator(slerp));
			if (cos > 1.0 - FLT_EPSILON)
			{
				b_isTrackingStarted = false;
			}
		}
	}
}


void UCameraPerturberComponent::ValidateRandomDomeProperties()
{
	if (SmallRadius > BigRadius)
	{
		float temp = SmallRadius;
		SmallRadius = BigRadius;
		BigRadius = temp;
	}

	if (LowerHeight > UpperHeight)
	{
		float temp = LowerHeight;
		LowerHeight = UpperHeight;
		UpperHeight = temp;
	}

	if (SmallRadius < MINIMUM_RADIUS)
	{
		SmallRadius = MINIMUM_RADIUS;
	}

	if (UpperHeight > BigRadius)
	{
		UpperHeight = BigRadius;
	}

	if (LowerHeight > BigRadius)
	{
		LowerHeight = BigRadius;
	}
}

void UCameraPerturberComponent::UpdateWithRandomPositions_Dome()
{
	float Radius = FMath::FRandRange(SmallRadius, BigRadius);
	float ThetaLimit01 = FMath::Acos(UpperHeight / Radius);
	float ThetaLimit02 = FMath::Acos(LowerHeight / Radius);
	float Theta = FMath::FRandRange(ThetaLimit01 < ThetaLimit02 ? ThetaLimit01 : ThetaLimit02, ThetaLimit01 < ThetaLimit02 ? ThetaLimit02 : ThetaLimit01);
	float Pie = FMath::FRandRange(0, 2 * PI);
	
	float sin = FMath::Sin(Theta);
	float x = Radius * sin * FMath::Cos(Pie);
	float y = Radius * sin * FMath::Sin(Pie);
	float z = Radius * FMath::Cos(Theta);

	FVector worldPos(x, y, z);
	worldPos += GetOwner()->GetActorLocation();
	//UE_LOG(SynthesisEngine, Warning, TEXT("Pos : %s"), *worldPos.ToString());
	
	FVector targetPos; FVector boxExtents; float radius;
	UKismetSystemLibrary::GetComponentBounds(SkeletalMeshComponent, targetPos, boxExtents, radius);
	FRotator rotation = UKismetMathLibrary::FindLookAtRotation(worldPos, targetPos);
	CameraComponent->SetWorldLocationAndRotation(worldPos, rotation);
}

void UCameraPerturberComponent::UpdateWithFixedPositions()
{
	USceneComponent* TargetSocket = CameraSockets[index % CameraSockets.Num()];
	FVector location; FRotator rotation;
	location = TargetSocket->GetComponentLocation();
	rotation = TargetSocket->GetComponentRotation();
	CameraComponent->SetWorldLocationAndRotation(location, rotation);

	index++;
}

void UCameraPerturberComponent::Update()
{
	if (Mode == ECameraPerturberMode::FIXED_POSITIONS)
	{
		UpdateWithFixedPositions();
	}
	else if (Mode == ECameraPerturberMode::RANDOM_DOME)
	{
		UpdateWithRandomPositions_Dome();
	}
}

void UCameraPerturberComponent::OnChangedEnableProperty(bool IsEnable)
{
}

void UCameraPerturberComponent::Init(USkeletalMeshComponent * SkeletalMeshComponent, UCameraComponent * CameraComponent)
{
	this->SkeletalMeshComponent = SkeletalMeshComponent;
	this->CameraComponent = CameraComponent;
}