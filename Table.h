#pragma once
#include <string>
#include <vtkSmartPointer.h>
#include <vtkPNGReader.h>
#include <vtkImageData.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkLight.h>
#include <set>

struct Limits {
	float minX;
	float minZ;
	float maxX;
	float maxZ;
};

class Table {
public:
	Table(const std::string& heightmapPath, const std::string& albedoPath);
	vtkActor* getActor();
	vtkLight* getLight();

	Limits getLimits();

private:
	vtkSmartPointer<vtkPNGReader> m_reader;
	vtkSmartPointer<vtkImageData> m_albedo;
	vtkSmartPointer<vtkImageData> m_heightmap;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkLight> m_light;

	void _preparePolyData();

	const float MIN = 0.0f;
	const float MAX = 0.1f;
};