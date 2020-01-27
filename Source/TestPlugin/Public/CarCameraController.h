// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarCameraActor.h"
#include "CacheDataInterface.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/SceneCaptureComponent2D.h"
#include "CameraMuiltThreadTask.h"
#include "CarCameraController.generated.h"

UCLASS()
class TESTPLUGIN_API ACarCameraController : public AActor
{
	GENERATED_BODY()

private:
	TArray<AActor*> CarCameraActors;
	CacheDataInterface cameraCacheController;
public:	
	// Sets default values for this actor's properties
	ACarCameraController();
	 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
