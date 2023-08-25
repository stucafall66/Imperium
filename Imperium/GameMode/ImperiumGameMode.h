// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ImperiumGameMode.generated.h"

/**
 * 
 */
UCLASS()
class IMPERIUM_API AImperiumGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* ConstructingMaterial;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* UnconstructableMaterial;
};
