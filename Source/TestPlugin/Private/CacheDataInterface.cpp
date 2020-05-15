// THIS FILE IS PART OF UE4 LIDAR SIMULATE AND AUTO DRIVE SIMULATE PROJECT 
// THIS PROGRAM IS FREE SOFTWARE, IS LICENSED UNDER MIT
// Copyright(c) Bowei Zhang

#include "CacheDataInterface.h"

CacheDataInterface::CacheDataInterface(UObject* in_worldPointer, FString in_targetFolder)
{
	this->targetFile = in_targetFolder+FString("/Cache.dat");
	this->worldPointer = in_worldPointer;
}

void CacheDataInterface::writeCurrentData(UObject* in_worldPointer, AActor* in_LidarActor)
{
	FVector CameraPosition, CameraRotation;
	CameraPosition = in_LidarActor->GetActorLocation();
	CameraRotation = in_LidarActor->GetActorForwardVector();

	FHitResult hHitResult;
	in_worldPointer->GetWorld()->LineTraceSingleByChannel(hHitResult, CameraPosition, CameraPosition + CameraRotation * 100000, ECC_Visibility);
	FVector hitPosition = hHitResult.ImpactPoint;
	FRotator c_rotator = in_worldPointer->GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorRotation();
	CameraRotation = FVector(c_rotator.Roll, c_rotator.Pitch, c_rotator.Yaw);

	std::stringstream sstream, sstream1, sstream2, sstream3;
	sstream << "position " << CameraPosition.X << " " << CameraPosition.Y << " " << CameraPosition.Z;
	sstream1 << "rotation " << CameraRotation.X << " " << CameraRotation.Y << " " << CameraRotation.Z;
	sstream2 << "lookat " << hitPosition.X << " " << hitPosition.Y << " " << hitPosition.Z;
	sstream3 << "CameraPosition\n{\n" << sstream.str() << "\n" << sstream1.str() << "\n" << sstream2.str() << "\n}\n";


	UGameplayStatics::GetAllActorsWithTag(GWorld->GetWorld(), "AnimationActor", Actors);

	std::stringstream sstream4;
	
	for (TArray<AActor*>::TConstIterator iter = Actors.CreateConstIterator(); iter; iter++)
	{
		sstream4 << "\nAnimationActors\n{\n";
		USkeletalMeshComponent* currentSketelMesh = (USkeletalMeshComponent*)(*iter)->GetComponentByClass(USkeletalMeshComponent::StaticClass());
		FVector anima_actor_location = currentSketelMesh->GetComponentLocation();
		FRotator anima_actor_rotation = currentSketelMesh->GetComponentRotation();

		sstream4 << TCHAR_TO_UTF8(*((*iter)->GetFName().ToString())) << "\n position " << anima_actor_location.X << " " << anima_actor_location.Y << " " << anima_actor_location.Z << " "\
			<<"\n rotation "<< anima_actor_rotation.Roll<<" "<< anima_actor_rotation.Pitch<<" "<< anima_actor_rotation.Yaw<<"\n";
		sstream4 << "}/n";
	}
	sstream3 << sstream4.str();
	int length = sstream3.str().length();
	//UE_LOG(LogTemp, Warning, TEXT("Text length %d"),length)
	if (!this->hasRequestSM)
	{
		while (true)
		{
			sharedMemaryManager.CreateMappedMemory("AnimatedActorsInfoLength", sizeof(int));
			this->SMaddress = sharedMemaryManager.GetMappedMemoryData();
			sharedMemaryManager.CreateMappedMemory("AnimatedActorsInfo", length * sizeof(char));
			this->SMaddress_1 = sharedMemaryManager.GetMappedMemoryData();

			if ((this->SMaddress != nullptr)&&(this->SMaddress_1 != nullptr))
			{
				this->hasRequestSM = true;
				break;
			}
		}
	}
	memcpy(SMaddress, &length, sizeof(int));
	memcpy(SMaddress_1, sstream3.str().c_str(), length * sizeof(char));
	
	//SM send directly to the ArgContainer
	std::stringstream sstream5;
	sstream5 << CameraPosition.X << "\n" << CameraPosition.Y << "\n" << CameraPosition.Z;
	sharedMemaryManager.CreateMappedMemory("CarCameraInfoLength", sizeof(int));
	this->SMCarInfoLenAddress = sharedMemaryManager.GetMappedMemoryData();
	int carInfoLen = sstream5.str().length();
	sharedMemaryManager.CreateMappedMemory("CarCameraInfo", carInfoLen * sizeof(char));
	this->SMCarInfoDataAddress = sharedMemaryManager.GetMappedMemoryData();

	memcpy(SMCarInfoLenAddress, &carInfoLen, sizeof(int));
	memcpy(SMCarInfoDataAddress, sstream5.str().c_str(), carInfoLen * sizeof(char));
	
	//discarded way, using file to transfer data
	/*std::ofstream cacheFile(*this->targetFile, std::ios::trunc);
	if (cacheFile.is_open())
	{
		cacheFile << sstream3.str();
		//cacheFile << sstream4.str();
		cacheFile.close();
	}*/
}

void CacheDataInterface::writeCurrentCameraCache(pixel_structure* Data, int num_pixels, int pic_width, int pic_height, int current_num, int num_camera, std::string in_SM)
{
	
	if (this->hasRequestSM2 == false)
	{
		camera_sm_info* info_list = new camera_sm_info[num_camera];
		for (int i = 0; i < num_camera; i++)
		{
			std::stringstream ss_buf;
			ss_buf << in_SM << i;
			strcpy(info_list[i].data_block_name, ss_buf.str().c_str());
			info_list[i].info_length = num_camera;
			info_list[i].width = pic_width;
			info_list[i].height = pic_height;
		}
		while (true)
		{
			sharedMemaryManager.CreateMappedMemory("CameraInfo", sizeof(camera_sm_info)*num_camera);
			this->SMaddress_2 = this->sharedMemaryManager.GetMappedMemoryData();
			memcpy(this->SMaddress_2, info_list, sizeof(camera_sm_info)*num_camera);
			for (int i = 0; i < num_camera; i++)
			{
				sharedMemaryManager.CreateMappedMemory(info_list[i].data_block_name, num_pixels * sizeof(pixel_structure));
				if(this->sharedMemaryManager.GetMappedMemoryData()!=nullptr)
					SMaddress_3.push_back(this->sharedMemaryManager.GetMappedMemoryData());
			}
			if ((this->SMaddress_2 != nullptr) && (this->SMaddress_3.size() == num_camera))
			{
				this->hasRequestSM2 = true;
				break;
			}
			else
				sharedMemaryManager.CloseSharedMemory();
		}
		delete[] info_list;
	}
	

	memcpy(this->SMaddress_3[current_num], Data, num_pixels * sizeof(pixel_structure));

	delete[] Data;

}

void CacheDataInterface::writeCurrentAnimatedActorCache(animated_actor_structure* Data, int current_num, int num_actors, std::string in_SM)
{
	if (this->hasRequestSM1 == false)
	{
		animated_actor_info* info_list = new animated_actor_info[num_actors];
		for (int i = 0; i < num_actors; i++)
		{
			std::stringstream ss_buf;
			ss_buf << in_SM << i;
			strcpy(info_list[i].data_block_name, ss_buf.str().c_str());
			info_list[i].info_length = num_actors;
		}
		while (true)
		{
			sharedMemaryManager.CreateMappedMemory("AnimatedActorInfo", sizeof(animated_actor_info)*num_actors);
			this->SMaddress_2 = this->sharedMemaryManager.GetMappedMemoryData();
			memcpy(this->SMaddress_2, &info_list, sizeof(animated_actor_info)*num_actors);
			for (int i = 0; i < num_actors; i++)
			{
				sharedMemaryManager.CreateMappedMemory(info_list[i].data_block_name, sizeof(animated_actor_info));
				if (this->sharedMemaryManager.GetMappedMemoryData() != nullptr)
					SMaddress_3.push_back(this->sharedMemaryManager.GetMappedMemoryData());
			}
			if ((this->SMaddress_2 != nullptr) && (this->SMaddress_3.size() == num_actors))
			{
				this->hasRequestSM1 = true;
				break;
			}
			else
				sharedMemaryManager.CloseSharedMemory();
		}
		delete[] info_list;
	}


	//memcpy(this->SMaddress_3[current_num], Data, sizeof(pixel_structure));

	delete Data;
}

void CacheDataInterface::writeCurrentCarInformation(car_info* Data)
{
	sharedMemaryManager.CreateMappedMemory("UE4CarInfo", sizeof(car_info));
	this->SMCarInfoDataAddress = this->sharedMemaryManager.GetMappedMemoryData();
	memcpy(this->SMCarInfoDataAddress, Data, sizeof(car_info));
}

void CacheDataInterface::writeCurrentCarBoundingBoxInfo(TArray<FVector2D>& GT_Results_TopLeft, TArray<FVector2D>& GT_Results_WidthHeight, int current_num, int num_camera)
{
	std::stringstream sstream;
	sstream << "Camera" << current_num;
	for (int i = 0; i < GT_Results_TopLeft.Num(); i++)
	{
		sstream << " " << GT_Results_TopLeft[i].X << "," << GT_Results_TopLeft[i].Y << ";" << GT_Results_WidthHeight[i].X << "," << GT_Results_WidthHeight[i].Y;
	}
	sstream << "\n";
	strcat(this->transferInfo, sstream.str().c_str());

	if (current_num+1 == num_camera)
	{
		sharedMemaryManager.CreateMappedMemory("GT_Info", sizeof(char)*10000);
		this->SMGTInfoAddress = this->sharedMemaryManager.GetMappedMemoryData();
		memcpy(this->SMGTInfoAddress, this->transferInfo, sizeof(char)*10000);
		strcpy(this->transferInfo, "");
	}
}

AlgInformation* CacheDataInterface::getAlgContainerInformation()
{
	AlgInformation* returnInformation = new AlgInformation();
	sharedMemaryManager.ReadExistingMappedMemory("AlgContainerOrder", sizeof(AlgInformation));
	this->SMAlgContainerAddress = this->sharedMemaryManager.GetExistingMappedMemoryData();
	if (this->SMAlgContainerAddress)
	{
		AlgInformation* pBuf = (AlgInformation*)this->SMAlgContainerAddress;
		memcpy(returnInformation, pBuf, sizeof(AlgInformation));
		return returnInformation;
	}
	return returnInformation;
}

void CacheDataInterface::releaseCurrentCameraCache()
{
	this->sharedMemaryManager.CloseSharedMemory();
}

CacheDataInterface::~CacheDataInterface()
{
	this->sharedMemaryManager.CloseSharedMemory();
}
