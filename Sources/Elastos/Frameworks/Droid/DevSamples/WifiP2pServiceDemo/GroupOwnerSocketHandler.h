#ifndef __GROUPOWNERSOCKETHANDLER_H__
#define __GROUPOWNERSOCKETHANDLER_H__

#include "Elastos.Droid.Core.h"

using Elastos::Net::IServerSocket;
using Elastos::Utility::Concurrent::IThreadPoolExecutor;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pServiceDemo {

class ChatManager;

/**
 * The implementation of a ServerSocket handler. This is used by the wifi p2p
 * group owner.
 */
class GroupOwnerSocketHandler
    : public ThreadBase
{
public:
    GroupOwnerSocketHandler(
        /* [in] */ IHandler* handler);

    //@Override /*implements Thread*/
    virtual CARAPI Run();

private:
    static const String TAG; //"GroupOwnerSocketHandler";
    static const Int32 THREAD_COUNT; //=10;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IServerSocket> mSocket;

    /**
     * A ThreadPool for client sockets.
     */
    AutoPtr<IThreadPoolExecutor> mPool;
};

} // namespace WifiP2pServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__GROUPOWNERSOCKETHANDLER_H__

