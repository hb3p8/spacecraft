#pragma once
#include "MessageTypes.h"

#include <Eigen/Eigen>
#include <iostream>
#include <QDataStream>
#include "../SimulatedScene.hpp"
#include "../SimulatedSceneServer.hpp"

namespace messages
{
  template <typename MessageTypes>
  class ClientHandler
  {
  public:
    ClientHandler( SimulatedScene& scene ): m_scene( scene ) {}

    void handle( MessageWrapper<MessageSnapshot, MessageTypes> msg )
    {
      m_scene.handleDataUpdate( &msg );
    }

    void handle( MessageWrapper<MessageAccept, MessageTypes> message )
    {
      m_scene.m_ID = message.clientId;

      MessageWrapper<MessageInitModel, MessageTypes> resp;
      resp.clientId = m_scene.m_ID;
      resp.modelName = m_scene.m_playerShipName;
      sendMessage( resp, m_scene.m_tcpSocket );
    }

    void handle( MessageWrapper<MessageInitModel, MessageTypes>)
    {
      assert( false );
    }

    void handle( MessageWrapper<MessageAcceptInit, MessageTypes> message )
    {
      m_scene.m_playerShip->setId( message.playerShipId );
    }

    void handle( MessageWrapper<MessageEngines, MessageTypes> )
    {
      assert( false );
    }

    void handle( MessageWrapper<MessageInitScene, MessageTypes> msg )
    {
      for( int i = 0; i < msg.modelIds.size(); i++ )
      {
        m_scene.addModelFromFile( msg.modelNames[ i ], msg.modelIds[ i ] );
        m_scene.m_sceneObjects.back()->refreshModel();
        m_scene.m_sceneObjects.back()->attachShader( m_scene.m_cubeShader );
      }
    }

  private:
    SimulatedScene& m_scene;
  };



  template <typename MessageTypes>
  class ServerHandler
  {
  public:
    ServerHandler( SimulatedSceneServer& server ): m_server( server ) {}

    void handle( MessageWrapper<MessageSnapshot, MessageTypes> )
    {
      assert( false );
    }

    void handle( MessageWrapper<MessageAccept, MessageTypes> )
    {
      assert( false );
    }

    void handle( MessageWrapper<MessageInitModel, MessageTypes> message )
    {
      m_server.getModel( message.clientId, message.modelName.toStdString() );
    }

    void handle( MessageWrapper<MessageAcceptInit, MessageTypes> message )
    {
      assert( false );
    }

    void handle( MessageWrapper<MessageEngines, MessageTypes> message )
    {
      m_server.enableEngines( message.clientId, message.enginesEnabled );
    }

    void handle( MessageWrapper<MessageInitScene, MessageTypes> )
    {
      assert( false );
    }

  private:
    SimulatedSceneServer& m_server;
  };


}
