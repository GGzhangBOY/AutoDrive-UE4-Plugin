// Fill out your copyright notice in the Description page of Project Settings.

// Fill out your copyright notice in the Description page of Project Settings.

#include "MeshFactory.h"


MeshFactory::MeshFactory()
{
}

void MeshFactory::Init(std::string targetFolder, std::string currentMeshName, std::string optix_renderer_path, int vertexNum, int triangleNum, MESH_TYPE makeType, bool isSkeletal)
{
	std::string file_name;
	std::string scene_file_name;
	this->makeType = makeType;
	this->currentMeshName = currentMeshName;
	this->sceneSMNames.push_back(currentMeshName);
	this->isSkeletal = isSkeletal;
	this->vertexNum = vertexNum;
	this->vertexTriganleNum = triangleNum;
	this->optix_renderer_path = optix_renderer_path;
	this->vertexPosRGBHolder.clear();
	this->vertexTriangleInfoHolder.clear();
	this->current_mesh.close();
	if (makeType == MESH_TYPE::MESH_PLY)
		file_name = targetFolder + "\\" + currentMeshName + ".ply";
	else
		file_name = targetFolder + "\\" + currentMeshName + ".obj";
	if(!isSkeletal)
	scene_file_name = targetFolder + "\\" + "exportscene" + ".scene";
	else
	scene_file_name = targetFolder + "\\" + "exportskeletal" + ".scene";

	this->current_mesh = std::ofstream(file_name.c_str(), std::ios::ate);
	this->scene_file = std::ofstream(scene_file_name.c_str(), std::ios::ate);

	makeHeader();
}

void MeshFactory::makeHeader()
{
	char str_holder[600];
	
	if (this->makeType == MESH_TYPE::MESH_PLY)
	{
		sprintf(str_holder, "ply\nformat ascii 1.0\ncomment author : MeshFactory1.0\ncomment object : %s\nelement vertex %d\nproperty float x\nproperty float y\nproperty float z\nproperty uchar red\nproperty uchar green\nproperty uchar blue\nelement face %d\nproperty list uchar int vertex_indices\nend_header\n",
			this->currentMeshName.c_str(),
			this->vertexNum,
			this->vertexTriganleNum);
	}
	else
	{
		sprintf(str_holder,"Sorry but the MeshFactory Plugin can only support .ply right now, convert to .ply if possible");
	}
	std::string outputString(str_holder);
	this->current_mesh << outputString;
}

void MeshFactory::serializeVertexData(FVector position, FVector color)
{
	std::string toPut;
	char str_holder[600];
	if (this->makeType == MESH_TYPE::MESH_PLY)
	{

		sprintf(str_holder, "%f %f %f %d %d %d\n", position.X, -position.Y, position.Z, int(color.X), int(color.Y), int(color.Z));
		toPut = std::string(str_holder);
		this->vertexPosRGBHolder.push_back(toPut);
	}
	else//The .obj TODO
		;

}

void MeshFactory::serializeTriangleData(FVector vertexOrder)
{
	std::string toPut;
	char str_holder[600];
	if (this->makeType == MESH_TYPE::MESH_PLY)
	{

		sprintf(str_holder, "3 %d %d %d\n", int(vertexOrder.X), int(vertexOrder.Y), int(vertexOrder.Z));
		toPut = std::string(str_holder);

		this->vertexTriangleInfoHolder.push_back(toPut);
	}
	else//The .obj TODO
		;
}

void MeshFactory::serializeFactoryData()
{
	if (this->makeType == MESH_TYPE::MESH_PLY)
	{
		for (int i = 0; i < this->vertexPosRGBHolder.size(); i++)
			this->current_mesh << vertexPosRGBHolder[i];
		for (int i = 0; i < this->vertexTriangleInfoHolder.size(); i++)
			this->current_mesh << vertexTriangleInfoHolder[i];
	}
}

void MeshFactory::setPlayerCameraPosition(FVector cameraPosition, FVector cameraRotation, FVector cameraLookAt)
{
	this->camera_Position = cameraPosition;
	this->camera_Rotation = cameraRotation;
	this->camera_LookAt = cameraLookAt;
}

void MeshFactory::makeSceneFile()
{
	this->scene_file<< "material default\n{\ncolor 0.350827 0.242986 0.17883\n}\n";
	if (this->makeType == MESH_TYPE::MESH_PLY)
	{
		std::string current_name;
		for (int i = 0; i < this->sceneSMNames.size(); i++)
		{
			current_name = sceneSMNames[i] + ".ply";
			this->scene_file << "mesh\n{\n" << "file " << current_name << "\nmaterial default\n}\n";
		}
	}
	//test light add to the scene
	if (!this->isSkeletal)
	{
		this->scene_file << "light\n{\ntype Sphere\nposition 0.320673 0.027337 0.228975\nv1  0.320673 0.476 0.228975\nv2  0.320673 0.027337 - 0.1375\nemission 4.0 4.0 4.0\n}\n";
		this->scene_file << "light\n{\ntype Sphere\nposition 0.230128 0.50385 0.267372\nv1 - 0.230128 0.50385 0.267372\nv2 0.230128 0.50385 - 0.1927\nemission 4.0 4.0 4.0\n}\n";

		//test start camera add to the scene
		std::stringstream sstream,sstream1,sstream2;
		sstream << "position " << this->camera_Position.X << " " << -this->camera_Position.Y << " " << this->camera_Position.Z;
		sstream1 << "rotation " << this->camera_Rotation.X << " " << this->camera_Rotation.Y << " " << this->camera_Rotation.Z;//roll picth raw
		sstream2 << "lookat " << this->camera_LookAt.X << " " << -this->camera_LookAt.Y << " " << this->camera_LookAt.Z;
		this->scene_file << "CameraPosition\n{\n"<<sstream.str()<<"\n"<<sstream1.str()<<"\n"<<sstream2.str()<<"\n}\n";

	}
	this->scene_file.close();
}

std::vector<FString> MeshFactory::startRenderParams()
{
	std::vector<FString> out_params;
	FString scene_path;

	if(!isSkeletal)
		scene_path = FString(this->targetFolder.c_str()) + "\\" + "exportscene" + ".scene";
	else
		scene_path = FString(this->targetFolder.c_str()) + "\\" + "exportskeletal" + ".scene";

	FString randerer_path = FString(this->optix_renderer_path.c_str());
	FString command_line = FString(" -scene " + scene_path);
	
	out_params.push_back(randerer_path);
	out_params.push_back(command_line);

	return out_params;
}


MeshFactory::~MeshFactory()
{
}

