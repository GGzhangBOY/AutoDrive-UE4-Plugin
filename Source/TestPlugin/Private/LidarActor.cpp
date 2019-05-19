// Fill out your copyright notice in the Description page of Project Settings.

#include "LidarActor.h"

// Sets default values
ALidarActor::ALidarActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ALidarActor::LidarOn(UObject * worldPointer)
{
}

// Called when the game starts or when spawned
void ALidarActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALidarActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALidarActor::UpadateCacheData(UObject* worldPointer)
{
	if (this->randerPath == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("The rander data folder has't set please set before using the tick funtion!"))
			return;
	}
	if (cacheController == NULL)
	{
		cacheController = new CacheDataInterface(this,this->randerPath);
	}
	cacheController->writeCurrentData(worldPointer);
}

