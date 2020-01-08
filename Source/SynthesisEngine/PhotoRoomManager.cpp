// Fill out your copyright notice in the Description page of Project Settings.


#include "PhotoRoomManager.h"

// Sets default values
APhotoRoomManager::APhotoRoomManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PhotoRoomIndex = 0;
}

void APhotoRoomManager::BeginPlay()
{
	Super::BeginPlay();
    
    PrimaryActorTick.TickInterval = AutoUpdateInterval;
    
    TSubclassOf<AActor> ActorClass;
    ActorClass = APhotoRoom::StaticClass();
    
    for(TActorIterator<AActor> It(GetWorld(), ActorClass); It; ++It)
    {
        AActor* Actor = *It;
        if(!Actor->IsPendingKill())
        {
            //UE_LOG(SynthesisEngine, Warning, TEXT("Found PhotoRoom : %s"), *Actor->GetName());
            APhotoRoom * temp = Cast<APhotoRoom>(Actor);
            PhotoRoomInstances.Add(temp);
            temp->OnProcessDone().AddLambda([=]()
            {
                PhotoRoomIndex++;
                USynthesisEngineInstance* EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
                EngineInstance->Progress->Reset();
                
                APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
                if(PhotoRoomInstances.IsValidIndex(PhotoRoomIndex))
                {
                    playerController->SetViewTarget(PhotoRoomInstances[PhotoRoomIndex]);
                }
                
                temp->Destroy();
            });
        }
    }

    EngineInstance = Cast<USynthesisEngineInstance>(GetWorld()->GetGameInstance());
    EngineInstance->RegisterPhotoRoomManager(this);
    
    APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    playerController->Possess(this);
    if(PhotoRoomInstances.IsValidIndex(0))
    {
        playerController->SetViewTarget(PhotoRoomInstances[0]);
    }
}

void APhotoRoomManager::BeginDestroy()
{
	if (EngineInstance != nullptr)
	{
        EngineInstance->ReleasePhotoRoomManager(this);
	}
	Super::BeginDestroy();
}

// Called every frame
void APhotoRoomManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    //UE_LOG(SynthesisEngine, Warning, TEXT("PhotoRoomManager Tick : %f"), UGameplayStatics::GetRealTimeSeconds(GetWorld()));
    if(EngineInstance->b_UpdateAuto)
    {
        Update();
    }
}

void APhotoRoomManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAction("UpdateOnce", IE_Pressed, this, &APhotoRoomManager::Update); //L
    PlayerInputComponent->BindAction("Update", IE_Repeat, this, &APhotoRoomManager::Update); //P
    
    PlayerInputComponent->BindAction("Skip3DModel", IE_Pressed, this, &APhotoRoomManager::Skip3DModel); //Q
    PlayerInputComponent->BindAction("Skip3DModelAnimation", IE_Pressed, this, &APhotoRoomManager::Skip3DModelAnimation); //W
    PlayerInputComponent->BindAction("SkipClothes", IE_Pressed, this, &APhotoRoomManager::SkipClothes); //E

    PlayerInputComponent->BindAction("LoadNextLevel", IE_Pressed, this, &APhotoRoomManager::LoadNextLevel); //R
    PlayerInputComponent->BindAction("AutoUpdate", IE_Pressed, this, &APhotoRoomManager::ToggleAutoUpdate); //T
}

void APhotoRoomManager::Update()
{
	if (PhotoRoomInstances.IsValidIndex(PhotoRoomIndex))
	{
//		PhotoRoomInstances[PhotoRoomIndex]->UpdateWithLateDataFlushing();
        PhotoRoomInstances[PhotoRoomIndex]->UpdateWithLateDataFlushing_COCOTEMP();
	}
    else
    {
        LoadNextLevel();
    }
}

void APhotoRoomManager::ToggleAutoUpdate()
{
	EngineInstance->b_UpdateAuto = !EngineInstance->b_UpdateAuto;
}

void APhotoRoomManager::Skip3DModel()
{
    if (PhotoRoomInstances.IsValidIndex(PhotoRoomIndex))
    {
        PhotoRoomInstances[PhotoRoomIndex]->Skip3DModel();
    }
}
void APhotoRoomManager::Skip3DModelAnimation()
{
    if (PhotoRoomInstances.IsValidIndex(PhotoRoomIndex))
    {
        PhotoRoomInstances[PhotoRoomIndex]->Skip3DModelAnimation();
    }
}
void APhotoRoomManager::SkipClothes()
{
    if (PhotoRoomInstances.IsValidIndex(PhotoRoomIndex))
    {
        PhotoRoomInstances[PhotoRoomIndex]->SkipClothes();
    }
}

void APhotoRoomManager::LoadNextLevel()
{
	if (EngineInstance->LevelsToLoad.IsValidIndex(EngineInstance->LevelIndex))
	{
		UGameplayStatics::OpenLevel(this, EngineInstance->LevelsToLoad[EngineInstance->LevelIndex++]);
	}
}
