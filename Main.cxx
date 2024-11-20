// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkPlaneSource.h>
#include <vtkImageData.h>
#include <vtkPNGReader.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkCamera.h>

#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDateTime>
#include <QElapsedTimer>

#include <cmath>
#include <cstdlib>
#include <random>
#include <thread>

#include "Table.h"
#include "Ball.h"

int main(int argc, char* argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);

    // main window
    QMainWindow mainWindow;
    mainWindow.resize(1200, 900);

    // control area
    QDockWidget controlDock;
    mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

    QLabel controlDockTitle("Control Dock");
    controlDockTitle.setMargin(20);
    controlDock.setTitleBarWidget(&controlDockTitle);

    QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
    QWidget layoutContainer;
    layoutContainer.setLayout(dockLayout);
    controlDock.setWidget(&layoutContainer);

    QPushButton randomizeButton;
    randomizeButton.setText("Randomize");
    dockLayout->addWidget(&randomizeButton);

    // render area
    QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget = new QVTKOpenGLNativeWidget();
    mainWindow.setCentralWidget(vtkRenderWidget);

    // VTK part
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    vtkRenderWidget->setRenderWindow(window.Get());

    Table table("hmap.png", "albedo.png");

    const float PLANE_HEIGHT = 0.0729412;
    const float BALL_RADIUS = (0.1 - PLANE_HEIGHT) / 2.0f;

    Ball ball1(0.4, 0.0, BALL_RADIUS, PLANE_HEIGHT);

    ball1.setVelocity(std::make_pair(0.5f, 0.1f));
    ball1.setLimits(table.getLimits());

    Ball ball2(0.0, 0.01, (0.1 - PLANE_HEIGHT) / 2.0f, PLANE_HEIGHT);

    ball2.setVelocity(std::make_pair(0.1f, 0.5f));
    ball2.setLimits(table.getLimits());

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(ball1.getActor());
    renderer->AddActor(ball2.getActor());
    renderer->AddActor(table.getActor());
    renderer->AddLight(table.getLight());

    renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);

    window->AddRenderer(renderer);

    // connect the buttons
    QObject::connect(&randomizeButton, &QPushButton::released, [&]() {});

    mainWindow.show();

    QElapsedTimer timer;

    float delta = 0;

    timer.restart();
    while (mainWindow.isVisible()) {
        app.processEvents();

        window->Render();
        delta = timer.elapsed() / 1000.f;
        
        ball1.update(delta);
        ball2.update(delta);

        timer.restart();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}