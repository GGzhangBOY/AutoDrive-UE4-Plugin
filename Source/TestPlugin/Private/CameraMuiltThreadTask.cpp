// THIS FILE IS PART OF UE4 LIDAR SIMULATE AND AUTO DRIVE SIMULATE PROJECT 
// THIS PROGRAM IS FREE SOFTWARE, IS LICENSED UNDER MIT
// Copyright(c) Bowei Zhang

#include "CameraMuiltThreadTask.h"


CameraMuiltThreadTask::~CameraMuiltThreadTask()
{
}

void CameraMuiltThreadTask::DoWork()
{
		((ACarCameraActor*)CarCameraActors[current_Num])->SaveRenderTargetToSharedMemary(((ACarCameraActor*)CarCameraActors[current_Num])->captureComponent->TextureTarget, this->cameraCacheController
			, current_Num, CarCameraActors.Num(), RTResource);
}
