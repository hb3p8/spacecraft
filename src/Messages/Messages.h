#pragma once
#include <QDataStream>
#include <QtNetwork>
#include <QVector3D>

namespace messages
{
  template< typename M >
  void sendMessage( M& message, QTcpSocket* connection )
  {
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    message.serialize(out);
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    connection->write(block);
  }

  class MessageSnapshot
  {
  public:
    QVector<QVector3D> positions;
    QVector<QVector3D> velocities;

    QVector<QVector3D> rotAxes;
    QVector<qreal> rotAngles;
    QVector<QVector3D> angularVelocities;

    QVector<QVector3D> massCenteres;

    QVector<qint32> objIDs;


  protected:
    void serialize( QDataStream & stream )
    {
      stream << positions;
      stream << velocities;
      stream << rotAxes;
      stream << rotAngles;
      stream << angularVelocities;
      stream << massCenteres;

      stream << objIDs;
    }
    void unserialize( QDataStream & stream )
    {
      stream >> positions;
      stream >> velocities;
      stream >> rotAxes;
      stream >> rotAngles;
      stream >> angularVelocities;
      stream >> massCenteres;

      stream >> objIDs;
    }
  };

  class MessageEngines
  {
  public:
    bool enginesEnabled;
    qint32 clientId;

  protected:
    void serialize( QDataStream & stream )
    {
      stream << enginesEnabled;
      stream << clientId;
    }
    void unserialize( QDataStream & stream )
    {
      stream >> enginesEnabled;
      stream >> clientId;
    }
  };

  class MessageCannons
  {
  public:
    qint32 clientId;

  protected:
    void serialize( QDataStream & stream )
    {
      stream << clientId;
    }
    void unserialize( QDataStream & stream )
    {
      stream >> clientId;
    }
  };

  // Подтверждаем соединение с сервера и высылаем ID клиенту
  class MessageAccept
  {
  public:
    qint32 clientId;

  protected:
    void serialize( QDataStream & stream )
    {
      stream << clientId;
    }
    void unserialize( QDataStream & stream )
    {
      stream >> clientId;
    }
  };

  // Посылаем с клиента данные для инициализации коробля игрока (пока шлём имя файла)
  class MessageInitModel
  {
  public:
    QString modelName;
    qint32 clientId;

  protected:
    void serialize( QDataStream & stream )
    {
      stream << modelName;
      stream << clientId;
    }
    void unserialize( QDataStream & stream )
    {
      stream >> modelName;
      stream >> clientId;
    }
  };

  // Подтверждаем с сервера инициализацию коробля игрока и возвращаем ID модели
  class MessageAcceptInit
  {
  public:
    qint32 playerShipId;

  protected:
    void serialize( QDataStream & stream )
    {
      stream << playerShipId;
    }
    void unserialize( QDataStream & stream )
    {
      stream >> playerShipId;
    }
  };

  // Посылаем с сервера данные для инициализации остальных объектов сцены (другие игроки, декорации)
  // пока тоже в виде имён файлов
  class MessageInitScene
  {
  public:
    QVector<QString> modelNames;
    QVector<qint32> modelIds;

  protected:
    void serialize( QDataStream & stream )
    {
      stream << modelNames;
      stream << modelIds;
    }
    void unserialize( QDataStream & stream )
    {
      stream >> modelNames;
      stream >> modelIds;
    }
  };



}
