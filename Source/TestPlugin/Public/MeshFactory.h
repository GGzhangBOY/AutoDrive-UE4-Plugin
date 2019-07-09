// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <string>
#include "Engine.h"
#include "CoreMinimal.h"

/**
 * 
 */
enum MESH_TYPE
{
	MESH_PLY = 0,
	MESH_OBJ = 1
};

struct Mesh_Info
{
	std::string mesh_Name;
	FVector mesh_position;
};


class TESTPLUGIN_API MeshFactory
{
private:
	std::ofstream current_mesh;

	std::ofstream scene_file;

	std::vector<std::string> vertexTriangleInfoHolder;

	std::vector<std::string> vertexPosRGBHolder;

	std::vector<Mesh_Info> sceneSMNames;

	std::string targetFolder;

	std::string optix_renderer_path;

	FVector camera_Position;

	FVector camera_Rotation;

	FVector camera_LookAt;

	bool isAnimated;

	int vertexNum;

	int vertexTriganleNum;

	MESH_TYPE makeType;

	std::string currentMeshName;

public:
	MeshFactory();

	void Init(std::string targetFolder, std::string currentMeshName, FVector mesh_position, std::string optix_renderer_path, int vertexNum, int triangleNum, MESH_TYPE makeType = MESH_TYPE::MESH_PLY, bool isAnimated = false);

	void makeHeader();

	void serializeVertexData(FVector position, FVector color);

	void serializeTriangleData(FVector vertexOrder);

	void serializeFactoryData();

	void setPlayerCameraPosition(FVector cameraPosition, FVector cameraRotation, FVector cameraLookAt);

	void makeSceneFile();

	std::vector<FString> startRenderParams();

	~MeshFactory();
};
