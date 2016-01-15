#ifndef __ELASTOS_DROID_SERVER_CBRICKRECEIVER_H__
#define __ELASTOS_DROID_SERVER_CBRICKRECEIVER_H__

#include "_Elastos_Droid_Server_CBrickReceiver.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CBrickReceiver), public BroadcastReceiver{
public:
    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext *context,
        /* [in] */ IIntent *intent);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info);
        *info = String("CBrickReceiver: ");
        (*info).AppendFormat("%p", this);
        return NOERROR;
    }
};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CBRICKRECEIVER_H__
