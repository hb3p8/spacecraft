#pragma once

#include "TypeList.h"
#include "Messages.h"
#include "MessageWrapper.h"

namespace messages
{
    typedef types::Cons<MessageSnapshot,
            types::Cons<MessageEngines,
            types::Cons<MessageAccept,
            types::Cons<MessageInitModel,
            types::Cons<MessageInitScene,
            types::Cons<MessageAcceptInit
            > > > > > >

            MessageTypes;
}
