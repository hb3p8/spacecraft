#pragma once
#include "TypeList.h"

#include <QDataStream>

namespace messages
{

//! Returns Message's type id.
//!
//! \tparam Cons    Type list of all message types.
//! \tparam Message Message to search for.
//! \return Message's type id in Cons.
template <typename Cons, typename Message>
inline int type_id()
{
    return types::IndexOf<Cons, Message>::value;
}

//! Message wrapper that helps with serialization.
//!
//! \tparam MessageImpl Message implementation.
//! \tparam MessageTypes    Type list of all message types.
template <typename MessageImpl, typename MessageTypes>
class MessageWrapper : public MessageImpl
{
public:
    //! Serialize the message to stream.
    //!
    //! \param  stream  Stream that holds the serialized message.
    void serialize(QDataStream & stream)
    {
        stream << type_id<MessageTypes, MessageImpl>();
        MessageImpl::serialize(stream);
    }

    //! Unserialize the message from stream.
    //!
    //! \param  stream  Stream that holds the serialized message.
    void unserialize(QDataStream & stream)
    {

        MessageImpl::unserialize(stream);
    }
};
}
