// Fill out your copyright notice in the Description page of Project Settings.

#include "CarCameraController.h"

// Sets default values
ACarCameraController::ACarCameraController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACarCameraController::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GWorld->GetWorld(), ACarCameraActor::StaticClass(), CarCameraActors);//Waring: remember to check the number of BP cars in level blueprint
}

// Called every frame
void ACarCameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (int i = 0; i < CarCameraActors.Num(); i++)
	{
		((ACarCameraActor*)CarCameraActors[i])->SaveRenderTargetToSharedMemary(((ACarCameraActor*)CarCameraActors[i])->captureComponent->TextureTarget, this->cameraCacheController
			, i, CarCameraActors.Num(), ((ACarCameraActor*)CarCameraActors[i])->captureComponent->TextureTarget->GameThread_GetRenderTargetResource());
	}//Old way: doing this in single thread
	/*for (int i = 0; i < CarCameraActors.Num(); i++)
	{
		auto task = new FAutoDeleteAsyncTask<CameraMuiltThreadTask>(CarCameraActors, cameraCacheController,
			((ACarCameraActor*)CarCameraActors[i])->captureComponent->TextureTarget->GameThread_GetRenderTargetResource(),i);
		if (task) task->StartBackgroundTask();
	}*/ // Doing this in mult thread TODO
}

