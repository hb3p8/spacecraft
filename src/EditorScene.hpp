#ifndef EDITOR_SCENE_H
#define EDITOR_SCENE_H

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
#include "IndexedMesh.hpp"
#include "SimpleMesh.hpp"


class GLRenderWidget;

class EditorScene : public Scene
{
    Q_OBJECT
public:
    EditorScene( GLRenderWidget* widget, QObject *parent = 0, QString modelFileName = "default.txt" );
    ~EditorScene();

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

private slots:
    void applyInput( float deltaTime );


private:
    void buildStarMesh();

    QGLShaderProgram m_cubeShader;
    QGLShaderProgram m_starShader;

    GLRenderWidget* m_widget;

    QPoint m_lastMousePos;
    TextRender m_text;

    CameraPtr m_camera;

    ShipModel m_shipModel;

    SimpleMesh m_starMesh;

    int m_lastTime;

    Eigen::Vector3f m_velocity;
    bool m_isJumping;
    bool m_justJumped;
    Intersection m_moveIntersect;

};

typedef std::shared_ptr< EditorScene > EditorScenePtr;

#endif // EDITOR_SCENE_H
