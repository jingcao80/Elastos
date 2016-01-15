#ifndef __CHATMANAGER_H__
#define __CHATMANAGER_H__

#include "Elastos.Droid.Core.h"

using Elastos::ElRefBase;
using Elastos::Core::IRunnable;
using Elastos::Net::ISocket;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pServiceDemo {

extern "C" const InterfaceID EIID_ChatManager;

/**
 * Handles reading and writing of messages with socket buffers. Uses a Handler
 * to post messages to UI thread for UI updates.
 */
class ChatManager
    : public ElRefBase
    , public IRunnable
{
public:
    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    ChatManager(
        /* [in] */ ISocket* socket,
        /* [in] */ IHandler* handler);

    virtual CARAPI Run();

    void Write(
        /* [in] */ ArrayOf<Byte>* buffer);

private:
    static const String TAG;// = "ChatHandler";

    AutoPtr<IInputStream> mInputStream;
    AutoPtr<IOutputStream> mOutputStream;

    AutoPtr<ISocket> mSocket;
    AutoPtr<IHandler> mHandler;
};

} // namespace WifiP2pServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif //__CHATMANAGER_H__
