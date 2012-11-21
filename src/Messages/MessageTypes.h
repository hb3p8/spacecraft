#pragma once

#include "TypeList.h"
#include "Messages.h"
#include "MessageWrapper.h"

namespace messages
{
    typedef types::Cons<MessageA,
            types::Cons<MessageText,
            types::Cons<MessageModel,
            types::Cons<MessageEngines
            > > > >

            MessageTypes;
}
