// Fill out your copyright notice in the Description page of Project Settings.

#include "CarCameraActor.h"

// Sets default values
ACarCameraActor::ACarCameraActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACarCameraActor::BeginPlay()
{
	Super::BeginPlay();
	this->captureComponent = (USceneCaptureComponent2D*)this->GetComponentByClass(USceneCaptureComponent2D::StaticClass());
	
}

// Called every frame
void ACarCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//SaveRenderTargetToSharedMemary(this->captureComponent->TextureTarget);
}

void ACarCameraActor::SaveRenderTargetToSharedMemary(UTextureRenderTarget2D* InRenderTarget, CacheDataInterface &in_cache_controller, int current_num, int num_camera)
{
	FTextureRenderTargetResource* RTResource = InRenderTarget->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	ReadPixelFlags.SetLinearToGamma(true);
	int pic_width = InRenderTarget->GetSurfaceWidth();
	int pic_height = InRenderTarget->GetSurfaceHeight();
	pixel_structure* pixel_array = new pixel_structure[pic_width*pic_height];
	TArray<FColor> OutBMP;

	RTResource->ReadPixels(OutBMP, ReadPixelFlags);

	int i = 0;
	for (FColor& color : OutBMP)
	{
		color.A = 255;
		pixel_array[i] = pixel_structure{ color.R, color.G, color.B, color.A, pic_height, pic_height };
		i++;
	}
	
	in_cache_controller.writeCurrentCameraCache(pixel_array, i, current_num, num_camera);
}

void ACarCameraActor::releaseActorCameraCache()
{
	this->cacheController.releaseCurrentCameraCache();
}

