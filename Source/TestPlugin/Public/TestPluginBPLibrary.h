// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include "MeshFactory.h"
#include "CacheDataInterface.h"
#include "Engine.h"
#include "Runtime/Engine/Public/ImageUtils.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Public/Rendering/SkeletalMeshRenderData.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Animation/SkeletalMeshActor.h"
#include "Developer/MeshUtilities/Public/MeshUtilities.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CarCameraActor.h"
#include "TestPluginBPLibrary.generated.h"



struct IPointCloudInfo
{
	FVector PointCloudPos;
	FVector PointCloudColor;
};

USTRUCT(BlueprintType)
struct FGroundTruthInfo
{
	GENERATED_BODY()

		// 用UPROPERTY包裹，可以更方便的在蓝图中使用
		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector2D TopLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector2D WidthHeight;

};


UENUM(BlueprintType)
enum class RootState :uint8 //设置uint8类型  
{
	E_ROOT_BOTTOM UMETA(DisplayName = "Root Bottom"),
	E_ROOT_MIDDLE UMETA(DisplayName = "Root Middle")
};

UENUM(BlueprintType)
enum class OutputColorState :uint8 //设置uint8类型  
{
	E_COLOR_GRAY UMETA(DisplayName = "Output Gray"),
	E_COLOR_RGBA UMETA(DisplayName = "Output RBGA")
};
UENUM(BlueprintType)
enum class ImageFormat : uint8
{
	/** Portable Network Graphics. */
	PNG = 0 UMETA(DisplayName = "Output PNG"),

	/** Joint Photographic Experts Group. */
	JPEG = 1 UMETA(DisplayName = "Output RGBA")
};


UCLASS()
class UTestPluginBPLibrary : public UBlueprintFunctionLibrary 
{
	GENERATED_UCLASS_BODY()

		static EImageFormat format;

		static int32 quality;

		static OutputColorState colorChannel;
		
		static int32 numPic;

		static FString filePath;

		static void* dllHandler;


		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Actor's GT Box", Keywords = " sample test testing"), Category = "TestPluginTesting")
		static TArray<FGroundTruthInfo> GetActorsGTBox(UObject* SelfPointer, FName TagName, RootState E_Rstate);
		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Make Dataset", Keywords = " sample test testing"), Category = "TestPluginTesting")
		static bool MakeDataSetFromGroundTruth(FString Folder, TArray<FGroundTruthInfo>GroundTruthInfo);
		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Adjust Dataset Parameter", Keywords = " sample test testing"), Category = "TestPluginTesting")
		static bool AdjustDatasetParameter(ImageFormat imageFormat, int32 imageQuality, OutputColorState imageColorChannel);
		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get mesh vertex test", Keywords = " sample test testing"), Category = "TestPluginTesting")
		static TArray<AActor*> GetStaticMeshVertex(UObject* selfPointer, FString targetFolder, FString optixRendererPath);
		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get skeleten mesh vertex test", Keywords = " sample test testing"), Category = "TestPluginTesting")
		static TArray<AActor*> GetSkeletonMeshVertex(UObject* selfPointer, FString targetFolder);
		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Update cache data", Keywords = " sample test testing"), Category = "TestPluginTesting")
		static bool UpadateCacheData(UObject* selfPointer, FString TargetFolder);
		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Release cache data", Keywords = " sample test testing"), Category = "TestPluginTesting")
		static TArray<AActor*> ReleaseCacheData(UObject* selfPointer);

		static void OnScreenshotCapturedInternal(int32 Width, int32 Height, const TArray<FColor>& Bitmap);
		static TArray<FGroundTruthInfo> CalculateBound(UObject* worldPointer, TArray<FVector> ActorPosition, TArray<FVector> Bound_Origin, RootState E_Rstate);
	
	
};




