
#ifndef __ELASTOS_DROID_SERVER_REMOTE_ROUTERSETTINGFACTORY_H__
#define __ELASTOS_DROID_SERVER_REMOTE_ROUTERSETTINGFACTORY_H__

#include "remote/AbstractDeviceFactory.h"

using Elastos::Net::IServerSocket;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

class RouterSettingFactory : public AbstractDeviceFactory
{
private:
    // the thread for send intent to RouterSettings in box
    class SendIntentThread : public ThreadBase
    {
    public:
        SendIntentThread(
            /* [in] */ RouterSettingFactory* host);

        CARAPI Run();

    private:
        AutoPtr<RouterSettingFactory> mHost;
    };


public:
    //Using public constructor for java reflection
    RouterSettingFactory(
        /* [in] */ IContext* ctx);

    CARAPI RunForResult(
        /* [in] */ const String& cmd,
        /* [out] */ String* result);

private:
    CARAPI_(Boolean) IsCorrectDevice();

    CARAPI_(String) SocketCmd_Task();

private:
    static const String TAG;
    static const Boolean DBG;

    String mRemoteCmd;
    Int32 mCmdCheckSN;
    Int32 mPort;
    String mCmd;
    String mKey;
    String mValue;
    AutoPtr<IServerSocket> mServersocket;
};

} // Remote
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_REMOTE_ROUTERSETTINGFACTORY_H__
