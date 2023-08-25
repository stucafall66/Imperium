// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Imperium/Type/Type.h"
#include "ConstructionBase.generated.h"

class UBoxComponent;

UCLASS()
class IMPERIUM_API AConstructionBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConstructionBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE UStaticMeshComponent* GetStaticMesh() { return StaticMesh; };
	FORCEINLINE UBoxComponent* GetCollisionBox() { return CollisionBox; };
	FORCEINLINE int32 GetCollisionCounter() { return CollisionCounter; };

	void RecoverOriginalMaterials();

public:
	ConstructionStatus ConstructionStatus{ ConstructionStatus::None };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* CollisionBox;

	UPROPERTY()
	TArray<UMaterialInterface*> OriginalMaterials;

	int32 CollisionCounter{ 0 };
};
