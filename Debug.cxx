#include "Debug.h"

#include <sstream>

Debug::Debug()
{
	m_actor = vtkNew<vtkTextActor>();
	m_currentIndex = 0;

	m_average = {};
}
	
void Debug::update() 
{
	m_average[m_currentIndex] = *p_deltaTime;
	m_currentIndex = (m_currentIndex + 1) % 60;

	float sum = 0.0f;
	for (auto f : m_average) {
		sum += f;
	}

	float average = sum / 60.0f;

	std::stringstream ss;
	ss << "Average frame time: " << std::setprecision(3) << average << std::endl;
	ss << "Cue rotation: " << std::setprecision(4) << p_cue->getRotation();

	m_actor->SetInput(ss.str().c_str());
}

void Debug::setDeltaTimeLocation(float* deltaTime)
{
	p_deltaTime = deltaTime;
}

void Debug::setCue(Cue* cue) 
{
	p_cue = cue;
}

vtkTextActor* Debug::getActor2D()
{
	return m_actor;
}
