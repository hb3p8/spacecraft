#ifndef SIMULATED_SCENE_SERVER_H
#define SIMULATED_SCENE_SERVER_H

#include <QTime>
#include <QTimer>
#include <QPoint>
#include <QMap>

#include <memory>

#include "Camera.hpp"
#include "ShipModel.hpp"

#include "Messages/MessageTypes.h"
#include "Messages/Dispatcher.h"

namespace mes = messages;

QT_BEGIN_NAMESPACE
  class QTcpServer;
  class QTcpSocket;
QT_END_NAMESPACE

namespace messages
{
  template <typename MessageTypes>
  class ServerHandler;
}

typedef qint32 IDType;
class SimulatedSceneServer;

class ClientData: public QObject
{
  Q_OBJECT

  friend class SimulatedSceneServer;

public:
  ClientData( SimulatedSceneServer* _server, IDType _id, QTcpSocket* _connection );

public slots:
  void newMessage();

private:
  SimulatedSceneServer* server;
  IDType id;
  QTcpSocket* connection;
  std::string modelName;
  ShipModel* model;
};

typedef QMap<IDType, ClientData*> ClientMap;

class SimulatedSceneServer: public QObject
{
    Q_OBJECT

    friend class mes::ServerHandler<mes::MessageTypes>;

public:
    SimulatedSceneServer();
    ~SimulatedSceneServer();

//    void doRequestModels() { emit requestModel(); }
    QString getServerAddres();
    int getServerPort();

    void fireCannons( int clientId );

public slots:
    void registerClient();
    void getModel( int id, std::string modelName );
    void enableEngines( int id, bool enabled );
    void work();

    void readMessage( QTcpSocket* connection );

    bool loadSceneFromFile( QString sceneFileName );

signals:
//    void requestModel();
//    void updateClientData( UpdateStruct );

private:
    void process( int newTime );

    bool addModelFromFile( QString modelFileName );

    QTcpServer* m_tcpServer;
    mes::Dispatcher<mes::MessageTypes> m_dispatcher;
    mes::ServerHandler<mes::MessageTypes>* m_handler;

    QTimer* m_timer;
    QTime* m_workTime;
    int m_lastTime;

    Eigen::Vector3f m_velocity;  //player
    Intersection m_minIntersection;

    std::vector<BaseSceneObjectPtr> m_sceneObjects;
    std::vector<QString> m_sceneObjectNames;

    ClientMap m_clients;
    qreal m_worldTimeRatio;



};

typedef std::shared_ptr< SimulatedSceneServer > SimulatedSceneServerPtr;

#endif // SIMULATED_SCENE_SERVER_H
