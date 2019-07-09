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

struct camera_sm_info {
	char data_block_name[100];
	int info_length;
};

struct animated_actor_info {
	char data_block_name[100];
	int info_length;
};

struct animated_actor_structure
{
	float rotation_x;
	float rotation_y;
	float rotation_z;

	float position_x;
	float position_y;
	float position_z;
};

class TESTPLUGIN_API CacheDataInterface
{
private:
	UObject* worldPointer;
	FString targetFile;
	WindowsSharedMemory sharedMemaryManager;
	bool hasRequestSM = false;
	bool hasRequestSM1 = false;
	bool hasRequestSM2 = false;
	void* SMaddress;
	void* SMaddress_1;
	void* SMaddress_2;
	std::vector<void*> SMaddress_3;
	TArray<AActor*> Actors;
	
public:
	CacheDataInterface() {};
	CacheDataInterface(UObject* in_worldPointer, FString in_targetFolder);
	void writeCurrentData(UObject* in_worldPointer);
	void writeCurrentCameraCache(pixel_structure* Data, int num_pixels, int current_num, int num_camera = 0, std::string in_SM = "Camera");
	void writeCurrentAnimatedActorCache(animated_actor_structure* Data, int current_num, int num_actors, std::string in_SM = "AActor");
	void releaseCurrentCameraCache();
	~CacheDataInterface();
};


