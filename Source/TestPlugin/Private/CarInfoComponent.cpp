// Fill out your copyright notice in the Description page of Project Settings.

#include "CarInfoComponent.h"

car_info carInfo;

AlgInformation algInforamtion;

// Sets default values for this component's properties
UCarInfoComponent::UCarInfoComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCarInfoComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCarInfoComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	this->CacheController.writeCurrentCarInformation(&carInfo);
	AlgInformation* buffer = this->CacheController.getAlgContainerInformation();
	algInforamtion = *(buffer);
	delete buffer;

	// ...
}

void UCarInfoComponent::getCarInformation(float in_speed, FVector in_carposition)
{
	carInfo.speed = in_speed;
	carInfo.car_position_x = in_carposition.X;
	carInfo.car_position_y = in_carposition.Y;
	carInfo.car_position_z = in_carposition.Z;
}

FCarMechanismControlOrder UCarInfoComponent::returnCarMechanismControlOrder()
{
	FCarMechanismControlOrder return_result;
	return_result.steeringMechanism_TuringRate = algInforamtion.steeringMechanism_TuringRate;
	return_result.brakingMechanism_BrakingRate = algInforamtion.brakingMechanism_BrakingRate;
	return_result.engineMechanism_ThrottleRate = algInforamtion.engineMechanism_ThrottleRate;

	return return_result;
}

