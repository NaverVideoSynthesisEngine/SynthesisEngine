// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimationPerturberComponent.h"
#include "PhotoRoom.h"

// Sets default values for this component's properties
UAnimationPerturberComponent::UAnimationPerturberComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


void UAnimationPerturberComponent::Update()
{
	USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
	FProgress* CurrentProgress = EngineInstance->Progress;

	if(Check3DModel(CurrentProgress))
    {
        Check3DModelAnimation(CurrentProgress);
        CheckClothes_MC(CurrentProgress);
        NextFrame_MC();
    }
}

bool UAnimationPerturberComponent::Check3DModel(FProgress* CurrentProgress)
{
	bool UpdateModel = SkeletalMesh->SkeletalMesh == NULL ||
		(SkeletalMesh->SkeletalMesh != NULL && !CurrentProgress->IsNextAnimationAvailable() && IsCurrentAnimationReachedEnd() && !CurrentProgress->IsNextClothesAvailable());
	if (UpdateModel)
	{
		if (!CurrentProgress->MoveToNextModel())
		{
			UE_LOG(SynthesisEngine, Error, TEXT("Process completed or Some error occured. || No Available Next 3D Model"));
			ProcessDoneEvent.Broadcast();
			return false;
		}
		CurrentSkeletalMesh = CurrentProgress->GetCurrentSkeletalMesh();
		SkeletalMesh->SetSkeletalMesh(CurrentSkeletalMesh);

		HumanChangedEvent.Broadcast();
		ResetAnimationFields();
		ResetClothesField_MC();
	}
    return true;
}
void UAnimationPerturberComponent::Check3DModelAnimation(FProgress* CurrentProgress)
{
	bool UpdateAnimation = CurrentAnimationAsset == NULL ||
		(CurrentAnimationAsset != NULL && !CurrentProgress->IsNextClothesAvailable() && IsCurrentAnimationReachedEnd());
	if (UpdateAnimation)
	{
		if (!CurrentProgress->MoveToNextAnimation())
		{
			UE_LOG(SynthesisEngine, Error, TEXT("Some error occured. || Load Animation failed"));
			return;
		}
		CurrentAnimationAsset = CurrentProgress->GetCurrentAnimation();
		SkeletalMesh->PlayAnimation(CurrentAnimationAsset, false);
		SkeletalMesh->Stop();

		HumanAnimationChangedEvent.Broadcast();
		ResetClothesField_MC();
	}
}
void UAnimationPerturberComponent::CheckClothes(FProgress* CurrentProgress)
{
	bool UpdateClothes = GarmentMesh->SkeletalMesh == NULL ||
		GarmentMesh->SkeletalMesh != NULL && IsCurrentClothesAnimationReachedEnd();
	if (UpdateClothes)
	{
		if (!CurrentProgress->MoveToNextClothes())
		{
			if (CurrentProgress->IsGarmentsLoadedProperly())
			{
				UE_LOG(SynthesisEngine, Error, TEXT("Some error occured. || Load Clothes failed"));
			}
			return;
		}
		if (CurrentClothesMesh != NULL)
		{
			ClothAnimationOffset += CurrentClothesAnimationAsset->GetMaxCurrentTime();
		}
		CurrentClothesMesh = CurrentProgress->GetCurrentClothesMesh();
		if (CurrentClothesMesh != NULL)
		{
			GarmentMesh->SetSkeletalMesh(CurrentClothesMesh);
			CurrentClothesAnimationAsset = CurrentProgress->GetCurrentClothesAnimation();
			GarmentMesh->PlayAnimation(CurrentClothesAnimationAsset, false);
			GarmentMesh->Stop();
			ClothChangedEvent.Broadcast();
		}
	}
}

void UAnimationPerturberComponent::CheckClothes_MC(FProgress* CurrentProgress)
{
	bool UpdateClothes = GarmentMeshes[0]->SkeletalMesh == NULL ||
		GarmentMeshes[0]->SkeletalMesh != NULL && IsCurrentClothesAnimationReachedEnd_MC();
	if (UpdateClothes)
	{
		bool isCombinationShouldBeChanged = CurrentProgress->WillGarmentCombinationBeChanged();
		if (!isCombinationShouldBeChanged || Owner->CheckIteration()) //using C++ short-circuit  http://ohyecloudy.com/pnotes/archives/542/
		{
			if (isCombinationShouldBeChanged)
			{
				GarmentCombinationIterationEvent.Broadcast();
				GarmentCombinationChangedEvent.Broadcast();
			}
			if (!CurrentProgress->MoveToNextClothes())
			{
				if (CurrentProgress->IsGarmentsLoadedProperly())
				{
					UE_LOG(SynthesisEngine, Error, TEXT("Some error occured. || Load Clothes failed"));
				}
				return;
			}
		}
		if (isCombinationShouldBeChanged) 
		{
			CurrentProgress->ResetGarmentIndex();
			ResetClothesField_MC();
			ResetHumanAnimation();

			GarmentCombinationIterationEvent.Broadcast();
		}
		
		if (CurrentClothesMeshes.Num() > 0)
		{
			ClothAnimationOffset += CurrentClothesAnimationAssets[0]->GetMaxCurrentTime();
		}
		CurrentClothesMeshes = CurrentProgress->GetCurrentClothesMeshes();
		if (CurrentClothesMeshes.Num() > 0)
		{
			bool wasClothesInit = GarmentMeshes[0]->SkeletalMesh == NULL;
			CurrentClothesAnimationAssets = CurrentProgress->GetCurrentClothesAnimations();
			for (int i = 0; i < CurrentClothesMeshes.Num(); i++)
			{
				if (isCombinationShouldBeChanged)
					GarmentMeshes[i]->SetPosition(0.0f); //Rewind Animation cursor before return garment mesh
                GarmentMeshes[i]->SetWorldLocation(GetOwner()->GetActorLocation());
                GarmentMeshes[i]->SetWorldRotation(GetOwner()->GetActorRotation());
				GarmentMeshes[i]->SetSkeletalMesh(CurrentClothesMeshes[i]);
				GarmentMeshes[i]->PlayAnimation(CurrentClothesAnimationAssets[i], false);
				GarmentMeshes[i]->Stop();
                if(isCombinationShouldBeChanged)
                    GarmentMeshes[i]->SetPosition(0.0f);
			}
			ClothChangedEvent.Broadcast();
		}
	}
}

void UAnimationPerturberComponent::ResetAnimationFields()
{
	CurrentAnimationAsset = NULL;
}

void UAnimationPerturberComponent::ResetClothesField()
{
	ClothAnimationOffset = 0.0;
	GarmentMesh->SetSkeletalMesh(NULL);
	CurrentClothesMesh = NULL;
	CurrentClothesAnimationAsset = NULL;
}

void UAnimationPerturberComponent::ResetClothesField_MC()
{
	ClothAnimationOffset = 0.0;
	for (const auto comp : GarmentMeshes)
	{
		comp->SetSkeletalMesh(NULL);
	}
	CurrentClothesMeshes.Empty();
	CurrentClothesAnimationAssets.Empty();
}

void UAnimationPerturberComponent::ResetHumanAnimation()
{
	if (CurrentAnimationAsset == NULL)
	{
		UE_LOG(SynthesisEngine, Error, TEXT("There is no Human Animation to reset."));
		return;
	} 
	SkeletalMesh->SetPosition(0.0f);
}

bool UAnimationPerturberComponent::IsCurrentAnimationReachedEnd() const
{
	if (CurrentAnimationAsset == NULL || SkeletalMesh->SkeletalMesh == NULL)
	{
		UE_LOG(SynthesisEngine, Error, TEXT("AnimationAsset or SkeletalMesh is null. Can't check animation frame"));
		return false;
	}

	return (CurrentAnimationAsset->GetMaxCurrentTime() - SkeletalMesh->GetPosition() < 0.000001);
}

bool UAnimationPerturberComponent::IsCurrentClothesAnimationReachedEnd() const
{
	if (CurrentClothesAnimationAsset == NULL || GarmentMesh->SkeletalMesh == NULL)
	{
		UE_LOG(SynthesisEngine, Error, TEXT("ClothesAnimationAsset or GarmentMesh is null. Can't check animation frame"));
		return false;
	}

	//UE_LOG(SynthesisEngine, Warning, TEXT("Max Time : %f"), CurrentClothesAnimationAsset->GetMaxCurrentTime()); //Debug
	//UE_LOG(SynthesisEngine, Warning, TEXT("Current Time : %f"), GarmentMesh->GetPosition()); //Debug

	return (CurrentClothesAnimationAsset->GetMaxCurrentTime() - GarmentMesh->GetPosition() < 0.000001);
}

bool UAnimationPerturberComponent::IsCurrentClothesAnimationReachedEnd_MC() const
{
	if (CurrentClothesAnimationAssets.Num() < 1)
	{
		UE_LOG(SynthesisEngine, Error, TEXT("ClothesAnimationAsset is null. Can't check animation frame"));
		return false;
	}
	if (GarmentMeshes[0]->SkeletalMesh == NULL)
	{
		UE_LOG(SynthesisEngine, Error, TEXT("At least one Garment is not null. Can't check animation frame"));
		return false;
	}

	UAnimationAsset* EvaluationTargetCloth = CurrentClothesAnimationAssets[0];
	if (IsCurrentAnimationReachedEnd())
	{
		float SingleFrameThreshold = 0.034;
		return (EvaluationTargetCloth->GetMaxCurrentTime() - GarmentMeshes[0]->GetPosition() < SingleFrameThreshold);
	}
	else
		return (EvaluationTargetCloth->GetMaxCurrentTime() - GarmentMeshes[0]->GetPosition() < 0.000001);
}

bool UAnimationPerturberComponent::NextFrame()
{
	if (!CurrentAnimationAsset->IsA(UAnimSequence::StaticClass()))
	{
		UE_LOG(SynthesisEngine, Error, TEXT("All Available Animations should be Anim Sequence. Please check Animations"));
		return false;
	}
	SkeletalMesh->GetSingleNodeInstance()->StepForward();
	if (GarmentMesh->SkeletalMesh != NULL)
	{
		float pos = SkeletalMesh->GetPosition();
		GarmentMesh->GetSingleNodeInstance()->SetPosition(pos - ClothAnimationOffset);
	}
	return true;
}

bool UAnimationPerturberComponent::NextFrame_MC()
{
	if (CurrentClothesAnimationAssets.Num() != CurrentClothesMeshes.Num())
	{
		UE_LOG(SynthesisEngine, Error, TEXT("Mismatch of AnimationAsset Num (%d) and ClothesMesh Num (%d)"), CurrentClothesAnimationAssets.Num(), CurrentClothesMeshes.Num());
		return false;
	}
	SkeletalMesh->GetSingleNodeInstance()->StepForward();
	for (int i = 0; i < CurrentClothesMeshes.Num(); i++)
	{
		if (!CurrentClothesAnimationAssets[i]->IsA(UAnimSequence::StaticClass()))
		{
			UE_LOG(SynthesisEngine, Error, TEXT("All Available Animations should be Anim Sequence. Please check Animations"));
			return false;
		}
		if (GarmentMeshes[i]->SkeletalMesh != NULL)
		{
			float pos = SkeletalMesh->GetPosition();
			GarmentMeshes[i]->GetSingleNodeInstance()->SetPosition(pos - ClothAnimationOffset);
		}
	}
	return true;
}

bool UAnimationPerturberComponent::BackFrame()
{
	/*
	Garment Update is not yet implemented (0812)
	*/
	if (!CurrentAnimationAsset->IsA(UAnimSequence::StaticClass()))
	{
		UE_LOG(SynthesisEngine, Error, TEXT("All Available Animations should be Anim Sequence. Please check Animations"));
		return false;
	}
	SkeletalMesh->GetSingleNodeInstance()->StepBackward();
	if (GarmentMesh->SkeletalMesh != NULL)
	{
		GarmentMesh->GetSingleNodeInstance()->StepBackward();
	}
	return true;
}

bool UAnimationPerturberComponent::NextFrameWithFixedFrameRate()
{
	/*
	Garment Update is not yet implemented (0809). If you use this function, garments will not be updated properly.
	*/
	/// Sometimes frame rate of some animations changes after it is imported.
	/// for example, frame rate goes 570 which was 30 before import. 
	/// this function makes all animation plays on their original frame rate. (update multiple times at once)
	if (!CurrentAnimationAsset->IsA(UAnimSequence::StaticClass()))
	{
		UE_LOG(SynthesisEngine, Error, TEXT("All Available Animations should be Anim Sequence. Please check Animations"));
		return false;
	}
	UAnimSequence* sequenceAnimation = Cast<UAnimSequence>(CurrentAnimationAsset);
	int updateCount = (int)(sequenceAnimation->GetFrameRate() / sequenceAnimation->ImportFileFramerate);
	for (int i = 0; i < updateCount; i++)
	{
		SkeletalMesh->GetSingleNodeInstance()->StepForward();
	}
	return true;
}

void UAnimationPerturberComponent::Skip3DModel()
{
	USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
	FProgress* CurrentProgress = EngineInstance->Progress;

	if (!CurrentProgress->MoveToNextModel())
	{
		UE_LOG(SynthesisEngine, Error, TEXT("Process completed or Some error occured. || No Available Next 3D Model"));
		return;
	}
	CurrentSkeletalMesh = CurrentProgress->GetCurrentSkeletalMesh();
	SkeletalMesh->SetSkeletalMesh(CurrentSkeletalMesh);

	HumanChangedEvent.Broadcast();
	ResetAnimationFields();
	ResetClothesField_MC();
}

void UAnimationPerturberComponent::Skip3DModelAnimation()
{
	USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
	FProgress* CurrentProgress = EngineInstance->Progress;

	if (!CurrentProgress->MoveToNextAnimation())
	{
		UE_LOG(SynthesisEngine, Error, TEXT("Some error occured. || Load Animation failed"));
		return;
	}
	CurrentAnimationAsset = CurrentProgress->GetCurrentAnimation();
	SkeletalMesh->PlayAnimation(CurrentAnimationAsset, false);
	SkeletalMesh->Stop();

	HumanAnimationChangedEvent.Broadcast();
	ResetClothesField_MC();
}

void UAnimationPerturberComponent::SkipClothes()
{
	USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
	FProgress* CurrentProgress = EngineInstance->Progress;

	if (!CurrentProgress->MoveToNextClothes())
	{
		if (CurrentProgress->IsGarmentsLoadedProperly())
		{
			UE_LOG(SynthesisEngine, Error, TEXT("Some error occured. || Load Clothes failed"));
		}
		return;
	}
	if (CurrentClothesMeshes.Num() > 0)
	{
		ClothAnimationOffset += CurrentClothesAnimationAssets[0]->GetMaxCurrentTime();
	}
	CurrentClothesMeshes = CurrentProgress->GetCurrentClothesMeshes();
	if (CurrentClothesMeshes.Num() > 0)
	{
		CurrentClothesAnimationAssets = CurrentProgress->GetCurrentClothesAnimations();
		for (int i = 0; i < CurrentClothesMeshes.Num(); i++)
		{
			GarmentMeshes[i]->SetSkeletalMesh(CurrentClothesMeshes[i]);
			GarmentMeshes[i]->PlayAnimation(CurrentClothesAnimationAssets[i], false);
			GarmentMeshes[i]->Stop();
			ClothChangedEvent.Broadcast();
		}
	}
}


void UAnimationPerturberComponent::OnChangedEnableProperty(bool IsEnable)
{
}

void UAnimationPerturberComponent::Init(USkeletalMeshComponent* SkeletalMesh, USkeletalMeshComponent* Garment)
{
	this->GarmentMesh = Garment;
	this->SkeletalMesh = SkeletalMesh;
}

void UAnimationPerturberComponent::Init(class APhotoRoom* Owner, USkeletalMeshComponent* SkeletalMesh, TArray<USkeletalMeshComponent*> Garments)
{
	this->Owner = Owner;
	this->SkeletalMesh = SkeletalMesh;
	this->GarmentMeshes = Garments; //Is this right? Should think about Deep/Shallow Copy more.
}
