#pragma once
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkVector.h>

#include "Ball.h"

class Predictor {
public:
	Predictor(float ballRadius);

	void setPrediction(vtkVector3f origin, vtkVector2f direction, int maxBounces = 3);
	void setLimits(const Limits& limits);

	vtkActor* getActor();
private:
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkPoints> m_points;
	vtkSmartPointer<vtkCellArray> m_lines;
	vtkSmartPointer<vtkPolyData> m_data;

	Limits m_limits;

	float m_ballRadius;
};