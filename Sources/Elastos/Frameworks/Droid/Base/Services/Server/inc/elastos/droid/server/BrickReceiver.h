#ifndef __ELASTOS_DROID_SERVER_BRICKRECEIVER_H__
#define __ELASTOS_DROID_SERVER_BRICKRECEIVER_H__

#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace Server {

class BrickReceiver : public BroadcastReceiver
{
public:
    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext *context,
        /* [in] */ IIntent *intent);

    CARAPI ToString(
        /* [out] */ String* str);
};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_BRICKRECEIVER_H__
