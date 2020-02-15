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
		if (((ACarCameraActor*)CarCameraActors[i])->enableDatasetMaking)
		{
			TArray<FVector2D> in_GT_Results_TopLeft; 
			TArray<FVector2D> in_GT_Results_WidthHeight;
			GetCameraBoundingBox(((ACarCameraActor*)CarCameraActors[i])->captureComponent, in_GT_Results_TopLeft, in_GT_Results_WidthHeight);
			((ACarCameraActor*)CarCameraActors[i])->SaveMarkedTargetToSharedMemary(this->cameraCacheController, in_GT_Results_TopLeft, in_GT_Results_WidthHeight,
				i, CarCameraActors.Num());
		}
	}//Old way: doing this in single thread
	/*for (int i = 0; i < CarCameraActors.Num(); i++)
	{
		auto task = new FAutoDeleteAsyncTask<CameraMuiltThreadTask>(CarCameraActors, cameraCacheController,
			((ACarCameraActor*)CarCameraActors[i])->captureComponent->TextureTarget->GameThread_GetRenderTargetResource(),i);
		if (task) task->StartBackgroundTask();
	}*/ // Doing this in mult thread TODO
}

bool CaptureComponent2D_Project(USceneCaptureComponent2D* Target, FVector Location, FVector2D& OutPixelLocation)
{
	if ((Target == nullptr) || (Target->TextureTarget == nullptr))
	{
		return false;
	}
	float GProjectionSignY = 1.0f; // Unable to find the header of this variable, but the value 1.0 is write inside the RHI.cpp
	const FTransform& Transform = Target->GetComponentToWorld();
	FMatrix ViewMatrix = Transform.ToInverseMatrixWithScale();
	FVector ViewLocation = Transform.GetTranslation();

	// swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
	ViewMatrix = ViewMatrix * FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1));

	const float FOV = Target->FOVAngle * (float)PI / 360.0f;

	FIntPoint CaptureSize(Target->TextureTarget->GetSurfaceWidth(), Target->TextureTarget->GetSurfaceHeight());

	float XAxisMultiplier;
	float YAxisMultiplier;

	if (CaptureSize.X > CaptureSize.Y)
	{
		// if the viewport is wider than it is tall
		XAxisMultiplier = 1.0f;
		YAxisMultiplier = CaptureSize.X / (float)CaptureSize.Y;
	}
	else
	{
		// if the viewport is taller than it is wide
		XAxisMultiplier = CaptureSize.Y / (float)CaptureSize.X;
		YAxisMultiplier = 1.0f;
	}

	FMatrix    ProjectionMatrix = FReversedZPerspectiveMatrix(
		FOV,
		FOV,
		XAxisMultiplier,
		YAxisMultiplier,
		GNearClippingPlane,
		GNearClippingPlane
	);

	FMatrix ViewProjectionMatrix = ViewMatrix * ProjectionMatrix;

	FVector4 ScreenPoint = ViewProjectionMatrix.TransformFVector4(FVector4(Location, 1));

	if (ScreenPoint.W > 0.0f)
	{
		float InvW = 1.0f / ScreenPoint.W;
		float Y = (GProjectionSignY > 0.0f) ? ScreenPoint.Y : 1.0f - ScreenPoint.Y;
		FIntRect ViewRect = FIntRect(0, 0, CaptureSize.X, CaptureSize.Y);
		OutPixelLocation = FVector2D(
			ViewRect.Min.X + (0.5f + ScreenPoint.X * 0.5f * InvW) * ViewRect.Width(),
			ViewRect.Min.Y + (0.5f - Y * 0.5f * InvW) * ViewRect.Height()
		);
		return true;
	}

	return false;
}

void ACarCameraController::GetCameraBoundingBox(USceneCaptureComponent2D* Target, TArray<FVector2D>& GT_Results_TopLeft, TArray<FVector2D>& GT_Results_WidthHeight)
{
	TArray<AActor*> Actors;

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "TrackingVehicle", Actors);

	TArray<FVector> Bound_Origin, Bound_Extend, Actors_Position;

	for (int i = 0; i < Actors.Num(); i++)
	{
		if (Actors[i]->WasRecentlyRendered())
		{
			FVector C_Bound_Origin, C_Bound_Extend;
			Actors[i]->GetActorBounds(false, C_Bound_Origin, C_Bound_Extend);
			Bound_Origin.Add(C_Bound_Origin);
			Bound_Extend.Add(C_Bound_Extend);


			Actors_Position.Add(Actors[i]->GetActorLocation());
		}
	}

	RootState E_Rstate = RootState::E_ROOT_BOTTOM;

	TArray<FVector>::TConstIterator ite_Pos = Actors_Position.CreateConstIterator();
	TArray<FVector>::TConstIterator ite_Bound = Bound_Extend.CreateConstIterator();
	FIntPoint CaptureSize(Target->TextureTarget->GetSurfaceWidth(), Target->TextureTarget->GetSurfaceHeight());

	for (int i = 0; i < Actors_Position.Num(); i++, ite_Pos++, ite_Bound)
	{
		FVector C_Pos, C_Bound;

		C_Pos = *ite_Pos;
		C_Bound = (*ite_Bound);

		TArray<FVector*> PointsCollection;
		if (E_Rstate == RootState::E_ROOT_BOTTOM)
		{
			PointsCollection.Add(new FVector(C_Pos.X + C_Bound.X, C_Pos.Y + C_Bound.Y, C_Pos.Z + C_Bound.Z * 0));
			PointsCollection.Add(new FVector(C_Pos.X + C_Bound.X, C_Pos.Y + C_Bound.Y, C_Pos.Z + C_Bound.Z * 2));
			PointsCollection.Add(new FVector(C_Pos.X - C_Bound.X, C_Pos.Y + C_Bound.Y, C_Pos.Z + C_Bound.Z * 0));
			PointsCollection.Add(new FVector(C_Pos.X - C_Bound.X, C_Pos.Y + C_Bound.Y, C_Pos.Z + C_Bound.Z * 2));
			PointsCollection.Add(new FVector(C_Pos.X + C_Bound.X, C_Pos.Y - C_Bound.Y, C_Pos.Z + C_Bound.Z * 0));
			PointsCollection.Add(new FVector(C_Pos.X + C_Bound.X, C_Pos.Y - C_Bound.Y, C_Pos.Z + C_Bound.Z * 2));
			PointsCollection.Add(new FVector(C_Pos.X - C_Bound.X, C_Pos.Y - C_Bound.Y, C_Pos.Z + C_Bound.Z * 0));
			PointsCollection.Add(new FVector(C_Pos.X - C_Bound.X, C_Pos.Y - C_Bound.Y, C_Pos.Z + C_Bound.Z * 2));
		}
		else
		{
			PointsCollection.Add(new FVector(C_Pos.X + C_Bound.X, C_Pos.Y + C_Bound.Y, C_Pos.Z + C_Bound.Z * -1));
			PointsCollection.Add(new FVector(C_Pos.X + C_Bound.X, C_Pos.Y + C_Bound.Y, C_Pos.Z + C_Bound.Z * 1));
			PointsCollection.Add(new FVector(C_Pos.X - C_Bound.X, C_Pos.Y + C_Bound.Y, C_Pos.Z + C_Bound.Z * -1));
			PointsCollection.Add(new FVector(C_Pos.X - C_Bound.X, C_Pos.Y + C_Bound.Y, C_Pos.Z + C_Bound.Z * 1));
			PointsCollection.Add(new FVector(C_Pos.X + C_Bound.X, C_Pos.Y - C_Bound.Y, C_Pos.Z + C_Bound.Z * -1));
			PointsCollection.Add(new FVector(C_Pos.X + C_Bound.X, C_Pos.Y - C_Bound.Y, C_Pos.Z + C_Bound.Z * 1));
			PointsCollection.Add(new FVector(C_Pos.X - C_Bound.X, C_Pos.Y - C_Bound.Y, C_Pos.Z + C_Bound.Z * -1));
			PointsCollection.Add(new FVector(C_Pos.X - C_Bound.X, C_Pos.Y - C_Bound.Y, C_Pos.Z + C_Bound.Z * 1));
		}
		int Scr_Pos_Counter = 0;
		float maxX = -CaptureSize.X, maxY = -CaptureSize.Y, minX = CaptureSize.X, minY = CaptureSize.Y;
		FGroundTruthInfo C_GT_Info;
		bool valid_GT = false;
		for (TArray<FVector*>::TConstIterator iter = PointsCollection.CreateConstIterator(); iter; iter++)
		{
			FVector2D C_Scr_Pos;
			if (CaptureComponent2D_Project(Target, *(*iter), C_Scr_Pos))
			{
				if (C_Scr_Pos.X > maxX)
					maxX = C_Scr_Pos.X;
				if (C_Scr_Pos.X < minX)
					minX = C_Scr_Pos.X;
				if (C_Scr_Pos.Y > maxY)
					maxY = C_Scr_Pos.Y;
				if (C_Scr_Pos.Y < minY)
					minY = C_Scr_Pos.Y;

				valid_GT = true;
			}
			else
			{
				valid_GT = false;
				break;
			}
		}

		if (minX > 0 && minY > 0 && valid_GT)
		{		
			GT_Results_TopLeft.Add(FVector2D(minX, minY));
			GT_Results_WidthHeight.Add(FVector2D(abs(maxX - minX), abs(maxY - minY)));
		}
	}

	
}



