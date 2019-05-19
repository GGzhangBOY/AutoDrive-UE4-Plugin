// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>
#include "Engine.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "WindowsSharedMemory.h"
#include "CoreMinimal.h"

/**
 * 
 */
struct pixel_structure
{
	UCHAR R;
	UCHAR G;
	UCHAR B;
	UCHAR A;
	int height;
	int width;
};
class TESTPLUGIN_API CacheDataInterface
{
private:
	UObject* worldPointer;
	FString targetFile;
	WindowsSharedMemory sharedMemaryManager;
	bool hasRequestSM = false;
	bool hasRequestSM1 = false;
	void* SMaddress;
	void* SMaddress_1;
	void* SMaddress_2;
	void* SMaddress_3;
	
public:
	CacheDataInterface() {};
	CacheDataInterface(UObject* in_worldPointer, FString in_targetFolder);
	void writeCurrentData(UObject* in_worldPointer);
	void writeCurrentCameraCache(pixel_structure* Data, int num_pixels);
	void releaseCurrentCameraCache();
	~CacheDataInterface();
};


