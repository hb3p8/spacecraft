#include "SimulatedSceneServer.hpp"

#include <QApplication>
#include <QMatrix>
#include <QFile>

#include "Utils.hpp"

#include <memory>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace Eigen;
using namespace std;


SimulatedSceneServer::SimulatedSceneServer() :
  m_lastTime( 0 ),
  m_velocity( Vector3f::Zero() )
{

}

bool SimulatedSceneServer::addModelFromFile( QString modelFileName )
{
  bool res = false;
  if( QFile::exists( modelFileName ) )
  {
    m_sceneObjects.push_back( BaseSceneObjectPtr( new ShipModel( modelFileName.toStdString() ) ) );
    m_sceneObjectNames.push_back( modelFileName.toStdString() );
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
  int deltaTime = newTime - m_lastTime;
  m_lastTime = newTime;

  float delta = deltaTime / 100.f;

  m_velocity *= 0.7;

//  applyInput();

//  m_camera->translate( m_velocity * delta );

  for( BaseSceneObjectPtr obj: m_sceneObjects )
    obj->process( delta );
}

//void SimulatedSceneServer::applyInput()
//{
//  Vector3f delta( 0.0F, 0.0F, 0.0F );
//  Vector3f deltaVelocity;
//  float scale = 1.0;

//  InputMap::const_iterator i;

//  i = m_inputMap.find( Qt::Key_W );
//  if( i != m_inputMap.end() && i.value() == true )
//    delta[ 2 ] += 1.0;

//  i = m_inputMap.find( Qt::Key_S );
//  if( i != m_inputMap.end() && i.value() == true )
//    delta[ 2 ] -= 1.0;

//  i = m_inputMap.find( Qt::Key_A );
//  if( i != m_inputMap.end() && i.value() == true )
//    delta[ 0 ] += 1.0;

//  i = m_inputMap.find( Qt::Key_D );
//  if( i != m_inputMap.end() && i.value() == true )
//    delta[ 0 ] -= 1.0;

//  deltaVelocity = m_camera->rotation() * delta;

//  m_velocity += deltaVelocity * scale;

//}

