// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Imperium/Type/Type.h"
#include "CameraPawn.generated.h"

class AConstructionBase;

UCLASS()
class IMPERIUM_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACameraPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SpawnConstructionOnCursor();

	UFUNCTION(BlueprintCallable)
	void CancelConstruction();

	UFUNCTION(BlueprintCallable)
	void ConfirmConstruction();

	UFUNCTION(BlueprintImplementableEvent)
	AConstructionBase* SpawnConstructionAtPosition(FVector Position, FRotator Rotation);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayConfirmSound();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayErrorSound();

public:
	UPROPERTY(BlueprintReadWrite)
	ConstructionName SelectedConstruction {ConstructionName::None};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION(BlueprintCallable)
	void ProcessMouseDrag(float MouseLocationX, float MouseLocationY);

	UFUNCTION(BlueprintCallable)
	void ResetMouseLocation();

	UFUNCTION(BlueprintCallable)
	void ProcessMouseWheelScroll(float MouseWheelAxis);

	void UpdateConstructingObject();

	void UpdateMaterial(ConstructionStatus NewConstructionStatus);

private:
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* CameraComp;

	float LastMouseLocationX{ 0.f };
	float LastMouseLocationY{ 0.f };
	FVector CameraLocation;
	FRotator CameraRotation;
	float Radius{ 0.f };

	float MinZoomDistance{ 1000.f };
	float MaxZoomDistance{ 3000.f };

	UPROPERTY()
	AConstructionBase* ConstructingObject{nullptr};

	float AddRotationPitch{ 0.f };
};
