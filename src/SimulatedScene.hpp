#ifndef SIMULATED_SCENE_H
#define SIMULATED_SCENE_H

#include <QGLWidget>
#include <QGLBuffer>
#include <QGLShaderProgram>
#include <QTime>
#include <QTimer>
#include <QPoint>
#include <QMap>

#include <memory>

#include "Scene.hpp"
#include "TextRender.hpp"
#include "Camera.hpp"
#include "ShipModel.hpp"
#include "Mesh.hpp"


class GLRenderWidget;

class SimulatedScene : public Scene
{
    Q_OBJECT
public:
    SimulatedScene();
    ~SimulatedScene();

public:
    virtual void initialize();
    virtual void draw();

    virtual void process( int newTime );

    virtual void viewportResize( int width, int height );

    InputMap& getInputMap() { return m_inputMap; }

    virtual void keyPressEvent( QKeyEvent* e );
    virtual void mouseMoveEvent( QMouseEvent* e );
    virtual void mousePressEvent( QMouseEvent* e );
    virtual void wheelEvent( QWheelEvent* e );

    bool addModelFromFile( QString modelFileName );
    bool loadSceneFromFile( QString sceneFileName );

private slots:
    void applyInput();


private:
    void buildStarMesh();

    QGLShaderProgram m_cubeShader;
    QGLShaderProgram m_starShader;

    QPoint m_lastMousePos;
    TextRender m_text;

    CameraPtr m_camera;

//    ShipModel& m_shipModel;

    Mesh m_starMesh;

    int m_lastTime;

    Eigen::Vector3f m_velocity;  //player
    Intersection m_minIntersection;

    QMap<std::string, GLuint> m_textures;

    std::vector<BaseSceneObjectPtr> m_sceneObjects;

};

typedef std::shared_ptr< SimulatedScene > SimulatedScenePtr;

#endif // SIMULATED_SCENE_H
