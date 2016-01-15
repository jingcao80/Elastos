
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSERVERSOCKET_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSERVERSOCKET_H__

#include "BluetoothSocket.h"

using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothServerSocket
    : public ElRefBase
    , public IBluetoothServerSocket
    , public ICloseable
{
public:
    BluetoothServerSocket(
        /* [in] */ Int32 type,
        /* [in] */ Boolean auth,
        /* [in] */ Boolean encrypt,
        /* [in] */ Int32 port);

    BluetoothServerSocket(
        /* [in] */ Int32 type,
        /* [in] */ Boolean auth,
        /* [in] */ Boolean encrypt,
        /* [in] */ IParcelUuid* uuid);

    CAR_INTERFACE_DECL()

    CARAPI Accept(
        /* [out] */ IBluetoothSocket** socket);

    CARAPI Accept(
        /* [in] */ Int32 timeout,
        /* [out] */ IBluetoothSocket** socket);

    CARAPI Close();

    CARAPI_(void) SetCloseHandler(
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 message);

    CARAPI_(void) SetServiceName(
        /* [in] */ const String& serviceName);

    CARAPI GetChannel(
        /* [out] */ Int32* channel);

public:
    AutoPtr<BluetoothSocket> mSocket;

private:
    AutoPtr<IHandler> mHandler;
    Int32 mMessage;
    Int32 mChannel;

    Object mLock;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __CBLUETOOTHSERVERSOCKET_H__
