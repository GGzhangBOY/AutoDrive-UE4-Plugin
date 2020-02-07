// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <iostream>
#include <vector>
#include "CoreMinimal.h"

/**
 * 
 */
class TESTPLUGIN_API WindowsSharedMemory
{
public:

	WindowsSharedMemory();
	~WindowsSharedMemory();

	bool CreateMappedMemory(const FString& sharedMemoryName, size_t shareMemorySize);
	void* GetMappedMemoryData();
	bool ReadExistingMappedMemory(const FString& sharedMemoryName, size_t shareMemorySize);
	void* GetExistingMappedMemoryData();
	void CloseSharedMemory();

private:
	std::vector<void*> handlerCollection;
	std::vector<void*> mapedFileCollection;
	size_t SMsize;
	void* SharedMemoryHandle;
	void* SharedMemoryData; 
};
