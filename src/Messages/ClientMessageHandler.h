#pragma once
#include "MessageTypes.h"

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

    void handle(MessageWrapper<MessageA, MessageTypes>)
    {
      ::std::cout << "MessageA" << ::std::endl;
    }
    void handle(MessageWrapper<MessageText, MessageTypes> message)
    {
      ::std::cout << "MessageText: " << message.text.toStdString()
                    << " Id: " << message.clientId << ::std::endl;

      m_scene.m_ID = message.clientId;

      MessageWrapper<MessageModel, MessageTypes> resp;
      resp.clientId = m_scene.m_ID;
      resp.modelName = QString( m_scene.m_sceneObjectNames[ 0 ].c_str() );
      sendMessage( resp, m_scene.m_tcpSocket );
    }
    void handle(MessageWrapper<MessageEngines, MessageTypes>)
    {
    }

    void handle(MessageWrapper<MessageModel, MessageTypes>)
    {
    }

  private:
    SimulatedScene& m_scene;
  };




  template <typename MessageTypes>
  class ServerHandler
  {
  public:
    ServerHandler( SimulatedSceneServer& server ): m_server( server ) {}

    void handle(MessageWrapper<MessageA, MessageTypes>)
    {
      ::std::cout << "Server MessageA" << ::std::endl;
    }
    void handle(MessageWrapper<MessageText, MessageTypes> message)
    {
      ::std::cout << "MessageText: " << message.text.toStdString()
                    << " Id: " << message.clientId << ::std::endl;

    }

    void handle(MessageWrapper<MessageModel, MessageTypes> message)
    {
      std::string str = message.modelName.toStdString();
      ::std::cout << "Server MessageModel: " << str << ::std::endl;
      m_server.getModel( message.clientId, str );
    }

    void handle(MessageWrapper<MessageEngines, MessageTypes> message)
    {
      ::std::cout << "Server Engines: " << message.enginesEnabled << ::std::endl;
      m_server.enableEngines( message.clientId, message.enginesEnabled );
    }

  private:
    SimulatedSceneServer& m_server;
  };


}
