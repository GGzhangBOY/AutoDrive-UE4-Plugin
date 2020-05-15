// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// THIS FILE IS PART OF UE4 LIDAR SIMULATE AND AUTO DRIVE SIMULATE PROJECT 
// THIS PROGRAM IS FREE SOFTWARE, IS LICENSED UNDER MIT
// Copyright(c) Bowei Zhang

#include "TestPluginBPLibrary.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "TestPlugin.h"

bool has_taken_screen_shot = true;
CacheDataInterface* cacheController = NULL;
TArray<FGroundTruthInfo> GroundTruthInfo_Global;
FDelegateHandle delegateHandle;
EImageFormat UTestPluginBPLibrary::format = EImageFormat::JPEG;
int32 UTestPluginBPLibrary::quality = 100;
int32 UTestPluginBPLibrary::numPic = 0;
OutputColorState UTestPluginBPLibrary::colorChannel = OutputColorState::E_COLOR_GRAY;
FString UTestPluginBPLibrary::filePath = ".\\MakeDataSet";


UTestPluginBPLibrary::UTestPluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

TArray<FGroundTruthInfo> UTestPluginBPLibrary::GetActorsGTBox(UObject* SelfPointer, FName TagName, RootState E_Rstate)
{
	TArray<AActor*> Actors;
	
	UGameplayStatics::GetAllActorsWithTag(SelfPointer, TagName, Actors);

	TArray<FVector> Bound_Origin, Bound_Extend, Actors_Position;
	for(TArray<AActor*>::TConstIterator iter = Actors.CreateConstIterator(); iter;iter++)
	{
		if ((*iter)->WasRecentlyRendered())
		{
			FVector C_Bound_Origin, C_Bound_Extend;
			(*iter)->GetActorBounds(false, C_Bound_Origin, C_Bound_Extend);
			Bound_Origin.Add(C_Bound_Origin);
			Bound_Extend.Add(C_Bound_Extend);

			Actors_Position.Add((*iter)->GetActorLocation());
		}
		continue;
	}

	return CalculateBound(SelfPointer, Actors_Position, Bound_Extend,E_Rstate);
}

void UTestPluginBPLibrary::OnScreenshotCapturedInternal(int32 Width, int32 Height, const TArray<FColor>& Bitmap)
{
	if (has_taken_screen_shot == false)
	{
		FString GT_Info_Path = filePath + FString("\\GroundTruthInfo.txt");
		std::ofstream GT_Info_File(*GT_Info_Path, std::ios::app);
		FString fileName = UTestPluginBPLibrary::filePath + "\\\\" + FString::FromInt(UTestPluginBPLibrary::numPic);
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper;
		ERGBFormat setColor = (UTestPluginBPLibrary::colorChannel == OutputColorState::E_COLOR_GRAY) ? ERGBFormat::BGRA : ERGBFormat::Gray;
		if (UTestPluginBPLibrary::format == EImageFormat::PNG)
		{
			TArray<uint8> PNGData;
			TArray<FColor> BitmapCopy(Bitmap);
			FImageUtils::CompressImageArray(Width, Height, BitmapCopy, PNGData);

			fileName += ".png";
			FFileHelper::SaveArrayToFile(PNGData, *fileName);
		}
		else if (UTestPluginBPLibrary::format == EImageFormat::JPEG)
		{
			ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
			ImageWrapper->SetRaw(Bitmap.GetData(), Bitmap.GetAllocatedSize(), Width, Height, setColor, 8);
			const TArray<uint8>& JPEGData = ImageWrapper->GetCompressed(UTestPluginBPLibrary::quality);
			fileName += ".jpg";
			FFileHelper::SaveArrayToFile(JPEGData, *fileName);
		}
		else if (UTestPluginBPLibrary::format == EImageFormat::EXR)
		{
			ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
			ImageWrapper->SetRaw(Bitmap.GetData(), Bitmap.GetAllocatedSize(), Width, Height, setColor, 8);
			const TArray<uint8>& Data = ImageWrapper->GetCompressed(UTestPluginBPLibrary::quality);
			fileName += ".exr";
			FFileHelper::SaveArrayToFile(Data, *fileName);
		}
		else
		{
			ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
			ImageWrapper->SetRaw(Bitmap.GetData(), Bitmap.GetAllocatedSize(), Width, Height, setColor, 8);
			const TArray<uint8>& JPEGData = ImageWrapper->GetCompressed(quality);
			fileName += ".jpg";
			FFileHelper::SaveArrayToFile(JPEGData, *fileName);
		}
		UE_LOG(LogTemp, Warning, TEXT("Calling reaction ,Path is %s"), *fileName)
			std::vector<char*> co_storer;
		for (int i = 0; i < GroundTruthInfo_Global.Num(); i++)
		{
			char* str = new char[1000];
			//follow the order of LetfTop-x,LeftTop-y,rectangle width,rectangle height to write to file
			sprintf(str, "%f %f %f %f", GroundTruthInfo_Global[i].TopLeft.X, GroundTruthInfo_Global[i].TopLeft.Y, GroundTruthInfo_Global[i].WidthHeight.X, GroundTruthInfo_Global[i].WidthHeight.Y);
			co_storer.push_back(str);
		}
		GT_Info_File << TCHAR_TO_UTF8(*fileName) << "\n" << GroundTruthInfo_Global.Num() << "\n";
		for (int i = 0; i < co_storer.size(); i++)
		{
			GT_Info_File << co_storer[i] << "\n";
		}
		has_taken_screen_shot = true;
		GT_Info_File.close();
		UGameViewportClient::OnScreenshotCaptured().Remove(delegateHandle);
		UTestPluginBPLibrary::numPic++;
		ImageWrapper.Reset();
	}
}


bool UTestPluginBPLibrary::MakeDataSetFromGroundTruth(FString Folder, TArray<FGroundTruthInfo>GroundTruthInfo)
{
	UTestPluginBPLibrary::filePath = Folder;
	GroundTruthInfo_Global = GroundTruthInfo;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Requesting screenshot")); // display message on window (to check that you are actually calling the lines below)
	has_taken_screen_shot = false;
	FDelegateHandle delegateHandle = UGameViewportClient::OnScreenshotCaptured().AddStatic(&UTestPluginBPLibrary::OnScreenshotCapturedInternal);
	FScreenshotRequest::RequestScreenshot(false);
	UE_LOG(LogTemp, Warning, TEXT("Requesting Screenshot"))

	return true;
}

bool UTestPluginBPLibrary::AdjustDatasetParameter(ImageFormat imageFormat, int32 imageQuality, OutputColorState imageColorChannel)
{
	UTestPluginBPLibrary::format = (imageFormat==ImageFormat::JPEG)? EImageFormat::JPEG : EImageFormat::PNG;
	UTestPluginBPLibrary::quality = imageQuality;
	UTestPluginBPLibrary::colorChannel = imageColorChannel;

	return true;
}

TArray<FGroundTruthInfo> UTestPluginBPLibrary::CalculateBound(UObject* SelfPointer, TArray<FVector> ActorPosition, TArray<FVector> Bound_Extend, RootState E_Rstate)
{
	TArray<FGroundTruthInfo > GT_Results;
	TArray<FVector>::TConstIterator ite_Pos = ActorPosition.CreateConstIterator();
	TArray<FVector>::TConstIterator ite_Bound = Bound_Extend.CreateConstIterator();
	
	for (int i = 0; i < ActorPosition.Num(); i++, ite_Pos++, ite_Bound)
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
		FVector2D maxCo(-9999, -9999), minCo(9999, 9999);
		float maxX = -9999, maxY = -9999, minX = 9999, minY = 9999;
		FGroundTruthInfo C_GT_Info;
		for (TArray<FVector*>::TConstIterator iter = PointsCollection.CreateConstIterator(); iter; iter++)
		{
			FVector2D C_Scr_Pos;
			UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(SelfPointer, 0), *(*iter), C_Scr_Pos);

			if (C_Scr_Pos.X > maxX)
				maxX = C_Scr_Pos.X;
			if (C_Scr_Pos.X < minX)
				minX = C_Scr_Pos.X;
			if (C_Scr_Pos.Y > maxY)
				maxY = C_Scr_Pos.Y;
			if (C_Scr_Pos.Y < minY)
				minY = C_Scr_Pos.Y;
		}
		
		if (minX > 0 && minY > 0)
		{
			C_GT_Info.WidthHeight.X = abs(maxX - minX);
			C_GT_Info.WidthHeight.Y = abs(maxY - minY);
			C_GT_Info.TopLeft = FVector2D(minX, minY);

			GT_Results.Add(C_GT_Info);
		}
	}
	return GT_Results;
}

TArray<AActor*> UTestPluginBPLibrary::GetStaticMeshVertex(UObject* worldPointer, FString targetFolder, FString optixRendererPath)
{
	TArray<AActor*> T_output_array;
	TArray<AActor*> T_road_spline_actors;
	std::vector<std::vector<FVector>> Vertex_output;
	UGameplayStatics::GetAllActorsOfClass(worldPointer, AStaticMeshActor::StaticClass(), T_output_array);
	UGameplayStatics::GetAllActorsWithTag(worldPointer, "SplineRoad", T_road_spline_actors);
	T_output_array.Append(T_road_spline_actors);
	MeshFactory meshFactory;
	FVector CameraPosition, CameraRotation;

	CameraPosition = worldPointer->GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	CameraRotation = worldPointer->GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorForwardVector();
	FHitResult hHitResult;
	worldPointer->GetWorld()->LineTraceSingleByChannel(hHitResult,CameraPosition,CameraPosition+CameraRotation*100000, ECC_Visibility);
	FVector hitPosition = hHitResult.ImpactPoint;
	FRotator c_rotator = worldPointer->GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorRotation();
	CameraRotation = FVector(c_rotator.Roll,c_rotator.Pitch,c_rotator.Yaw);
	UE_LOG(LogTemp, Warning, TEXT("CameraRotation: %s"),*(CameraRotation.ToString()))
	for (TArray<AActor*>::TConstIterator iter = T_output_array.CreateConstIterator(); iter; iter++)
	{
		TArray<UStaticMeshComponent*> Components;
		(*iter)->GetComponents<UStaticMeshComponent>(Components);
		std::vector<FVector> Per_vertex_index;
		
		for (int32 i = 0; i < Components.Num(); i++)
		{
			UStaticMeshComponent* StaticMeshComponent = Components[i];
			UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
			if (StaticMesh)
			{
				FPositionVertexBuffer* VertexBuffer = &(StaticMesh->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer);

				if (i == 0)
				{
					meshFactory.Init(TCHAR_TO_UTF8(*targetFolder), TCHAR_TO_UTF8(*((*iter)->GetFName().ToString())), (*iter)->GetActorLocation(), TCHAR_TO_UTF8(*optixRendererPath),
						StaticMesh->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer.GetNumVertices(), StaticMesh->RenderData->LODResources[0].GetNumTriangles());
				}
				if (VertexBuffer)
				{
					const int32 VertexCount = VertexBuffer->GetNumVertices();
					TArray<FColor> VertexColors;
					StaticMesh->RenderData->LODResources[0].VertexBuffers.ColorVertexBuffer.Init(VertexCount);
					StaticMesh->RenderData->LODResources[0].VertexBuffers.ColorVertexBuffer.GetVertexColors(VertexColors);

					for (int32 Index = 0; Index < VertexCount; Index++)
					{
						const FVector WorldSpaceVertexLocation = /*(*iter)->GetActorLocation() + */(*iter)->GetTransform().TransformVector(VertexBuffer->VertexPosition(Index));

						const FVector4 VertexNormal = StaticMesh->RenderData->LODResources[0].VertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(static_cast<uint32>(Index));

						meshFactory.serializeVertexData(WorldSpaceVertexLocation, FVector(VertexColors[Index].R, VertexColors[Index].G, VertexColors[Index].B));
						Per_vertex_index.push_back(WorldSpaceVertexLocation);
					}
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Mesh name : %s %d %d %d"), *((*iter)->GetFName().ToString()), VertexColors.Num(), VertexCount, StaticMesh->RenderData->LODResources[0].VertexBuffers.ColorVertexBuffer.GetNumVertices()));
					TArray<uint32> Indices;  // << Result saved here
					const FRawStaticIndexBuffer& IndexBuffer = StaticMesh->RenderData->LODResources[0].IndexBuffer;
					IndexBuffer.GetCopy(Indices);
					//calculate the triangle vertex postion
					int NumIndices = Indices.Num();
					for (int i = 0; i < NumIndices; i += 3) {
						uint32 i0 = Indices[i + 0];
						uint32 i1 = Indices[i + 1];
						uint32 i2 = Indices[i + 2];

						const FVector& v0 = Per_vertex_index[i0];
						const FVector& v1 = Per_vertex_index[i1];
						const FVector& v2 = Per_vertex_index[i2];

						meshFactory.serializeTriangleData(FVector(i0, i1, i2));
					}
					UE_LOG(LogTemp, Warning, TEXT("NumIndices: %d NumTrangle:%d "), NumIndices, StaticMesh->RenderData->LODResources[0].GetNumTriangles());


				}
				Vertex_output.push_back(Per_vertex_index);
				meshFactory.serializeFactoryData();
			}
		}
	}
	meshFactory.setPlayerCameraPosition(CameraPosition,CameraRotation,hitPosition);
	meshFactory.makeSceneFile();
	std::vector<FString>start_params = meshFactory.startRenderParams();
	UE_LOG(LogTemp, Warning, TEXT("Render Path: %s"), TCHAR_TO_UTF8(*start_params[1]));
	FPlatformProcess::CreateProc(*(start_params[0]),NULL, true, false, false, nullptr, 1, nullptr, nullptr);
	return T_output_array;
}

TArray<AActor*> UTestPluginBPLibrary::GetAnimatedMeshVertex(UObject * worldPointer, FString targetFolder)
{
	TArray<AActor*> T_output_array;
	std::vector<std::vector<FVector>> Vertex_output;
	UGameplayStatics::GetAllActorsWithTag(worldPointer, "AnimationActor", T_output_array);
	MeshFactory meshFactory;
	UE_LOG(LogTemp, Warning, TEXT("Num skeletal mesh find %d "),T_output_array.Num());
	for (TArray<AActor*>::TConstIterator iter = T_output_array.CreateConstIterator(); iter; iter++)
	{
		TArray<USkeletalMeshComponent*> Components;
		(*iter)->GetComponents<USkeletalMeshComponent>(Components);
		std::vector<FVector> Per_vertex_index;
		
		for (int32 i = 0; i < Components.Num(); i++)
		{
			TArray<FBoneVertInfo> fBoneInfo;
			USkeletalMeshComponent* SkeletalMeshComponent = Components[i];
			USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->SkeletalMesh;
			if (SkeletalMesh)
			{
				FPositionVertexBuffer* SkeletalMeshVertexBuffer = &(SkeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[0].StaticVertexBuffers.PositionVertexBuffer);

				if (i == 0)
				{
					meshFactory.Init(TCHAR_TO_UTF8(*targetFolder), TCHAR_TO_UTF8(*((*iter)->GetFName().ToString())), (*iter)->GetActorLocation(), TCHAR_TO_UTF8(*FString("")),
						SkeletalMeshVertexBuffer->GetNumVertices(), SkeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[0].GetTotalFaces(), MESH_TYPE::MESH_PLY, true);
				}

				if (SkeletalMeshVertexBuffer)
				{
					const int32 VertexCount = SkeletalMeshVertexBuffer->GetNumVertices();

					for (int32 Index = 0; Index < VertexCount; Index++)
					{
						const FVector WorldSpaceVertexLocation = /*(*iter)->GetActorLocation() + */(*iter)->GetTransform().TransformVector(SkeletalMeshVertexBuffer->VertexPosition(Index));

						meshFactory.serializeVertexData(WorldSpaceVertexLocation, FVector(0, 0, 0));
						Per_vertex_index.push_back(WorldSpaceVertexLocation);
					}
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Skeletal mesh name : %s %d"), *((*iter)->GetFName().ToString()), SkeletalMeshVertexBuffer->GetNumVertices()));
					TArray<uint32> Indices;

					SkeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[0].MultiSizeIndexContainer.GetIndexBuffer(Indices);
					//calculate the triangle vertex postion
					int NumIndices = Indices.Num();
					for (int i = 0; i < NumIndices; i += 3) {
						uint32 i0 = Indices[i + 0];
						uint32 i1 = Indices[i + 1];
						uint32 i2 = Indices[i + 2];

						const FVector& v0 = Per_vertex_index[i0];
						const FVector& v1 = Per_vertex_index[i1];
						const FVector& v2 = Per_vertex_index[i2];

						meshFactory.serializeTriangleData(FVector(i0, i1, i2));
					}
					UE_LOG(LogTemp, Warning, TEXT("NumIndices: %d NumTrangle:%d "), NumIndices, SkeletalMeshComponent->GetSkeletalMeshRenderData()->LODRenderData[0].GetTotalFaces());


				}
				Vertex_output.push_back(Per_vertex_index);
				meshFactory.serializeFactoryData();
			}
		}
	}
	meshFactory.makeSceneFile();
		
	return T_output_array;
}

/*Abandoned*/
bool UTestPluginBPLibrary::UpadateCacheData(UObject * selfPointer,FString TargetFolder)
{
	if (cacheController == NULL)
	{
		cacheController = new CacheDataInterface(selfPointer, TargetFolder);
	}
	cacheController->writeCurrentData(selfPointer,NULL);

	return true;
}

TArray<AActor*> UTestPluginBPLibrary::ReleaseCacheData(UObject* selfPointer)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(selfPointer, ACarCameraActor::StaticClass(), Actors);
	for (int i = 0; i < Actors.Num(); i++)
	{
		((ACarCameraActor*)Actors[i])->releaseActorCameraCache();
	}
	return Actors;
}



