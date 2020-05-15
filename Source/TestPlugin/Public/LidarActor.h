// THIS FILE IS PART OF UE4 LIDAR SIMULATE AND AUTO DRIVE SIMULATE PROJECT 
// THIS PROGRAM IS FREE SOFTWARE, IS LICENSED UNDER MIT
// Copyright(c) Bowei Zhang


#pragma once

#include "CacheDataInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LidarActor.generated.h"

UCLASS()
class TESTPLUGIN_API ALidarActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALidarActor();

	UPROPERTY(EditAnywhere)
		FString randerPath="";
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Lidar", Keywords = " sample test testing"), Category = "LidarActor")
		static void LidarOn(UObject* worldPointer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	CacheDataInterface LidarCacheController;

	void UpadateLidarCacheData(UObject* worldPointer);
};
