// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraMuiltThreadTask.h"


CameraMuiltThreadTask::~CameraMuiltThreadTask()
{
}

void CameraMuiltThreadTask::DoWork()
{
		((ACarCameraActor*)CarCameraActors[current_Num])->SaveRenderTargetToSharedMemary(((ACarCameraActor*)CarCameraActors[current_Num])->captureComponent->TextureTarget, this->cameraCacheController
			, current_Num, CarCameraActors.Num(), RTResource);
}
