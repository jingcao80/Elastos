
#ifndef __ELASTOS_DROID_SERVER_REMOTE_REMOTECMDLISTENER_H__
#define __ELASTOS_DROID_SERVER_REMOTE_REMOTECMDLISTENER_H__

#include <ext/frameworkext.h>

using Elastos::Net::IServerSocket;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class RemoteCmdListener : public ThreadBase
{
public:
    /**********************************************
     *
     *  RemoteCmdListener Constructor
     *
     **********************************************/
    RemoteCmdListener(
        /* [in] */ IContext* context,
        /* [in] */ IServerSocket* listenSocket);

    CARAPI_(void) Quit();

    CARAPI Run();

    CARAPI_(void) AcceptStringCmd();

private:
    static const String TAG;
    static const Boolean DBG;
    static const Int32 CMDTIMEOUT_SEC;

    AutoPtr<IServerSocket> mSocketListener;
    AutoPtr<IContext> mContext;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_REMOTECMDLISTENER_H__
