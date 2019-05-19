// Fill out your copyright notice in the Description page of Project Settings.

#include "CacheDataInterface.h"

CacheDataInterface::CacheDataInterface(UObject* in_worldPointer, FString in_targetFolder)
{
	this->targetFile = in_targetFolder+FString("/Cache.dat");
	this->worldPointer = in_worldPointer;
}

void CacheDataInterface::writeCurrentData(UObject* in_worldPointer)
{
	FVector CameraPosition, CameraRotation;
	
	CameraPosition = GWorld->GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorLocation();
	CameraRotation = GWorld->GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorForwardVector();

	FHitResult hHitResult;
	GWorld->GetWorld()->LineTraceSingleByChannel(hHitResult, CameraPosition, CameraPosition + CameraRotation * 100000, ECC_Visibility);
	FVector hitPosition = hHitResult.ImpactPoint;
	FRotator c_rotator = GWorld->GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorRotation();
	CameraRotation = FVector(c_rotator.Roll, c_rotator.Pitch, c_rotator.Yaw);

	std::stringstream sstream, sstream1, sstream2, sstream3;
	sstream << "position " << CameraPosition.X << " " << CameraPosition.Y << " " << CameraPosition.Z;
	sstream1 << "rotation " << CameraRotation.X << " " << CameraRotation.Y << " " << CameraRotation.Z;
	sstream2 << "lookat " << hitPosition.X << " " << hitPosition.Y << " " << hitPosition.Z;
	sstream3 << "CameraPosition\n{\n" << sstream.str() << "\n" << sstream1.str() << "\n" << sstream2.str() << "\n}\n";

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GWorld->GetWorld(), "Lidar Animation Actor", Actors);
	std::stringstream sstream4;
	sstream4 << "AnimationActors\n{\n";
	for (TArray<AActor*>::TConstIterator iter = Actors.CreateConstIterator(); iter; iter++)
	{
		FVector anima_actor_location = (*iter)->GetActorLocation();
		FRotator anima_actor_rotation = (*iter)->GetActorRotation();

		sstream4 << TCHAR_TO_UTF8(*((*iter)->GetFName().ToString())) << " position " << anima_actor_location.X << " " << anima_actor_location.Y << " " << anima_actor_location.Z << " "\
			<<"Rotation "<< anima_actor_rotation.Roll<<" "<< anima_actor_rotation.Pitch<<" "<< anima_actor_rotation.Yaw<<"\n";
	}
	sstream4 << "}/n";
	int length = sstream3.str().length();
	UE_LOG(LogTemp, Warning, TEXT("Text length %d"),length)
	if (this->hasRequestSM == false)
	{
		while (true)
		{
			sharedMemaryManager.CreateMappedMemory("AnimatedActorsInfoLength", sizeof(int));
			sharedMemaryManager.CreateMappedMemory("AnimatedActorsInfo", length * sizeof(char));
			this->SMaddress = sharedMemaryManager.GetMappedMemoryData();
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
	
	
	
	/*std::ofstream cacheFile(*this->targetFile, std::ios::trunc);
	if (cacheFile.is_open())
	{
		cacheFile << sstream3.str();
		//cacheFile << sstream4.str();
		cacheFile.close();
	}*/
}

void CacheDataInterface::writeCurrentCameraCache(pixel_structure * Data, int num_pixels)
{
	if (this->hasRequestSM1 == false)
	{
		while (true)
		{
			sharedMemaryManager.CreateMappedMemory("Camera1Pixels", sizeof(int));
			sharedMemaryManager.CreateMappedMemory("CarCameraCache", num_pixels * sizeof(pixel_structure));
			this->SMaddress_2 = this->sharedMemaryManager.GetMappedMemoryData();
			this->SMaddress_3 = this->sharedMemaryManager.GetMappedMemoryData();
			if ((this->SMaddress_2 != nullptr) && (this->SMaddress_3 != nullptr))
			{
				this->hasRequestSM1 = true;
				break;
			}
		}
	}
	memcpy(this->SMaddress_2, &num_pixels, sizeof(int));
	memcpy(this->SMaddress_3, Data, num_pixels * sizeof(pixel_structure));
	delete[] Data;
}

void CacheDataInterface::releaseCurrentCameraCache()
{
	this->sharedMemaryManager.CloseSharedMemory();
}

CacheDataInterface::~CacheDataInterface()
{
	this->sharedMemaryManager.CloseSharedMemory();
}
