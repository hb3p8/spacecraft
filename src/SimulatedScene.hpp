#ifndef SIMULATED_SCENE_H
#define SIMULATED_SCENE_H

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

//#include "Messages/ClientMessageHandler.h"
#include "Messages/MessageTypes.h"
#include "Messages/Dispatcher.h"

#include "FxManager.hpp"
#include "GUIManager.hpp"

namespace fx = spacefx;
namespace mes = messages;
namespace gui = space::gui;

QT_BEGIN_NAMESPACE
  class QTcpSocket;
QT_END_NAMESPACE

class GLRenderWidget;
namespace messages
{
  template <typename MessageTypes>
  class ClientHandler;
}

class SimulatedScene : public Scene
{
    Q_OBJECT

  friend class mes::ClientHandler<mes::MessageTypes>;

public:
    SimulatedScene();
    ~SimulatedScene();

    virtual void initialize();
    virtual void draw();

    virtual void process( int newTime );

    virtual void viewportResize( int width, int height );

    InputMap& getInputMap() { return m_inputMap; }

    virtual void keyPressEvent( QKeyEvent* e );
    virtual void mouseMoveEvent( QMouseEvent* e );
    virtual void mousePressEvent( QMouseEvent* e );
    virtual void mouseReleaseEvent( QMouseEvent* e );
    virtual void wheelEvent( QWheelEvent* e );

    bool addModelFromFile( QString modelFileName, int modelId = -1 );
    bool loadSceneFromFile( QString sceneFileName );

    void connectToServer( QString addres, int port );

    void handleDataUpdate( mes::MessageSnapshot* msg );


public slots:
    void readMessage();


private slots:
    void applyInput();


private:
    void buildStarMesh();

    QTcpSocket* m_tcpSocket;
    mes::Dispatcher<mes::MessageTypes> m_dispatcher;
    mes::ClientHandler<mes::MessageTypes>* m_handler;

    QGLShaderProgram m_cubeShader;
    QGLShaderProgram m_starShader;

    QPoint m_lastMousePos;
    TextRender m_text;

    CameraPtr m_camera;

    Mesh m_starMesh;

    int m_lastTime;

    Eigen::Vector3f m_velocity;  //player
    Intersection m_minIntersection;

    QMap<std::string, GLuint> m_textures;

    std::vector<BaseSceneObjectPtr> m_sceneObjects;
    std::vector<std::string> m_sceneObjectNames;

    ShipModel* m_playerShip;
    QString m_playerShipName;

    int m_ID;

    fx::Manager m_fxmanager;
    gui::GUIManager m_guiManager;

};

typedef std::shared_ptr< SimulatedScene > SimulatedScenePtr;

#endif // SIMULATED_SCENE_H
