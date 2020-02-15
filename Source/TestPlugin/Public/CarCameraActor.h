// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Engine/Public/TextureResource.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Public/HighResScreenshot.h"
#include "Runtime/Core/Public/Windows/WindowsPlatformMemory.h"
#include "CacheDataInterface.h"
#include "stdio.h"
#include "vector.h"
#include "WindowsSharedMemory.h"
#include "CarCameraActor.generated.h"

UCLASS()
class TESTPLUGIN_API ACarCameraActor : public AActor
{
	GENERATED_BODY()
private:
	CacheDataInterface cacheController;
public:	
	// Sets default values for this actor's properties
	USceneCaptureComponent2D* captureComponent;
	void SaveRenderTargetToSharedMemary(UTextureRenderTarget2D* InRenderTarget, CacheDataInterface &in_cache_controller, int current_num, int num_camera, FTextureRenderTargetResource* RTResource);
	void SaveMarkedTargetToSharedMemary(CacheDataInterface &in_cache_controller, TArray<FVector2D>& GT_Results_TopLeft, TArray<FVector2D>& GT_Results_WidthHeight, int current_num, int num_camera);
	UPROPERTY(EditAnywhere)
		FString randerPath = "";
	UPROPERTY(EditAnywhere)
		bool enableDatasetMaking = false;
	ACarCameraActor();
	void releaseActorCameraCache();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
