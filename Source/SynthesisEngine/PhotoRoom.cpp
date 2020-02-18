// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoRoom.h"
#include "MultiPersonPerturberComponent.h"
#include "AnimationPerturberComponent.h"

const int APhotoRoom::GarmentSocketNumber = 5;
const int APhotoRoom::MultiPersonSocketNumber = 3;

// Sets default values
APhotoRoom::APhotoRoom()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainCharacter"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraPerturber = CreateDefaultSubobject<UCameraPerturberComponent>(TEXT("Camera Perturber"));
	MaterialPerturber = CreateDefaultSubobject<UMaterialPerturberComponent>(TEXT("Material Perturber"));
	AnimationPerturber = CreateDefaultSubobject<UAnimationPerturberComponent>(TEXT("Animation Perturber"));

	RootComponent = SkeletalMesh;
	CameraComponent->SetupAttachment(SkeletalMesh);
	CameraPerturber->SetupAttachment(SkeletalMesh);
	MaterialPerturber->SetupAttachment(SkeletalMesh);
	AnimationPerturber->SetupAttachment(SkeletalMesh);
    
	for (int i = 0; i < GarmentSocketNumber; i++)
	{
		FString ID = FString::Printf(TEXT("Garment%d"), i);
		USkeletalMeshComponent* TempGarment = CreateDefaultSubobject<USkeletalMeshComponent>(*ID);
		Garments.Add(TempGarment);
	}
    
    for (int i = 0; i < MultiPersonSocketNumber; i++)
    {
        /*
         Initialize fields of specific class at the outside of the class
         This seems really inconvinent and unnatural.
         I wanted to initializa them on inside of the class but I encountered mysterious run-time error.
         Everytime I assign a non-zero value to the transfrom(location), UE shuts down.
         I think this is because all of the components should be created before its constructor ends.
        */
        FString ID = FString::Printf(TEXT("MultiPerson%d"), i);
        UMultiPersonPerturberComponent* TempPerson = CreateDefaultSubobject<UMultiPersonPerturberComponent>(*ID);
        TempPerson->SetupAttachment(SkeletalMesh);
        FString CharacterName = FString::Printf(TEXT("Character_%d"), i);
        
        TempPerson->SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(*CharacterName);
        TempPerson->SkeletalMesh->SetupAttachment(TempPerson);
        
        for (int j = 0; j < GarmentSocketNumber; j++)
        {
            FString GarmentName = FString::Printf(TEXT("Garment_%d_%d"), i,j);
            USkeletalMeshComponent* TempGarment = CreateDefaultSubobject<USkeletalMeshComponent>(*GarmentName);
            TempPerson->Garments.Add(TempGarment);
        }
        MultiPeople.Add(TempPerson);
    }

	LateDataFlushingCount = 0;
	b_ShouldUpdate = true;
    UpdatePhase_COCO = ECocoUpdatePhase::UPDATE_PERTURBERS;
}

void APhotoRoom::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//AnimationPerturber->Init(this->SkeletalMesh, this->Garment); //For Single Clothes
	//MaterialPerturber->Init(this->SkeletalMesh, this->Garment); //For Single Clothes

	AnimationPerturber->Init(this, this->SkeletalMesh, this->Garments); //For Multiple Clothes
	MaterialPerturber->Init(this->SkeletalMesh, this->Garments); //For Multiple Clothes
	CameraPerturber->Init(this->SkeletalMesh, this->CameraComponent);
    for (int i = 0; i < MultiPersonSocketNumber; i++)
    {
        MultiPeople[i]->Init(this);
    }
	AnimationPerturber->OnClothChanged().AddLambda([=]()
	{
		if (MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_CLOTH_IS_CHANGED)
		{
			MaterialPerturber->Update();
		}
		if (CameraPerturberUpdateProtocol == EUpdateProtocol::UPDATE_CLOTH_IS_CHANGED)
		{
			CameraPerturber->Update();
		}
	});
	AnimationPerturber->OnGarmentCombinationChanged().AddLambda([=]()
	{
		if (MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_COMBINATION_IS_CHANGED)
		{
			MaterialPerturber->Update();
		}
		if (CameraPerturberUpdateProtocol == EUpdateProtocol::UPDATE_COMBINATION_IS_CHANGED)
		{
			CameraPerturber->Update();
		}
	});
	AnimationPerturber->OnGarmentCombinationIteration().AddLambda([=]()
	{
		if (MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_COMBINATION_ITERATION)
		{
			MaterialPerturber->Update();
		}
		if (CameraPerturberUpdateProtocol == EUpdateProtocol::UPDATE_COMBINATION_ITERATION)
		{
			CameraPerturber->Update();
		}
	});
	AnimationPerturber->OnProcessDone().AddLambda([=]()
	{
		this->ProcessDoneEvent.Broadcast();
	});
    
}

// Called when the game starts or when spawned
void APhotoRoom::BeginPlay()
{
	Super::BeginPlay();
    APostProcessVolume * volume = nullptr;

    for (TActorIterator<APostProcessVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        volume = Cast<APostProcessVolume>(*ActorItr);
        if (volume->GetActorLabel() == "PostProcessVolume") {
            break;
        }
    }
	DataFlushManager = new FDataFlushManager(Cast<AActor>(this), GetWorld(), this->CameraComponent, volume);
}

void APhotoRoom::BeginDestroy()
{
	Super::BeginDestroy();
	delete(DataFlushManager);
}

void APhotoRoom::PostEditChangeProperty(FPropertyChangedEvent& e)
{
	FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;

	/* Enable Property */
	if (PropertyName == GET_MEMBER_NAME_CHECKED(APhotoRoom, EnableCameraPerturber))
	{
		CameraPerturber->OnChangedEnableProperty(EnableCameraPerturber);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(APhotoRoom, EnableAnimationPerturber))
	{
		AnimationPerturber->OnChangedEnableProperty(EnableAnimationPerturber);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(APhotoRoom, EnableMaterialPerturber))
	{
		MaterialPerturber->OnChangedEnableProperty(EnableMaterialPerturber);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(APhotoRoom, EnableDataFlush))
	{
		DataFlushManager->OnChangedEnableProperty(EnableDataFlush);
	}
	Super::PostEditChangeProperty(e);
}

// Called every frame
void APhotoRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APhotoRoom::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
//	PlayerInputComponent->BindAction("LoadNextLevel", IE_Pressed, this, &APhotoRoom::DEBUG_LoadNextLevel);
}

void APhotoRoom::Update()
{
	if (EnableAnimationPerturber)
		AnimationPerturber->Update();
	if (EnableCameraPerturber && CameraPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
		CameraPerturber->Update();
	if (EnableMaterialPerturber && MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
		MaterialPerturber->Update();
	if (EnableDataFlush) 
		DataFlushManager->FlushToData(TEXT("D:/workspace/SynthesisEngine/ProtoOutputs"), *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
	
	if (b_FirstUpdate && PerturbCameraAndMaterialOnStart)
	{
		b_FirstUpdate = false;
		CameraPerturber->Update();
		MaterialPerturber->Update();
	}
}

void APhotoRoom::UpdateWithLateDataFlushing()
{
	if (b_ShouldUpdate)
	{
		if (EnableAnimationPerturber)
			AnimationPerturber->Update();
		if (EnableCameraPerturber && CameraPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
			CameraPerturber->Update();
		if (EnableMaterialPerturber && MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
			MaterialPerturber->Update();
		b_ShouldUpdate = false;

		if (b_FirstUpdate && PerturbCameraAndMaterialOnStart)
		{
			b_FirstUpdate = false;
			CameraPerturber->Update();
			MaterialPerturber->Update();
		}
	}
	else
	{
        FString path;
        FString platform = UGameplayStatics::GetPlatformName();
        if(platform == TEXT("Mac"))
        {
            path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
        }
        else
        {
            path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
        }
		if(LateDataFlushingCount == LATE_DATA_FLUSHING_Frame_to_Skip)
		{
			if (EnableDataFlush)
				DataFlushManager->FlushToDataCocoFormat(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
			LateDataFlushingCount = 0;
			b_ShouldUpdate = true;
		}
		else
		{
			LateDataFlushingCount++;
		}
	}
}

void APhotoRoom::UpdateWithLateDataFlushing_COCOTEMP()
{
    FString path;
    FString platform;
    switch(UpdatePhase_COCO)
    {
        case ECocoUpdatePhase::UPDATE_PERTURBERS:
            if (EnableAnimationPerturber)
                AnimationPerturber->Update();
            if (EnableCameraPerturber && CameraPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
                CameraPerturber->Update();
            if (EnableMaterialPerturber && MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
                MaterialPerturber->Update();
            UpdatePhase_COCO = ECocoUpdatePhase::FLUSH_IMAGE_AND_KEYPOINTS;

            if (b_FirstUpdate && PerturbCameraAndMaterialOnStart)
            {
                b_FirstUpdate = false;
                CameraPerturber->Update();
                MaterialPerturber->Update();
            }
            break;
            
        case ECocoUpdatePhase::FLUSH_IMAGE_AND_KEYPOINTS:
            platform = UGameplayStatics::GetPlatformName();
            if(platform == TEXT("Mac"))
            {
                path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
            }
            else
            {
                path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
            }
            
            if (EnableDataFlush)
                DataFlushManager->FlushToDataCocoFormat(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
            UpdatePhase_COCO = ECocoUpdatePhase::ENABLE_POSTPROCESSVOLUME;
            break;
            
        case ECocoUpdatePhase::ENABLE_POSTPROCESSVOLUME:
            DataFlushManager->EnablePostProcessVolume(true);
            UpdatePhase_COCO = ECocoUpdatePhase::FLUSH_MASK;
            break;
            
        case ECocoUpdatePhase::FLUSH_MASK:
            platform = UGameplayStatics::GetPlatformName();
            if(platform == TEXT("Mac"))
            {
                path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
            }
            else
            {
                path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
            }
            
            if (EnableDataFlush)
                DataFlushManager->FlushToDataCocoFormat_MASK(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
            UpdatePhase_COCO = ECocoUpdatePhase::DISABLE_POSTPROCESSVOLUME;
            break;
            
        case ECocoUpdatePhase::DISABLE_POSTPROCESSVOLUME:
            DataFlushManager->EnablePostProcessVolume(false);
            UpdatePhase_COCO = ECocoUpdatePhase::UPDATE_PERTURBERS;
            break;
    }
}

void APhotoRoom::UpdateWithLateDataFlushing_MPITEMP()
{
    if (b_ShouldUpdate)
    {
        if (EnableAnimationPerturber)
            AnimationPerturber->Update();
        if (EnableCameraPerturber && CameraPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
            CameraPerturber->Update();
        if (EnableMaterialPerturber && MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
            MaterialPerturber->Update();
        b_ShouldUpdate = false;

        if (b_FirstUpdate && PerturbCameraAndMaterialOnStart)
        {
            b_FirstUpdate = false;
            CameraPerturber->Update();
            MaterialPerturber->Update();
        }
    }
    else
    {
        FString path;
        FString platform = UGameplayStatics::GetPlatformName();
        if(platform == TEXT("Mac"))
        {
            path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
        }
        else
        {
            path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
        }
        if(LateDataFlushingCount == LATE_DATA_FLUSHING_Frame_to_Skip)
        {
            if (EnableDataFlush)
                DataFlushManager->FlushToDataMPIFormat(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
            LateDataFlushingCount = 0;
            b_ShouldUpdate = true;
        }
        else
        {
            LateDataFlushingCount++;
        }
    }
}

void APhotoRoom::UpdateWithLateDataFlushing_TOTAL()
{
    FString path;
    FString platform;
    switch (UpdatePhase_TOTAL)
    {
    case ETotalUpdatePhase::UPDATE_PERTURBERS:
        if (EnableAnimationPerturber)
            AnimationPerturber->Update();
        if (EnableCameraPerturber && CameraPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
            CameraPerturber->Update();
        if (EnableMaterialPerturber && MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
            MaterialPerturber->Update();
            
        if (EnableMultiPersonPerturber)
        {
            for(UMultiPersonPerturberComponent * multi : MultiPeople)
            {
                multi->Update();
            }
        }
            
        UpdatePhase_TOTAL = ETotalUpdatePhase::FLUSH_IMAGE_AND_KEYPOINTS;
            
        if (b_FirstUpdate && PerturbCameraAndMaterialOnStart)
        {
            b_FirstUpdate = false;
            CameraPerturber->Update();
            MaterialPerturber->Update();
        }
        break;

    case ETotalUpdatePhase::FLUSH_IMAGE_AND_KEYPOINTS:
        platform = UGameplayStatics::GetPlatformName();
        if (platform == TEXT("Mac"))
        {
            path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
        }
        else
        {
            path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
        }

        if (EnableDataFlush)
        {
            DataFlushManager->FlushToDataTotalFormat_ANNOT(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
            if(EnableMultiPersonPerturber)
            {
                for(int i = 0 ; i < MultiPersonSocketNumber; i++)
                {
                    
                    DataFlushManager->FlushToDataTotalFormat_ANNOT(path, *GetWorld()->GetName(), MultiPeople[i]->GetName(), MultiPeople[i]->SkeletalMesh, this->CameraComponent);
                }
                
            }
            DataFlushManager->FlushToDataTotalFormat_IMAGE(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
        }
        UpdatePhase_TOTAL = ETotalUpdatePhase::ENABLE_POSTPROCESSVOLUME;
        break;

    case ETotalUpdatePhase::ENABLE_POSTPROCESSVOLUME:
        DataFlushManager->EnablePostProcessVolume(true);
        UpdatePhase_TOTAL = ETotalUpdatePhase::FLUSH_MASK;
        break;

    case ETotalUpdatePhase::FLUSH_MASK:
        platform = UGameplayStatics::GetPlatformName();
        if (platform == TEXT("Mac"))
        {
            path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
        }
        else
        {
            path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
        }

        if (EnableDataFlush)
            DataFlushManager->FlushToDataTotalFormat_MASK(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
        UpdatePhase_TOTAL = ETotalUpdatePhase::DISABLE_POSTPROCESSVOLUME;
        break;

    case ETotalUpdatePhase::DISABLE_POSTPROCESSVOLUME:
        DataFlushManager->EnablePostProcessVolume(false);
        UpdatePhase_TOTAL = ETotalUpdatePhase::UPDATE_PERTURBERS;
        break;
    }

}

void APhotoRoom::UpdateWithLateDataFlushing_TOTAL_FIXEDCAMERA()
{
    /*
    It is okay not to remove all Update Protocol
    Just make sure that Update Protocol of camera perturber -> None
    */
    FString path;
    FString platform;
    switch (UpdatePhase_TOTAL)
    {
    case ETotalUpdatePhase::UPDATE_PERTURBERS:
        if (b_FirstUpdate)
        {
            b_FirstUpdate = false;
            AnimationPerturber->Update();
        }
        if (CameraPerturber->Mode != ECameraPerturberMode::FIXED_POSITIONS)
        {
            UE_LOG(SynthesisEngine, Warning, TEXT("Camera Perturber Mode is not FIXED_POSITION while you are using TOTAL_FIXEDCAMERA function. please re-check the camera perturber setting."));
        }
        if (EnableCameraPerturber)
        {
            CameraPerturber->Update();
            FixedCameraIndex++;
        }
        if (FixedCameraIndex >= CameraPoints.Num())
        {
            if (EnableAnimationPerturber)
            {
                AnimationPerturber->Update();
            }
            if (EnableMaterialPerturber && MaterialPerturberUpdateProtocol == EUpdateProtocol::UPDATE_EVERY_FRAME)
                MaterialPerturber->Update();

            FixedCameraIndex = 0;
        }
        UpdatePhase_TOTAL = ETotalUpdatePhase::FLUSH_IMAGE_AND_KEYPOINTS;
        break;

    case ETotalUpdatePhase::FLUSH_IMAGE_AND_KEYPOINTS:
        platform = UGameplayStatics::GetPlatformName();
        if (platform == TEXT("Mac"))
        {
            path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
        }
        else
        {
            path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
        }

        if (EnableDataFlush)
        {
            DataFlushManager->FlushToDataTotalFormat_ANNOT(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
            DataFlushManager->FlushToDataTotalFormat_IMAGE(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
        }
        UpdatePhase_TOTAL = ETotalUpdatePhase::ENABLE_POSTPROCESSVOLUME;
        break;

    case ETotalUpdatePhase::ENABLE_POSTPROCESSVOLUME:
        DataFlushManager->EnablePostProcessVolume(true);
        UpdatePhase_TOTAL = ETotalUpdatePhase::FLUSH_MASK;
        break;

    case ETotalUpdatePhase::FLUSH_MASK:
        platform = UGameplayStatics::GetPlatformName();
        if (platform == TEXT("Mac"))
        {
            path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
        }
        else
        {
            path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
        }

        if (EnableDataFlush)
            DataFlushManager->FlushToDataTotalFormat_MASK(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
        UpdatePhase_TOTAL = ETotalUpdatePhase::DISABLE_POSTPROCESSVOLUME;
        break;

    case ETotalUpdatePhase::DISABLE_POSTPROCESSVOLUME:
        DataFlushManager->EnablePostProcessVolume(false);
        UpdatePhase_TOTAL = ETotalUpdatePhase::UPDATE_PERTURBERS;
        break;
    }
}

bool APhotoRoom::CheckIteration()
{
	IterationIndex++;
	if (IterationIndex == IterationCount)
	{
		IterationIndex = 0;
		return true; //Should proceed to next animation.
	}
	return false; //Repeat this animation.
}

void APhotoRoom::Skip3DModel() { AnimationPerturber->Skip3DModel(); }

void APhotoRoom::Skip3DModelAnimation() { AnimationPerturber->Skip3DModelAnimation();  }

void APhotoRoom::SkipClothes() { AnimationPerturber->SkipClothes(); }
