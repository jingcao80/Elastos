
#ifndef __ELASTOS_DROID_SERVER_MASTERCLEARRECEIVER_H__
#define __ELASTOS_DROID_SERVER_MASTERCLEARRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Thread;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace Server {

class MasterClearReceiver
    : public BroadcastReceiver
{
private:
    class RebootThread
        : public Thread
    {
        friend class FileObserver;
    public:
        RebootThread(
            /* [in] */ Boolean shutdown,
            /* [in] */ const String& reason);

        CARAPI Run();

    private:
        Boolean mShutdown;
        String mReason;
    };

public:
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info);
        *info = String("MasterClearReceiver: ");
        (*info).AppendFormat("%p", this);
        return NOERROR;
    }
private:
    AutoPtr<RebootThread> mRebootThread;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_MASTERCLEARRECEIVER_H__
