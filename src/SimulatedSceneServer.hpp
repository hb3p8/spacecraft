#ifndef SIMULATED_SCENE_SERVER_H
#define SIMULATED_SCENE_SERVER_H

#include <QTime>
#include <QTimer>
#include <QPoint>
#include <QMap>

#include <memory>

#include "Camera.hpp"
#include "ShipModel.hpp"


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

private slots:

private:
    int m_lastTime;

    Eigen::Vector3f m_velocity;  //player
    Intersection m_minIntersection;

    std::vector<BaseSceneObjectPtr> m_sceneObjects;
    std::vector<std::string> m_sceneObjectNames;

};

typedef std::shared_ptr< SimulatedSceneServer > SimulatedSceneServerPtr;

#endif // SIMULATED_SCENE_SERVER_H
