#include "SimulatedSceneServer.hpp"

#include <QApplication>
#include <QMatrix>
#include <QFile>

#include "Utils.hpp"

#include <memory>
#include <math.h>
#include <iostream>
#include <fstream>

#include <QtNetwork>
#include <QMessageBox>

#include "Messages/MessageWrapper.h"
#include "Messages/ClientMessageHandler.h"

using namespace Eigen;
using namespace std;

ClientData::ClientData( SimulatedSceneServer* _server, IDType _id, QTcpSocket* _connection ):
  QObject( server ), server( _server ), id( _id ), connection( _connection )
{
}

void ClientData::newMessage()
{
  server->readMessage( connection );
}


SimulatedSceneServer::SimulatedSceneServer() :
  m_timer( new QTimer( this ) ),
  m_workTime( new QTime() ),
  m_lastTime( 0 ),
  m_velocity( Vector3f::Zero() )
{

  m_timer = new QTimer( this );
  m_timer->start( 100 );
  connect( m_timer, SIGNAL( timeout() ), this, SLOT( work() ) );

  m_workTime->start();

  m_tcpServer = new QTcpServer(this);
  if (!m_tcpServer->listen( QHostAddress::Any, 34202 )) {
      QMessageBox::critical(0, tr("Spacecraft Server"),
                            tr("Unable to start the server: %1.")
                            .arg(m_tcpServer->errorString()));
      assert( false );
  }

  connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(registerClient()));

  QString ipAddress = QHostAddress(QHostAddress::LocalHost).toString();


  std::cout << tr("The server is running on\n\nIP: %1\nport: %2\n\n")
                .arg(ipAddress)
                .arg(m_tcpServer->serverPort()
               ).toStdString() << std::flush;

  m_handler = new mes::ServerHandler<mes::MessageTypes>( *this );
}

void SimulatedSceneServer::work()
{
  process( m_workTime->elapsed() );
}

bool SimulatedSceneServer::addModelFromFile( QString modelFileName )
{
  bool result = false;
  if( QFile::exists( modelFileName ) )
  {
    m_sceneObjects.push_back( BaseSceneObjectPtr( new ShipModel( modelFileName.toStdString(), true ) ) );
    m_sceneObjectNames.push_back( modelFileName );
    result = true;
  }
  return result;
}

bool SimulatedSceneServer::loadSceneFromFile( QString sceneFileName )
{
  bool res = false;
  m_sceneObjects.clear();

  if( QFile::exists( sceneFileName ) )
  {
    res = true;

    ifstream infile;
    infile.open( sceneFileName.toStdString().c_str() );

    size_t objCount;
    infile >> objCount;

    for( size_t i = 0; i < objCount; i++ )
    {
      string modelFileName;
      infile >> modelFileName;
      float x, y, z;
      infile >> x; infile >> y; infile >> z;

      if( addModelFromFile( QString( modelFileName.c_str() ) ) )
      {
        m_sceneObjects.back()->position() = Vector3d( x, y, z );
        m_sceneObjects.back()->refreshModel();
      }

    }

    infile.close();
  }

  return res;
}

SimulatedSceneServer::~SimulatedSceneServer()
{
}

void SimulatedSceneServer::process( int newTime )
{
//  std::cout << "process: " << newTime << std::endl << std::flush;
  int deltaTime = newTime - m_lastTime;
  m_lastTime = newTime;

  float delta = deltaTime / 100.f;

//  m_velocity *= 0.7;

//  applyInput();

//  m_camera->translate( m_velocity * delta );

  mes::MessageWrapper<mes::MessageSnapshot, mes::MessageTypes> msg;

  for( BaseSceneObjectPtr obj: m_sceneObjects )
  {
    obj->process( delta );    

    msg.objIDs.push_back( obj->m_id );
    msg.positions.push_back( eigenVectorToQt( obj->m_position ) );
    msg.rotAngles.push_back( obj->m_rotation.angle() );
    msg.rotAxes.push_back( eigenVectorToQt( obj->m_rotation.axis() ) );
    msg.velocities.push_back( eigenVectorToQt( obj->m_velocity ) );
    msg.angularVelocities.push_back( eigenVectorToQt( obj->m_angularVelocity ) );
    msg.massCenteres.push_back( eigenVectorToQt( obj->m_massCenter ) );
  }

  // отсылаем состояния всех объектов на клиенты
  for( ClientData* client: m_clients )
  {
    mes::sendMessage( msg, client->connection );
  }

}

QString SimulatedSceneServer::getServerAddres()
{
  return QHostAddress(QHostAddress::LocalHost).toString();
}

int SimulatedSceneServer::getServerPort()
{
  return m_tcpServer->serverPort();
}

void SimulatedSceneServer::registerClient()
{
  static int id = 0;
  id++;

  std::cout << "Registered ID: " << id << std::endl << std::flush;

  mes::MessageWrapper<mes::MessageAccept, mes::MessageTypes> msg;
  msg.clientId = id;

  QTcpSocket *clientConnection = m_tcpServer->nextPendingConnection();
  connect(clientConnection, SIGNAL(disconnected()),
          clientConnection, SLOT(deleteLater())); //!

  ClientData* client = new ClientData( this, id, clientConnection );
  m_clients.insert( id,  client );
  connect( clientConnection, SIGNAL( readyRead() ),
           client, SLOT( newMessage() ) );

  mes::sendMessage( msg, clientConnection );
}

void SimulatedSceneServer::readMessage( QTcpSocket* connection )
{
  QDataStream in( connection );
  in.setVersion( QDataStream::Qt_4_0 );

  static quint16 blockSize = 0;
  if ( blockSize == 0 ) {
      if ( connection->bytesAvailable() < (int)sizeof( quint16 ) )
          return;

      in >> blockSize;
  }

  if ( connection->bytesAvailable() < blockSize )
      return;

  m_dispatcher.dispatch( in, *m_handler );

  blockSize = 0;
}

void SimulatedSceneServer::enableEngines( int id, bool enabled )
{
  std::cout << "engines: " << enabled << std::endl << std::flush;
  ClientMap::iterator i;

  i = m_clients.find( id );
  if( i != m_clients.end() )
  {
    ClientData* data = i.value();

    ShipModel& shipModel = *data->model;
    if( enabled )
    {
      for( BlockRef engine : shipModel.getEngines() )
      {
        int idx = engine.generalIndex( shipModel.modelSize(), shipModel.getBlock( engine ).orientation );
        shipModel.enginePower().insert( idx, 1.0 );
      }

    } else
    {
      for( BlockRef engine : shipModel.getEngines() )
        shipModel.enginePower().insert(
              engine.generalIndex( shipModel.modelSize(), shipModel.getBlock( engine ).orientation ), 0.0 );
    }

  }

}

void SimulatedSceneServer::getModel( int id, std::string modelName )
{
  ClientMap::iterator i;

  i = m_clients.find( id );
  if( i != m_clients.end() )
  {
    ClientData* data = i.value();
    data->modelName = modelName;
    std::cout << "Given model name: " << modelName << std::endl << std::flush;

    mes::MessageWrapper<mes::MessageInitScene, mes::MessageTypes> sceneMsg;
    for( size_t j = 0; j < m_sceneObjects.size(); j++ )
    {
      sceneMsg.modelNames.push_back( m_sceneObjectNames[ j ] );
      sceneMsg.modelIds.push_back( m_sceneObjects[ j ]->m_id );
    }

    data->model = 0;
    if( QFile::exists( QString( modelName.c_str() ) ) )
    {
      data->model = new ShipModel( modelName, true );
      data->model->refreshModel();
      m_sceneObjects.push_back( BaseSceneObjectPtr( data->model ) );
      m_sceneObjectNames.push_back( QString( modelName.c_str() ) );

      mes::MessageWrapper<mes::MessageAcceptInit, mes::MessageTypes> acceptMsg;
      acceptMsg.playerShipId = m_sceneObjects.back()->m_id;
      mes::sendMessage( acceptMsg, data->connection );

      // Отсылаем новому клиенту всю уже загруженную сцену
      mes::sendMessage( sceneMsg, data->connection );

      mes::MessageWrapper<mes::MessageInitScene, mes::MessageTypes> newModelMsg;
      newModelMsg.modelNames.push_back( m_sceneObjectNames.back() );
      newModelMsg.modelIds.push_back( m_sceneObjects.back()->m_id );

      // Отсылаем всем старым клиентам новую модель
      for( ClientData* client: m_clients )
      {
        if( client == data ) continue;
        mes::sendMessage( newModelMsg, client->connection );
      }

    }

    assert( data->model != 0 );

  }
}
