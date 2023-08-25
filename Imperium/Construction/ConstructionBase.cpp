// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructionBase.h"
#include "Components/BoxComponent.h"

// Sets default values
AConstructionBase::AConstructionBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(StaticMesh);
}

// Called when the game starts or when spawned
void AConstructionBase::BeginPlay()
{
	Super::BeginPlay();

	// Save all original materials
	OriginalMaterials = StaticMesh->GetMaterials();
	// UE_LOG(LogTemp, Warning, TEXT("111OriginalMaterials Length is %d"), StaticMesh->GetNumMaterials());

	if (CollisionBox != nullptr)
	{
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
		CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
	}
}

void AConstructionBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
	{
		AConstructionBase* OtherConstructionActor = Cast<AConstructionBase>(OtherActor);
		if (OtherConstructionActor != nullptr && OtherConstructionActor != this)
		{
			CollisionCounter += 1;
		}
	}
}

void AConstructionBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != nullptr)
	{
		AConstructionBase* OtherConstructionActor = Cast<AConstructionBase>(OtherActor);
		if (OtherConstructionActor != nullptr && OtherConstructionActor != this)
		{
			CollisionCounter -= 1;
		}
	}
}

// Called every frame
void AConstructionBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AConstructionBase::RecoverOriginalMaterials()
{
	int32 MaterialSlot = 0;
	UE_LOG(LogTemp, Warning, TEXT("OriginalMaterials Length is %d"), OriginalMaterials.Num());
	
	for (UMaterialInterface* OriginalMaterial : OriginalMaterials)
	{
		StaticMesh->SetMaterial(MaterialSlot, OriginalMaterial);
		MaterialSlot += 1;
	}
}

