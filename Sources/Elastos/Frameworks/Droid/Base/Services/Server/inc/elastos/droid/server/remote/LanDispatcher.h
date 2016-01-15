
#ifndef __ELASTOS_DROID_SERVER_REMOTE_LANDISPATCHER_H__
#define __ELASTOS_DROID_SERVER_REMOTE_LANDISPATCHER_H__

#include <ext/frameworkext.h>
#include "remote/ServerSocketTimer.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Net::IDatagramSocket;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class LanDispatcher : public ThreadBase
{
public:
    LanDispatcher(
        /* [in] */ IContext* context);

    CARAPI Run();

private:
    CARAPI_(String) GetVerifiedPort(
        /* [in] */ const String& clientMsg);

private:
    static const String TAG;
    static const Boolean DBG;
    static const String LISTENADDR;
    static const Int32 LISTENPORT;
    List<AutoPtr<ServerSocketTimer> > mServerSocketList;

    AutoPtr<IContext> mContext;
    AutoPtr<IDatagramSocket> mLanListenSocket;
    String mLogin;
    String mCheck;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_LANDISPATCHER_H__
