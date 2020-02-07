// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TestPluginBPLibrary.h"
#include "CacheDataInterface.h"
#include "CarInfoComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTPLUGIN_API UCarInfoComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UCarInfoComponent();
	UPROPERTY(EditAnywhere)
		FString CarInfoVal1 = "";
	UPROPERTY(EditAnywhere)
		FString CarInfoVal2 = "";
	UPROPERTY(EditAnywhere)
		FString CarInfoVal3 = "";

private:
	CacheDataInterface CacheController;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Car Info", Keywords = " sample test testing"), Category = "Car Info Component")
		static void getCarInformation(float in_speed, FVector in_carposition);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Return Car Mechanism Control Order", Keywords = " sample test testing"), Category = "Car Info Component")
		static FCarMechanismControlOrder returnCarMechanismControlOrder();
};
