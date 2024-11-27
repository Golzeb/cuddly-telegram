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
#include <vtkInteractorStyle.h>
#include <vtkPropPicker.h>

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
#include <QTimer>

#include <cmath>
#include <cstdlib>
#include <random>
#include <thread>

#include "Table.h"
#include "Ball.h"
#include "Cue.h"
#include "Predictor.h"

class CustomInteractorStyle : public vtkInteractorStyle {
public:
    static CustomInteractorStyle* New() { return new CustomInteractorStyle(); }

    virtual void OnMouseMove() override {
        if (m_rightButtonHeld) {
            m_oldMousePos[0] = m_lastMousePos[0];
            m_oldMousePos[1] = m_lastMousePos[1];

            Interactor->GetEventPosition(m_lastMousePos);

            float deltaX = float(m_lastMousePos[0] - m_oldMousePos[0]);
            float deltaY = float(m_lastMousePos[1] - m_oldMousePos[1]);

            m_renderer->GetActiveCamera()->Azimuth(-deltaX * 0.25);

            //std::cout << "Moving: " << deltaX << " " << deltaY << std::endl;
        }

        if (m_leftButtonHeld) {
            m_oldMousePos[0] = m_lastMousePos[0];
            m_oldMousePos[1] = m_lastMousePos[1];

            Interactor->GetEventPosition(m_lastMousePos);

            float deltaX = float(m_lastMousePos[0] - m_oldMousePos[0]);
            float deltaY = float(m_lastMousePos[1] - m_oldMousePos[1]);

            if (m_picker->GetActor() == p_cue->getActor()) {
                p_cue->rotate(deltaX * 0.25);
            }
        }
    }

    virtual void OnRightButtonDown() override {
        m_rightButtonHeld = true;
        Interactor->GetEventPosition(m_lastMousePos);
    }

    virtual void OnRightButtonUp() override {
        m_rightButtonHeld = false;
    }

    virtual void OnLeftButtonDown() override {
        m_leftButtonHeld = true;
        Interactor->GetEventPosition(m_lastMousePos);

        m_picker->Pick(m_lastMousePos[0], m_lastMousePos[1], 0, m_renderer);
    }

    virtual void OnLeftButtonUp() override {
        m_leftButtonHeld = false;
    }

    virtual void OnMouseWheelForward() override {;
        m_renderer->GetActiveCamera()->Zoom(1.00 / 0.95);
    }

    virtual void OnMouseWheelBackward() override {
        m_renderer->GetActiveCamera()->Zoom(0.95);
    }

    virtual void OnKeyPress() override {
        char c = Interactor->GetKeyCode();

        std::cout << "#" << c << "#" << std::endl;

        if (c == ' ') {
            p_cue->shoot();
        }
    }

    void setRenderer(vtkRenderer* renderer) {
        m_renderer = renderer;
    }

    void setCue(Cue* cue) {
        p_cue = cue;
    }

private:
    bool m_rightButtonHeld = false;
    bool m_leftButtonHeld = false;
    int m_lastMousePos[2];
    int m_oldMousePos[2];

    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkNew<vtkPropPicker> m_picker;
    Cue* p_cue;
};

int main(int argc, char* argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);

    // main window
    QMainWindow mainWindow;
    mainWindow.resize(1200, 900);

    // control area
    //QDockWidget controlDock;
    //mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

    //QLabel controlDockTitle("Control Dock");
    //controlDockTitle.setMargin(20);
    //controlDock.setTitleBarWidget(&controlDockTitle);

    QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
    QWidget layoutContainer;
    layoutContainer.setLayout(dockLayout);
    //controlDock.setWidget(&layoutContainer);

    //QPushButton randomizeButton;
    //randomizeButton.setText("Randomize");
    //dockLayout->addWidget(&randomizeButton);

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

    ball1.setVelocity(std::make_pair(0.0f, 0.0f));
    ball1.setLimits(table.getLimits());

    Ball ball2(0.0, 0.0, BALL_RADIUS, PLANE_HEIGHT);

    ball2.setVelocity(std::make_pair(0.0f, 0.0f));
    ball2.setLimits(table.getLimits());
    ball2.getActor()->GetProperty()->SetColor(1.0, 0.0, 0.0);

    Cue cue;
    cue.setBall(&ball1);
    

    Predictor predictor(BALL_RADIUS);
    predictor.setLimits(table.getLimits());

    cue.setPredictor(&predictor);
    cue.rotate(180);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(ball1.getActor());
    renderer->AddActor(ball2.getActor());
    renderer->AddActor(table.getActor());
    renderer->AddActor(cue.getActor());
    renderer->AddActor(predictor.getActor());
    renderer->AddLight(table.getLight());

    renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
    renderer->GetActiveCamera()->SetPosition(1.5, 1.5 * 0.75, 0.0);

    window->AddRenderer(renderer);

    vtkNew<CustomInteractorStyle> style;
    style->setRenderer(renderer);
    style->setCue(&cue);

    window->GetInteractor()->SetInteractorStyle(style);

    // connect the buttons
    //QObject::connect(&randomizeButton, &QPushButton::released, [&]() {});

    mainWindow.show();

    QTimer timer;
    timer.setInterval(16);

    QElapsedTimer deltaTimer;

    float delta = 0;

    deltaTimer.restart();
    QObject::connect(&timer, &QTimer::timeout, [&](){
        window->Render();
        delta = deltaTimer.elapsed() / 1000.f;

        ball1.update(delta);
        ball2.update(delta);

        Ball::updateBallCollisions(delta);

        cue.update(delta);

        deltaTimer.restart();
    });

    timer.start();

    return app.exec();
}