#pragma once
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>

#include "Table.h"

struct Vector3 {
	double x;
	double y;
	double z;
};

class Ball {
public: 
	Ball(double x, double z, double radius, double planeHeight);
	~Ball();

	void setLimits(const Limits& limits);

	void setVelocity(const std::pair<float, float>& velocity);

	bool isMoving();

	void update(float delta);

	static void updateBallCollisions(float delta);
	
	vtkActor* getActor();
private:
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkSphereSource> m_sphere;

	Limits m_limits;

	double m_radius;
	Vector3 m_position;

	const float FRICITON = 0.1f; //0.05f

	std::pair<float, float> m_velocity;

	static std::vector<Ball*> m_other;
};