// THIS FILE IS PART OF UE4 LIDAR SIMULATE AND AUTO DRIVE SIMULATE PROJECT 
// THIS PROGRAM IS FREE SOFTWARE, IS LICENSED UNDER MIT
// Copyright(c) Bowei Zhang

#pragma once

#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarCameraActor.h"
#include "CacheDataInterface.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Async/AsyncWork.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetSystemLibrary.h"

/**
 * 
 */
class TESTPLUGIN_API CameraMuiltThreadTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<CameraMuiltThreadTask>;
private:
	TArray<AActor*> CarCameraActors;
	CacheDataInterface cameraCacheController;
	FTextureRenderTargetResource* RTResource;
	int current_Num;
public:
	CameraMuiltThreadTask(TArray<AActor*> in_CarCameraActors, CacheDataInterface& in_cameraCacheController, FTextureRenderTargetResource* in_RTResource, int in_current_Num)
		:CarCameraActors(in_CarCameraActors),
		cameraCacheController(in_cameraCacheController),
		RTResource(in_RTResource),
		current_Num(in_current_Num)
	{

	}

	~CameraMuiltThreadTask();

	void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAutoDeleteAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};
