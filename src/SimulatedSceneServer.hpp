#ifndef SIMULATED_SCENE_SERVER_H
#define SIMULATED_SCENE_SERVER_H

#include <QTime>
#include <QTimer>
#include <QPoint>
#include <QMap>

#include <memory>

#include "Camera.hpp"
#include "ShipModel.hpp"

typedef int IDType;

struct ClientData
{
  ClientData( IDType _id ): id(_id) {}
  IDType id;
  std::string modelName;
};

typedef QMap<IDType, ClientData> ClientMap;

class SimulatedSceneServer: public QObject
{
    Q_OBJECT
public:
    SimulatedSceneServer();
    ~SimulatedSceneServer();

public:
    void process( int newTime );

    bool addModelFromFile( QString modelFileName );
    bool loadSceneFromFile( QString sceneFileName );

public slots:
    IDType registerClient();
    void getModel( IDType id, std::string modelName );

signals:
    void requestModel();

private:
    int m_lastTime;

    Eigen::Vector3f m_velocity;  //player
    Intersection m_minIntersection;

    std::vector<BaseSceneObjectPtr> m_sceneObjects;

    ClientMap m_clients;
    //std::vector<Client> m_clients;

};

typedef std::shared_ptr< SimulatedSceneServer > SimulatedSceneServerPtr;

#endif // SIMULATED_SCENE_SERVER_H
