// THIS FILE IS PART OF UE4 LIDAR SIMULATE AND AUTO DRIVE SIMULATE PROJECT 
// THIS PROGRAM IS FREE SOFTWARE, IS LICENSED UNDER MIT
// Copyright(c) Bowei Zhang

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
	
}

void ACarCameraActor::SaveRenderTargetToSharedMemary(UTextureRenderTarget2D* InRenderTarget, CacheDataInterface &in_cache_controller, int current_num, int num_camera, FTextureRenderTargetResource* RTResource)
{
	FReadSurfaceDataFlags ReadPixelFlags(RCM_UNorm);
	ReadPixelFlags.SetLinearToGamma(true);
	int pic_width = InRenderTarget->GetSurfaceWidth();
	int pic_height = InRenderTarget->GetSurfaceHeight();
	pixel_structure* pixel_array = new pixel_structure[pic_width*pic_height];
	FColor* OutBMP = new FColor[pic_width*pic_height];

	//RTResource->ReadPixels(OutBMP, ReadPixelFlags);
	RTResource->ReadPixelsPtr(OutBMP);
	memcpy(pixel_array, OutBMP, sizeof(FColor)*pic_width*pic_height);
	delete[] OutBMP;
	//int i = 0;
	/*for (FColor& color : OutBMP)
	{
		color.A = 255;
		pixel_array[i] = pixel_structure{ char(color.R), char(color.G), char(color.B)};
		i++;
	}*/
	in_cache_controller.writeCurrentCameraCache(pixel_array, pic_width*pic_height, pic_width, pic_height, current_num, num_camera);
}

void ACarCameraActor::SaveMarkedTargetToSharedMemary(CacheDataInterface &in_cache_controller, TArray<FVector2D>& GT_Results_TopLeft, TArray<FVector2D>& GT_Results_WidthHeight, int current_num, int num_camera)
{
	in_cache_controller.writeCurrentCarBoundingBoxInfo(GT_Results_TopLeft, GT_Results_WidthHeight, current_num, num_camera);
}

void ACarCameraActor::releaseActorCameraCache()
{
	this->cacheController.releaseCurrentCameraCache();
}

