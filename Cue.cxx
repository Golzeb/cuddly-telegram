#include "Cue.h"

Cue::Cue()
{
	m_cylinder = vtkNew<vtkCylinderSource>();

	m_cylinder->SetHeight(0.6);
	m_cylinder->SetRadius(0.005);
	m_cylinder->SetResolution(32);
	m_cylinder->SetCenter(0.0, 0.35, 0.0);

	m_transform = vtkNew<vtkTransform>();

	m_transform->RotateZ(70);

	m_filter = vtkNew<vtkTransformFilter>();

	m_filter->SetTransform(m_transform);
	m_filter->SetInputConnection(m_cylinder->GetOutputPort());

	m_finalTransform = vtkNew<vtkTransform>();

	m_finalFilter = vtkNew<vtkTransformFilter>();
	m_finalFilter->SetTransform(m_finalTransform);

	m_finalFilter->SetInputConnection(m_filter->GetOutputPort());

	m_mapper = vtkNew<vtkDataSetMapper>();

	m_mapper->SetInputConnection(m_finalFilter->GetOutputPort());

	m_actor = vtkNew<vtkActor>();
	m_actor->SetMapper(m_mapper);
	m_actor->GetProperty()->SetRepresentationToSurface();
	m_actor->SetPosition(0, 0, 0);

	m_direction = vtkVector2f(1.0, 0.0);

	m_rotation = 0.0f;
}

Cue::~Cue()
{
}

vtkActor* Cue::getActor()
{
	return m_actor;
}


void Cue::shoot()
{
	auto data = m_finalTransform->TransformFloatVector(vtkVector3f(m_direction.GetX(), m_direction.GetY(), 0.0).GetData());

	p_ball->setVelocity(std::make_pair(data[0], data[2]));
}

void Cue::update(float delta) {
	bool visible = !p_ball->isMoving();

	if (visible && !m_oldVisible) {
		rotate(0);
	}

	m_actor->SetVisibility(visible);
	p_predictor->getActor()->SetVisibility(visible);
	m_actor->SetPosition(p_ball->getActor()->GetPosition());

	m_oldVisible = visible;
}

void Cue::rotate(float angle)
{
	m_rotation += angle;
	m_finalTransform->RotateY(angle);

	auto data = m_finalTransform->TransformFloatVector(vtkVector3f(m_direction.GetX(), m_direction.GetY(), 0.0).GetData());

	double ballPosition[3];
	p_ball->getActor()->GetPosition(ballPosition);

	p_predictor->setPrediction(vtkVector3f(ballPosition[0], ballPosition[1], ballPosition[2]), vtkVector2f(data[0], data[2]), 6);
}

float Cue::getRotation()
{
	return m_rotation;
}

void Cue::setBall(Ball* ball)
{
	p_ball = ball;
	m_actor->SetPosition(p_ball->getActor()->GetPosition());
}

void Cue::setPredictor(Predictor* predictor)
{
	p_predictor = predictor;
}
