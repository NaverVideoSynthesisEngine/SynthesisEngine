// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoRoom.h"
#include "AnimationPerturberComponent.h"

const int APhotoRoom::GarmentSocketNumber = 5;

// Sets default values
APhotoRoom::APhotoRoom()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MainCharacter"));
	Garment = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Garment"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraPerturber = CreateDefaultSubobject<UCameraPerturberComponent>(TEXT("Camera Perturber"));
	MaterialPerturber = CreateDefaultSubobject<UMaterialPerturberComponent>(TEXT("Material Perturber"));
	AnimationPerturber = CreateDefaultSubobject<UAnimationPerturberComponent>(TEXT("Animation Perturber"));

	RootComponent = SkeletalMesh;
	Garment->SetupAttachment(SkeletalMesh);
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
    APostProcessVolume * volume1 = nullptr;
    APostProcessVolume * volume2 = nullptr;
    AInstancedFoliageActor* foliageActor = nullptr;

    for (TActorIterator<APostProcessVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        volume1 = Cast<APostProcessVolume>(*ActorItr);
        if (volume1->GetActorLabel() == "PostProcessVolume1") {
            break;
        }
    }
    for (TActorIterator<APostProcessVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        volume2 = Cast<APostProcessVolume>(*ActorItr);
        if (volume2->GetActorLabel() == "PostProcessVolume2") {
            break;
        }
        else {
            volume2 = nullptr;
        }
    }
    for (TActorIterator<AInstancedFoliageActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
    {
        foliageActor = Cast<AInstancedFoliageActor>(*ActorItr);
        if (foliageActor != nullptr) {
            break;
        }
    }
	DataFlushManager = new FDataFlushManager(Cast<AActor>(this), GetWorld(), this->CameraComponent, volume1, volume2, foliageActor);
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
            DataFlushManager->EnablePostProcessVolume1(true);
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
            DataFlushManager->EnablePostProcessVolume1(false);
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
            DataFlushManager->FlushToDataTotalFormat(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
        UpdatePhase_TOTAL = ETotalUpdatePhase::ENABLE_POSTPROCESSVOLUME_STENCIL;
        break;

    case ETotalUpdatePhase::ENABLE_POSTPROCESSVOLUME_STENCIL:
        DataFlushManager->EnablePostProcessVolume1(true);
        DataFlushManager->EnablePostProcessVolume2(false);
        UpdatePhase_TOTAL = ETotalUpdatePhase::FLUSH_MASK_STENCIL;
        break;

    case ETotalUpdatePhase::FLUSH_MASK_STENCIL:
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
        UpdatePhase_TOTAL = ETotalUpdatePhase::DISABLE_POSTPROCESSVOLUME_STENCIL;
        break;

    case ETotalUpdatePhase::DISABLE_POSTPROCESSVOLUME_STENCIL:
        DataFlushManager->EnablePostProcessVolume1(false);
        UpdatePhase_TOTAL = ETotalUpdatePhase::ENABLE_POSTPROCESSVOLUME_OCCLUSION;
        break;

    case ETotalUpdatePhase::ENABLE_POSTPROCESSVOLUME_OCCLUSION:
        DataFlushManager->EnablePostProcessVolume2(true);
        UpdatePhase_TOTAL = ETotalUpdatePhase::CHANGE_FOLIAGE_SCALE1;
        break;

    case ETotalUpdatePhase::CHANGE_FOLIAGE_SCALE1:
        DataFlushManager->ChangeFoliageScale(FVector(0.f, 0.f, 0.f));
        UpdatePhase_TOTAL = ETotalUpdatePhase::FLUSH_MASK_OCCLUSION1;
        break;

    case ETotalUpdatePhase::FLUSH_MASK_OCCLUSION1:
        platform = UGameplayStatics::GetPlatformName();
        if (platform == TEXT("Mac"))
        {
            path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
        }
        else
        {
            path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
        }

        if (EnableDataFlush) {
            DataFlushManager->ChangeFoliageScale(FVector(0.f, 0.f, 0.f));
            DataFlushManager->FlushToDataTotalFormat_OCCLUSION1(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
        }
        UpdatePhase_TOTAL = ETotalUpdatePhase::CHANGE_FOLIAGE_SCALE2;
        break;

    case ETotalUpdatePhase::CHANGE_FOLIAGE_SCALE2:
        DataFlushManager->ChangeFoliageScale(FVector(1.f, 1.f, 1.f));
        UpdatePhase_TOTAL = ETotalUpdatePhase::FLUSH_MASK_OCCLUSION2;
        break;

    case ETotalUpdatePhase::FLUSH_MASK_OCCLUSION2:
        platform = UGameplayStatics::GetPlatformName();
        if (platform == TEXT("Mac"))
        {
            path = TEXT("/Users/chan/Desktop/Naver/ProtoOutputs");
        }
        else
        {
            path = TEXT("D:/workspace/SynthesisEngine/ProtoOutputs");
        }

        if (EnableDataFlush) {
            DataFlushManager->FlushToDataTotalFormat_OCCLUSION2(path, *GetWorld()->GetName(), GetActorLabel(), SkeletalMesh, this->CameraComponent);
        }
        UpdatePhase_TOTAL = ETotalUpdatePhase::DISABLE_POSTPROCESSVOLUME_OCCLUSION;
        break;

    case ETotalUpdatePhase::DISABLE_POSTPROCESSVOLUME_OCCLUSION:
        DataFlushManager->EnablePostProcessVolume1(false);
        DataFlushManager->EnablePostProcessVolume2(false);
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
