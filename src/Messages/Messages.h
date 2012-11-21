#pragma once
#include <QDataStream>
#include <QtNetwork>

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

  class MessageA
  {
  protected:
    void serialize(QDataStream &) { }
    void unserialize(QDataStream &) { }
  };

  class MessageModel
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

  class MessageText
  {
  public:
    QString text;
    qint32 clientId;

  protected:
    void serialize( QDataStream & stream )
    {
      stream << text;
      stream << clientId;
    }
    void unserialize( QDataStream & stream )
    {
      stream >> text;
      stream >> clientId;
    }
};




}
