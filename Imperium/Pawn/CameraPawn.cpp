// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"
#include "Imperium/Construction/ConstructionBase.h"
#include "Imperium/GameMode/ImperiumGameMode.h"
#include "Components/BoxComponent.h"

// Sets default values
ACameraPawn::ACameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Spring Arm
	//Instantiating Class Components
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(GetRootComponent());
	CameraComp->AttachToComponent(SpringArmComp, FAttachmentTransformRules::KeepRelativeTransform);

	//Setting default properties of the SpringArmComp
	SpringArmComp->TargetArmLength = 2000.0f;
	SpringArmComp->bDoCollisionTest = false;
}

// Called when the game starts or when spawned
void ACameraPawn::BeginPlay()
{
	Super::BeginPlay();
	
	CameraLocation = GetActorLocation();
	Radius = sqrt(CameraLocation.X * CameraLocation.X + CameraLocation.Y * CameraLocation.Y + CameraLocation.Z * CameraLocation.Z);
}

void ACameraPawn::ProcessMouseDrag(float MouseLocationX, float MouseLocationY)
{
	// Move & Rotate this CameraPawn based on MouseLocation Change
	// Then set LastMouseLocation
	if (LastMouseLocationX == 0.f || LastMouseLocationY == 0.f)
	{
		LastMouseLocationX = MouseLocationX;
		LastMouseLocationY = MouseLocationY;
		return;
	}

	CameraLocation = GetActorLocation();
	FRotator RadiusRotation = CameraLocation.Rotation();
	

	// RadiusRotation.Yaw -= (LastMouseLocationX - MouseLocationX) / 10.f;
	// Because of Gimbal Lock, pitch is limited to (-90, 90), to set pitch over 90, use AddActorLocalRotation
	FRotator RotationDelta = FRotator((LastMouseLocationY - MouseLocationY) / 10.f, -(LastMouseLocationX - MouseLocationX) / 10.f, 0.f);
	AddActorLocalRotation(RotationDelta);

	LastMouseLocationX = MouseLocationX;
	LastMouseLocationY = MouseLocationY;
}

void ACameraPawn::ResetMouseLocation()
{
	LastMouseLocationX = 0.f;
	LastMouseLocationY = 0.f;
}

void ACameraPawn::ProcessMouseWheelScroll(float MouseWheelAxis)
{
	if (ConstructingObject != nullptr)
	{
		// Rotate the Constructing Object
		AddRotationPitch += 4 * MouseWheelAxis;
	}
	else
	{
		SpringArmComp->TargetArmLength += -MouseWheelAxis * 50.f;
		if (SpringArmComp->TargetArmLength < MinZoomDistance)
		{
			SpringArmComp->TargetArmLength = MinZoomDistance;
		}
		if (SpringArmComp->TargetArmLength > MaxZoomDistance)
		{
			SpringArmComp->TargetArmLength = MaxZoomDistance;
		}
	}
}

void ACameraPawn::UpdateConstructingObject()
{
	if (ConstructingObject != nullptr)
	{
		// Transform cursor location to world location
		// Line trace from this location to the globe
		// Get impact position
		// Generate rotation based on normal
		FVector MouseWorldLocation, MouseWorldDirection;
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ConstructionStatus NewConstructionStatus = ConstructionStatus::Constructing;
		if (PlayerController != nullptr)
		{
			bool bSuccess = PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
			if (bSuccess)
			{
				FHitResult OutHit;
				bSuccess = GetWorld()->LineTraceSingleByChannel(OutHit, MouseWorldLocation, MouseWorldLocation + MouseWorldDirection * 10000, ECollisionChannel::ECC_GameTraceChannel1);
				if (bSuccess)
				{
					FRotator ConstructionRotator = FRotationMatrix::MakeFromZ(OutHit.ImpactNormal).Rotator();
					// ConstructionRotator.Pitch += AddRotationPitch;
					FTransform ConstructionTransform(ConstructionRotator, OutHit.ImpactPoint);
					ConstructingObject->SetActorTransform(ConstructionTransform);
					ConstructingObject->AddActorLocalRotation(FRotator(0, AddRotationPitch, 0));

					//UE_LOG(LogTemp, Warning, TEXT("CollisionCounter is %d"), ConstructingObject->GetCollisionCounter());
					if (ConstructingObject->GetCollisionCounter() > 0)
					{
						NewConstructionStatus = ConstructionStatus::Unconstructable;
						//UE_LOG(LogTemp, Warning, TEXT("111"));
					}
					else
					{
						NewConstructionStatus = ConstructionStatus::Constructing;
						//UE_LOG(LogTemp, Warning, TEXT("222"));
					}
				}
				else
				{
					FTransform ConstructionTransform(FRotator(0, 0, 0), FVector(0, 0, 0));
					ConstructingObject->SetActorTransform(ConstructionTransform);
					NewConstructionStatus = ConstructionStatus::Unconstructable;
					//UE_LOG(LogTemp, Warning, TEXT("333"));
				}
			}
		}
		UpdateMaterial(NewConstructionStatus);
	}
}

void ACameraPawn::UpdateMaterial(ConstructionStatus NewConstructionStatus)
{
	// Check if ConstructionStatus changed, if yes, change material instance
	// ConstructionStatusNumber 1: Constructing, 2: Constructed, 3: UnConstructable
	if (ConstructingObject->ConstructionStatus != NewConstructionStatus)
	{
		AImperiumGameMode* ImperiumGameMode = Cast<AImperiumGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		UMaterialInterface* ConstructingMaterial = ImperiumGameMode->ConstructingMaterial;
		UMaterialInterface* UnconstructableMaterial = ImperiumGameMode->UnconstructableMaterial;

		TArray<UMaterialInterface*> Materials = ConstructingObject->GetStaticMesh()->GetMaterials();
		int32 MaterialSlot = 0;
		for (UMaterialInterface* Material : Materials)
		{
			switch (NewConstructionStatus)
			{
			case ConstructionStatus::Constructing:
				if (ConstructingMaterial != nullptr)
				{
					ConstructingObject->GetStaticMesh()->SetMaterial(MaterialSlot, ConstructingMaterial);
				}
				break;
			case ConstructionStatus::Constructed:
				break;
			case ConstructionStatus::Unconstructable:
				if (ConstructingMaterial != nullptr)
				{
					ConstructingObject->GetStaticMesh()->SetMaterial(MaterialSlot, UnconstructableMaterial);
				}
				break;
			case ConstructionStatus::None:
				break;
			default:
				break;
			}
			MaterialSlot += 1;
		}

		ConstructingObject->ConstructionStatus = NewConstructionStatus;
	}
}

// Called every frame
void ACameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update ConstructingObject position on cursor if 1 construction has been selected 
	UpdateConstructingObject();
}

// Called to bind functionality to input
void ACameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACameraPawn::SpawnConstructionOnCursor()
{
	// Spawn a ConstructingObject at (0, 0, 0) first
	ConstructingObject = SpawnConstructionAtPosition(FVector(0, 0, 0), FRotator(0, 0, 0));
	ConstructingObject->ConstructionStatus = ConstructionStatus::None;
}

void ACameraPawn::CancelConstruction()
{
	if (ConstructingObject != nullptr)
	{
		ConstructingObject->Destroy();
		ConstructingObject = nullptr;
	}
}

void ACameraPawn::ConfirmConstruction()
{
	if (ConstructingObject != nullptr)
	{
		// Check if this place is constructable
		// if yes, Play Confirm Sound
		// construct, Set Materials to original ones, set SelectedConstruction to None
		// Set ConstructionStatus to constructed
		// Then set ConstructingObject to null
		// if no, Play Error Sound
		if (ConstructingObject->ConstructionStatus == ConstructionStatus::Constructing)
		{
			PlayConfirmSound();
			ConstructingObject->RecoverOriginalMaterials();
			SelectedConstruction = ConstructionName::None;
			ConstructingObject->ConstructionStatus = ConstructionStatus::Constructed;
			ConstructingObject = nullptr;
		}
		else
		{
			PlayErrorSound();
		}
	}
}

