#include "Ball.h"

std::vector<Ball*> Ball::m_other;

Ball::Ball(double x, double z, double radius, double planeHeight)
{
	m_radius = radius;

	m_position.x = x;
	m_position.y = planeHeight + radius;
	m_position.z = z;

	m_sphere = vtkNew<vtkSphereSource>();

	m_sphere->SetCenter(0.0, 0.0, 0.0);
	m_sphere->SetRadius(radius);
	m_sphere->SetPhiResolution(32);
	m_sphere->SetThetaResolution(32);

	m_mapper = vtkNew<vtkDataSetMapper>();
	
	m_mapper->SetInputConnection(m_sphere->GetOutputPort());

	m_actor = vtkNew<vtkActor>();
	m_actor->SetMapper(m_mapper);
	m_actor->GetProperty()->SetRepresentationToSurface();
	m_actor->SetPosition((double*)&m_position);

	m_other.push_back(this);
}

Ball::~Ball()
{
	auto e = std::find(m_other.begin(), m_other.end(), this);
	m_other.erase(e);
}

void Ball::setLimits(const Limits& limits)
{
	m_limits = limits;
}

void Ball::setVelocity(const std::pair<float, float>& velocity)
{
	m_velocity = velocity;
}

void Ball::update(float delta) 
{
	static auto dot = [](float ax, float ay, float bx, float by) -> float { return ax * bx + ay * by; };
	static auto cross = [](float ax, float ay, float bx, float by) -> float { return ax * by - ay * bx; };
	static auto vecLength = [](float ax, float ay) -> float { return std::sqrt(ax * ax + ay * ay); };

	m_position.x += m_velocity.first * delta;
	m_position.z += m_velocity.second * delta;

	if (m_position.x + m_radius > m_limits.maxX) 
	{
		m_position.x = m_limits.maxX - m_radius;
		m_velocity.first *= -1.0f;
	}
	else if (m_position.x - m_radius < m_limits.minX) 
	{
		m_position.x = m_limits.minX + m_radius;
		m_velocity.first *= -1.0f;
	}

	if (m_position.z + m_radius > m_limits.maxZ) 
	{
		m_position.z = m_limits.maxZ - m_radius;
		m_velocity.second *= -1.0f;
	}
	else if (m_position.z - m_radius < m_limits.minZ) 
	{
		m_position.z = m_limits.minZ + m_radius;
		m_velocity.second *= -1.0f;
	}

	for (auto b : m_other) {
		if (b != this) {
			float xDir = m_position.x - b->m_position.x;
			float zDir = m_position.z - b->m_position.z;

			float posDifLength = vecLength(xDir, zDir);

			if (posDifLength < (m_radius + b->m_radius)) {
				float posDifLength = vecLength(xDir, zDir);

				std::cout << "Collision: " << posDifLength << " " << m_radius + b->m_radius << std::endl;

				m_position.x += (xDir / posDifLength) * std::abs(m_radius - b->m_radius) / 2.0f;
				m_position.y += (zDir / posDifLength) * std::abs(m_radius - b->m_radius) / 2.0f;

				b->m_position.x += (-xDir / posDifLength) * std::abs(m_radius - b->m_radius) / 2.0f;
				b->m_position.y += (-zDir / posDifLength) * std::abs(m_radius - b->m_radius) / 2.0f;

				float selfDot = dot(m_velocity.first - b->m_velocity.first, m_velocity.second - b->m_velocity.second, m_position.x - b->m_position.x, m_position.z - b->m_position.z);
				float otherDot = dot(b->m_velocity.first - m_velocity.first, b->m_velocity.second - m_velocity.second, b->m_position.x - m_position.x, b->m_position.z - m_position.z);

				std::cout << "Dots: " << selfDot << " " << otherDot << std::endl;
				std::cout << "Pos diff: " << (m_position.x - b->m_position.x) << " " << (m_position.z - b->m_position.z) << std::endl;

				m_velocity.first = m_velocity.first - (selfDot / (posDifLength * posDifLength) * (m_position.x - b->m_position.x));
				m_velocity.second = m_velocity.second - (selfDot / (posDifLength * posDifLength) * (m_position.z - b->m_position.z));

				b->m_velocity.first = b->m_velocity.first - (otherDot / (posDifLength * posDifLength) * (b->m_position.x - m_position.x));
				b->m_velocity.second = b->m_velocity.second - (otherDot / (posDifLength * posDifLength) * (b->m_position.z - m_position.z));

				std::cout << "Velocity self: " << m_velocity.first << " " << m_velocity.second << std::endl;
				std::cout << "Velocity other: " << b->m_velocity.first << " " << b->m_velocity.second << std::endl;

				/*m_velocity.first = 0.0f;
				m_velocity.second = 0.0f;

				b->m_velocity.first = 0.0f;
				b->m_velocity.second = 0.0f;*/
			}
		}
	}

	if(m_velocity.first >= 0.0f) {
		m_velocity.first -= FRICITON * delta;
		m_velocity.first = std::max(m_velocity.first, 0.0f);
	}
	else {
		m_velocity.first += FRICITON * delta;
		m_velocity.first = std::min(m_velocity.first, 0.0f);
	}

	if (m_velocity.second >= 0.0f) {
		m_velocity.second -= FRICITON * delta;
		m_velocity.second = std::max(m_velocity.second, 0.0f);
	}
	else {
		m_velocity.second += FRICITON * delta;
		m_velocity.second = std::min(m_velocity.second, 0.0f);
	}

	m_actor->SetPosition((double*)&m_position);
}


vtkActor* Ball::getActor()
{
	return m_actor;
}
