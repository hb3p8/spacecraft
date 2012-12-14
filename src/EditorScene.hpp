#ifndef EDITOR_SCENE_H
#define EDITOR_SCENE_H

#include <QGLBuffer>
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

class EditorScene : public Scene
{
    Q_OBJECT
public:
    EditorScene( QString modelFileName = "default.txt" );
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
    void applyInput();


private:
    void buildStarMesh();

    QGLShaderProgram m_cubeShader;
    QGLShaderProgram m_starShader;

    QPoint m_lastMousePos;
    TextRender m_text;

    CameraPtr m_camera;

    ShipModel m_shipModel;

    Mesh m_starMesh;

    int m_lastTime;

    Eigen::Vector3f m_velocity;
    bool m_isJumping;
    bool m_justJumped;
    Intersection m_moveIntersect;
    Intersection m_minIntersection;
    int m_blockToInsert;
    int m_currentOrient;
    QMap<std::string, GLuint> m_textures;
    bool m_gravity;

};

typedef std::shared_ptr< EditorScene > EditorScenePtr;

#endif // EDITOR_SCENE_H
