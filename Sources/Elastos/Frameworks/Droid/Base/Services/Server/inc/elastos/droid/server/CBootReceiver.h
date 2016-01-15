
#ifndef __ELASTOS_DROID_SERVER_CBOOTRECEIVER_H__
#define __ELASTOS_DROID_SERVER_CBOOTRECEIVER_H__

#include "_Elastos_Droid_Server_CBootReceiver.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Server {

class CBootReceiver : public BroadcastReceiver
{
public:
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI ToString(
        /* [out] */ String* info);
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBOOTRECEIVER_H__
