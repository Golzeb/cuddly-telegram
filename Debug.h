#pragma once
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>

#include <array>

#include "Cue.h"

class Debug {
public:
	Debug();

	void setDeltaTimeLocation(float* deltaTime);
	void setCue(Cue* cue);
	void update();

	vtkTextActor* getActor2D();
private:
	vtkSmartPointer<vtkTextActor> m_actor;

	std::array<float, 60> m_average;
	int m_currentIndex;

	Cue* p_cue;
	float* p_deltaTime;
};