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
  m_timer->start( 10 );
  connect( m_timer, SIGNAL( timeout() ), this, SLOT( work() ) );

  m_workTime->start();

  m_tcpServer = new QTcpServer(this);
  if (!m_tcpServer->listen()) {
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
  bool res = false;
  if( QFile::exists( modelFileName ) )
  {
    m_sceneObjects.push_back( BaseSceneObjectPtr( new ShipModel( modelFileName.toStdString() ) ) );
    res = true;
  }
  return res;
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

  m_velocity *= 0.7;

//  applyInput();

//  m_camera->translate( m_velocity * delta );

  for( BaseSceneObjectPtr obj: m_sceneObjects )
    obj->process( delta );

//  UpdateStruct ustruct;
//  ShipModel& clientShipModel = *m_clients.find( 1 ).value().model;
//  ustruct.position = clientShipModel.m_position;
//  ustruct.rotation = clientShipModel.m_rotation;
//  ustruct.velocity = clientShipModel.m_velocity;
//  ustruct.angularVelocity = clientShipModel.m_angularVelocity;
//  ustruct.massCenter= clientShipModel.m_massCenter;
//  if( newTime % 10 == 0 )
//    emit updateClientData( ustruct );
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

  mes::MessageWrapper<mes::MessageText, mes::MessageTypes> msg;
  msg.text = tr("sample message");
  msg.clientId = id;

  QTcpSocket *clientConnection = m_tcpServer->nextPendingConnection();
  connect(clientConnection, SIGNAL(disconnected()),
          clientConnection, SLOT(deleteLater())); //!

  ClientData* client = new ClientData( this, id, clientConnection );
  m_clients.insert( id,  client );
  connect( clientConnection, SIGNAL( readyRead() ),
           client, SLOT( newMessage() ) );

  mes::sendMessage( msg, clientConnection );

//  mes::MessageWrapper<mes::MessageA, mes::MessageTypes> amsg;
//  mes::sendMessage( amsg, clientConnection );

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

    data->model = 0;
    if( QFile::exists( QString( modelName.c_str() ) ) )
    {
      data->model = new ShipModel( modelName, true );
      data->model->refreshModel();
      m_sceneObjects.push_back( BaseSceneObjectPtr( data->model ) );
    }

    assert( data->model != 0 );

  }
}


