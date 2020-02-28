// Fill out your copyright notice in the Description page of Project Settings.


#include "FoliageConverter.h"

// Sets default values
AFoliageConverter::AFoliageConverter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
    
    RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void AFoliageConverter::BeginPlay()
{
    Super::BeginPlay();
    TArray<FVector> converterWorldPositions;
    for(TActorIterator<AFoliageConverter> Iter(GetWorld()); Iter; ++Iter)
    {
        FVector pos = Iter->SceneComponent->GetComponentLocation();
        converterWorldPositions.Add(pos);
//        UE_LOG(SynthesisEngine, Warning, TEXT("pos : %s"), *pos.ToString());
    }
    
    for (TActorIterator<AInstancedFoliageActor> Iter(GetWorld()); Iter; ++Iter)
    {
        if (*Iter)
        {
            //Get the all InstancedStaticMeshComponent in current Level.
            TArray<UActorComponent*> InstancedStaticMeshCompArray = Iter->GetComponentsByClass(UInstancedStaticMeshComponent::StaticClass());
            
            for (UActorComponent* ActorComp : InstancedStaticMeshCompArray)
            {
                if (UInstancedStaticMeshComponent* InstancedComp = Cast<UInstancedStaticMeshComponent>(ActorComp))
                {
                    UStaticMesh* Mesh = InstancedComp->GetStaticMesh();
                    TArray<UMaterialInterface*> Materials = InstancedComp->GetMaterials();
                    
                    int count = InstancedComp->GetInstanceCount();
                    for(int i = 0;  i < count; i++)
                    {
                        FTransform transform;
                        InstancedComp->GetInstanceTransform(i, transform, true);
                        if(IsNearByConverter(converterWorldPositions, transform.GetLocation()))
                        {
                            FActorSpawnParameters params;
                            AStaticMeshActor * actor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), transform, params);
                            UStaticMeshComponent * comp = actor->GetStaticMeshComponent();
                            comp->SetStaticMesh(Mesh);
                            comp->bRenderCustomDepth = true;
                            comp->CustomDepthStencilValue = CustomStencilValueToApply;
                            InstancedComp->RemoveInstance(i);
                        }
                    }
//                    UE_LOG(SynthesisEngine, Error, TEXT("Static Mesh : %s"), *Mesh->GetName());
                }
            }
            break;
        }
    }
}

// Called every frame
void AFoliageConverter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AFoliageConverter::IsNearByConverter(TArray<FVector> Positions, FVector foliagePosition)
{
    for(const auto& pos : Positions)
    {
        if((pos-foliagePosition).Size() < Radius)
        {
            return true;
        }
    }
    return false;
}
