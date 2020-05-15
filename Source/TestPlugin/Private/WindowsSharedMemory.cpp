// THIS FILE IS PART OF UE4 LIDAR SIMULATE AND AUTO DRIVE SIMULATE PROJECT 
// THIS PROGRAM IS FREE SOFTWARE, IS LICENSED UNDER MIT
// Copyright(c) Bowei Zhang

#include "WindowsSharedMemory.h"
#include <windows.h>

WindowsSharedMemory::WindowsSharedMemory()
{
}

WindowsSharedMemory::~WindowsSharedMemory()
{

}

bool WindowsSharedMemory::CreateMappedMemory(const FString& sharedMemoryName, size_t shareMemorySize)
{
	this->SMsize = shareMemorySize;
	//TODO
	SharedMemoryHandle = CreateFileMapping(
		INVALID_HANDLE_VALUE, // File Mapping Security and Access Rights
		NULL,
		PAGE_READWRITE,
		0,
		shareMemorySize,
		*sharedMemoryName);

	if (!SharedMemoryHandle)
	{
		UE_LOG(LogClass, Log, TEXT("Memory-mapped file does not exist."));
		return false;
	}
	this->handlerCollection.push_back(SharedMemoryHandle);
	return true;
}

void* WindowsSharedMemory::GetMappedMemoryData()
{
	SharedMemoryData = (void*)MapViewOfFile(SharedMemoryHandle,
		FILE_MAP_ALL_ACCESS,
		0, 0,
		0);

	if (!SharedMemoryData)
	{
		UE_LOG(LogClass, Log, TEXT("Memory-mapped data does not exist."));
		return NULL;
	}
	this->mapedFileCollection.push_back(SharedMemoryData);
	return SharedMemoryData;
}

bool WindowsSharedMemory::ReadExistingMappedMemory(const FString & sharedMemoryName, size_t shareMemorySize)
{
	this->SMsize = shareMemorySize;
	//TODO
	SharedMemoryHandle = OpenFileMapping(
		FILE_MAP_READ, // File Mapping Security and Access Rights
		0,
		*sharedMemoryName);

	if (!SharedMemoryHandle)
	{
		UE_LOG(LogClass, Log, TEXT("Existing Memory-mapped file does not exist."));
		return false;
	}
	this->handlerCollection.push_back(SharedMemoryHandle);
	return true;
}

void * WindowsSharedMemory::GetExistingMappedMemoryData()
{
	SharedMemoryData = (void*)MapViewOfFile(SharedMemoryHandle,
		FILE_MAP_READ,
		0, 0,
		0);

	if (!SharedMemoryData)
	{
		UE_LOG(LogClass, Log, TEXT("Memory-mapped data does not exist."));
		return NULL;
	}

	this->mapedFileCollection.push_back(SharedMemoryData);
	return SharedMemoryData;
}



void WindowsSharedMemory::CloseSharedMemory()
{
	for (int i = 0; i < this->mapedFileCollection.size(); i++)
	{
		if (SharedMemoryData != nullptr)
		{
			UnmapViewOfFile(mapedFileCollection[i]);
			SharedMemoryData = nullptr;
		}
	}
	for (int i = 0; i < this->handlerCollection.size(); i++)
	{
		if (SharedMemoryHandle != nullptr)
		{
			CloseHandle(handlerCollection[i]);
			SharedMemoryHandle = nullptr;
		}
	}
}