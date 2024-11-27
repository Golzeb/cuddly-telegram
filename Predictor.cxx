#include "Predictor.h"

Predictor::Predictor(float ballRadius)
{
	m_ballRadius = ballRadius;

	m_points = vtkNew<vtkPoints>();

	m_lines = vtkNew<vtkCellArray>();

	m_data = vtkNew<vtkPolyData>();

	m_data->SetPoints(m_points);
	m_data->SetLines(m_lines);

	m_mapper = vtkNew<vtkDataSetMapper>();
	m_mapper->SetInputData(m_data);

	m_actor = vtkNew<vtkActor>();
	m_actor->SetMapper(m_mapper);
	m_actor->GetProperty()->SetRepresentationToSurface();
	m_actor->SetPickable(false);
}

void Predictor::setPrediction(vtkVector3f origin, vtkVector2f direction, int maxBounces)
{
	m_points->Reset();
	m_lines->Reset();

	float yPos = origin.GetY();

	m_points->InsertNextPoint(origin.GetData());

	float oX = origin.GetX();
	float oZ = origin.GetZ();

	float dirX = direction.GetX();
	float dirZ = direction.GetY();

	//std::cout << dirX << " " << dirZ << std::endl;

	for (int i = 0; i < maxBounces; i++) {
		bool positiveX = dirX > 0.0f;
		bool positiveZ = dirZ > 0.0f;

		if (positiveX && positiveZ) {
			float tx = std::abs(m_limits.maxX - oX - m_ballRadius) / std::abs(dirX);
			float tz = std::abs(m_limits.maxZ - oZ - m_ballRadius) / std::abs(dirZ);

			if (tx > tz) {
				oX = tz * dirX + oX;
				oZ = tz * dirZ + oZ;
				m_points->InsertNextPoint(oX, yPos, oZ);

				dirZ *= -1.0f;
			}
			else {
				oX = tx * dirX + oX;
				oZ = tx * dirZ + oZ;
				m_points->InsertNextPoint(oX, yPos, oZ);

				dirX *= -1.0f;
			}
		}
		else if (positiveX && !positiveZ) {
			float tx = std::abs(m_limits.maxX - oX - m_ballRadius) / std::abs(dirX);
			float tz = std::abs(m_limits.minZ - oZ + m_ballRadius) / std::abs(dirZ);

			if (tx > tz) {
				oX = tz * dirX + oX;
				oZ = tz * dirZ + oZ;
				m_points->InsertNextPoint(oX, yPos, oZ);

				dirZ *= -1.0f;
			}
			else {
				oX = tx * dirX + oX;
				oZ = tx * dirZ + oZ;
				m_points->InsertNextPoint(oX, yPos, oZ);

				dirX *= -1.0f;
			}
		}
		else if (!positiveX && positiveZ) {
			float tx = std::abs(m_limits.minX - oX + m_ballRadius) / std::abs(dirX);
			float tz = std::abs(m_limits.maxZ - oZ - m_ballRadius) / std::abs(dirZ);

			if (tx > tz) {
				oX = tz * dirX + oX;
				oZ = tz * dirZ + oZ;
				m_points->InsertNextPoint(oX, yPos, oZ);

				dirZ *= -1.0f;
			}
			else {
				oX = tx * dirX + oX;
				oZ = tx * dirZ + oZ;
				m_points->InsertNextPoint(oX, yPos, oZ);

				dirX *= -1.0f;
			}
		}
		else {
			float tx = std::abs(m_limits.minX - oX + m_ballRadius) / std::abs(dirX);
			float tz = std::abs(m_limits.minZ - oZ + m_ballRadius) / std::abs(dirZ);

			if (tx > tz) {
				oX = tz * dirX + oX;
				oZ = tz * dirZ + oZ;
				m_points->InsertNextPoint(oX, yPos, oZ);

				dirZ *= -1.0f;
			}
			else {
				oX = tx * dirX + oX;
				oZ = tx * dirZ + oZ;
				m_points->InsertNextPoint(oX, yPos, oZ);

				dirX *= -1.0f;
			}
		}
	}

	for (int i = 0; i < maxBounces; i++) {
		m_lines->InsertNextCell(2);
		m_lines->InsertCellPoint(i);
		m_lines->InsertCellPoint(i + 1);
	}
}

void Predictor::setLimits(const Limits& limits) {
	m_limits = limits;
}

vtkActor* Predictor::getActor()
{
	return m_actor;
}
