#ifndef __CLIENTSOCKETHANDLER_H__
#define __CLIENTSOCKETHANDLER_H__

#include "Elastos.Droid.Core.h"

using Elastos::Net::IInetAddress;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pServiceDemo {

class ChatManager;

class ClientSocketHandler
    : public ThreadBase
{
public:
    ClientSocketHandler(
        /* [in] */ IHandler* handler,
        /* [in] */ IInetAddress* groupOwnerAddress);

    //@Override /*implements Thread*/
    virtual CARAPI Run();

    AutoPtr<ChatManager> GetChat();

private:
    static const String TAG; //"ClientSocketHandler";

    AutoPtr<IHandler> mHandler;
    AutoPtr<ChatManager> mChat;
    AutoPtr<IInetAddress> mAddress;
};

} // namespace WifiP2pServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CLIENTSOCKETHANDLER_H__
