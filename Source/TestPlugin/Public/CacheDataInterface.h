// THIS FILE IS PART OF UE4 LIDAR SIMULATE AND AUTO DRIVE SIMULATE PROJECT 
// THIS PROGRAM IS FREE SOFTWARE, IS LICENSED UNDER MIT
// Copyright(c) Bowei Zhang

#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
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
	uint8 B;
	uint8 G;
	uint8 R;
	uint8 A;
};

struct camera_sm_info {
	char data_block_name[100];
	int info_length;
	int width;
	int height;
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

struct car_info
{
	float speed;
	float car_position_x;
	float car_position_y;
	float car_position_z;
};

struct AlgInformation
{
	float steeringMechanism_TuringRate;
	float brakingMechanism_BrakingRate;
	float engineMechanism_ThrottleRate;
	char message[200];
};



class ALidarActor;
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
	void* SMCarInfoLenAddress;
	void* SMCarInfoDataAddress;//Used twice watch out
	void* SMAlgContainerAddress;
	void* SMGTInfoAddress;
	char transferInfo[10000];
	std::vector<void*> SMaddress_3;
	TArray<AActor*> Actors;
	
public:
	CacheDataInterface() {};
	CacheDataInterface(UObject* in_worldPointer, FString in_targetFolder);
	void writeCurrentData(UObject* in_worldPointer, AActor* in_LidarActor);
	void writeCurrentCameraCache(pixel_structure* Data, int num_pixels, int current_num, int pic_width, int pic_height, int num_camera = 0, std::string in_SM = "Camera");
	void writeCurrentAnimatedActorCache(animated_actor_structure* Data, int current_num, int num_actors, std::string in_SM = "AActor");
	void writeCurrentCarInformation(car_info* Data);
	void writeCurrentCarBoundingBoxInfo(TArray<FVector2D>& GT_Results_TopLeft, TArray<FVector2D>& GT_Results_WidthHeight, int current_num, int num_camera);
	AlgInformation* getAlgContainerInformation();
	void releaseCurrentCameraCache();
	~CacheDataInterface();
};


