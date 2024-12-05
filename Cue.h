#pragma once
#include "Ball.h"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkCylinderSource.h>
#include <vtkProperty.h>
#include <vtkTransformFilter.h>
#include <vtkTransform.h>
#include <vtkVector.h>

#include "Predictor.h"

class Cue
{
public:
	Cue();
	~Cue();

	void shoot();
	void update(float delta);
	void rotate(float angle);

	float getRotation();

	vtkActor* getActor();

	void setBall(Ball* ball);
	void setPredictor(Predictor* predictor);
private:
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkTransformFilter> m_filter;
	vtkSmartPointer<vtkTransformFilter> m_finalFilter;
	vtkSmartPointer<vtkTransform> m_transform; 
	vtkSmartPointer<vtkTransform> m_finalTransform;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkCylinderSource> m_cylinder;

	vtkVector2f m_direction;

	Predictor* p_predictor;
	Ball* p_ball;

	float m_rotation;
	bool m_oldVisible = true;
};

