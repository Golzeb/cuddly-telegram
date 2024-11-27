#include "Table.h"

Table::Table(const std::string& heightmapPath, const std::string& albedoPath) {
	m_reader = vtkNew<vtkPNGReader>();

	m_reader->SetFileName(heightmapPath.c_str());
	m_reader->Update();

	m_heightmap = m_reader->GetOutput()->NewInstance();
    m_heightmap->DeepCopy(m_reader->GetOutput());

	m_reader->SetFileName(albedoPath.c_str());
	m_reader->Update();

	m_albedo = m_reader->GetOutput()->NewInstance();
    m_albedo->DeepCopy(m_reader->GetOutput());

	m_mapper = vtkNew<vtkDataSetMapper>();
	m_actor = vtkNew<vtkActor>();

	m_actor->SetMapper(m_mapper);
	m_actor->GetProperty()->SetRepresentationToSurface();
    m_actor->SetPickable(false);

    m_light = vtkNew<vtkLight>();
    m_light->SetLightTypeToSceneLight();
    m_light->SetConeAngle(10.0);
    m_light->SetPosition(0.0, 1.0, 0.0);
    m_light->SetFocalPoint(0.0, 0.0, 0.0);

	_preparePolyData();
}

vtkActor* Table::getActor() {
	return m_actor;
}

vtkLight* Table::getLight() {
    return m_light;
}

Limits Table::getLimits()
{
    const float PIXEL_COUNT = 40.0f;

    Limits limits = {};

    auto dimensions = m_heightmap->GetDimensions();

    float pixelSize = 1.0f / dimensions[0];
    float offset = pixelSize * PIXEL_COUNT;

    float x = (1.0f / 2.0f) - offset;
    float z = float(dimensions[1]) / float(dimensions[0]) / 2.0f - offset;

    limits.minX = -x;
    limits.maxX = x;
    limits.minZ = -z;
    limits.maxZ = z;

    //std::cout << x << ", " << z << std::endl;

    return limits;
}

void Table::_preparePolyData()
{
    auto dimensions = m_heightmap->GetDimensions();

    std::set<float> heightSet;

    vtkNew<vtkPlaneSource> plane;

    auto hmapColors = m_heightmap->GetPointData()->GetScalars();
    auto albedoColors = m_albedo->GetPointData()->GetScalars();

    std::cout << dimensions[0] << "x" << dimensions[1] << std::endl;

    float x = 1.0;
    float z = float(dimensions[1]) / float(dimensions[0]);

    plane->SetOrigin(0.0, 0.0, 0.0);
    plane->SetPoint1(x, 0.0, 0.0);
    plane->SetPoint2(0.0, 0.0, z);
    plane->SetNormal(0.0, 1.0, 0.0);
    plane->SetResolution(dimensions[0] - 1, dimensions[1] - 1);

    plane->Update();

    vtkSmartPointer<vtkPolyData> table;

    table.TakeReference(plane->GetOutput()->NewInstance());
    table->DeepCopy(plane->GetOutput());

    vtkSmartPointer<vtkUnsignedCharArray> albedoScalars = vtkSmartPointer<vtkUnsignedCharArray>::New();
    albedoScalars->SetNumberOfComponents(3);
    albedoScalars->SetName("Colors");
    albedoScalars->SetNumberOfTuples(table->GetNumberOfPoints());
    table->GetPointData()->AddArray(albedoScalars);

    for (int iP = 0; iP < table->GetNumberOfPoints(); iP++)
    {
        auto hmapColor = hmapColors->GetTuple(iP);
        auto albedoColor = albedoColors->GetTuple(iP);

        auto pointPosition = table->GetPoint(iP);

        pointPosition[1] = MIN + (float(hmapColor[0]) / 255.f) * (MAX - MIN);
        heightSet.insert(pointPosition[1]);

        table->GetPoints()->SetPoint(iP, pointPosition);
        albedoScalars->SetTuple3(iP, albedoColor[0], albedoColor[1], albedoColor[2]);
    }

    table->GetPointData()->SetScalars(albedoScalars);

    m_actor->SetPosition(x / -2.f, 0.0, z / -2.f);

    for (auto& height : heightSet) {
        std::cout << height << std::endl;
    }

    vtkNew<vtkSmoothPolyDataFilter> smoothFilter;
    smoothFilter->SetInputData(table);
    smoothFilter->SetNumberOfIterations(60);
    smoothFilter->SetRelaxationFactor(0.05);
    smoothFilter->Update();

    m_mapper->SetInputConnection(smoothFilter->GetOutputPort());
    m_mapper->SetScalarModeToUsePointData();
    m_mapper->ColorByArrayComponent("Colors", 0);
}