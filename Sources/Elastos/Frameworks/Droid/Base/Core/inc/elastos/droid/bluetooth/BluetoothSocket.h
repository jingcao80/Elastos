
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSOCKET_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSOCKET_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>
//#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/bluetooth/BluetoothInputStream.h"
#include "elastos/droid/bluetooth/BluetoothOutputStream.h"

using Elastos::IO::ICloseable;;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothSocket
    : public Object
    , public IBluetoothSocket
    , public ICloseable
{
private:
    enum SocketState
    {
        INIT,
        CONNECTED,
        LISTENING,
        CLOSED,
    };

public:
    CAR_INTERFACE_DECL();

    BluetoothSocket();

    BluetoothSocket(
        /* [in] */ Int32 type,
        /* [in] */ Int32 fd,
        /* [in] */ Boolean auth,
        /* [in] */ Boolean encrypt,
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 port,
        /* [in] */ IParcelUuid* uuid);

    ~BluetoothSocket();

    CARAPI GetRemoteDevice(
        /* [out] */ IBluetoothDevice** device);

    CARAPI GetInputStream(
        /* [out] */ IInputStream** inputstream);

    CARAPI GetOutputStream(
        /* [out] */ IOutputStream** outputstream);

    CARAPI IsConnected(
        /* [out] */ Boolean* isConnected);

    CARAPI_(void) SetServiceName(
        /* [in] */ const String& name);

    CARAPI Connect();

    CARAPI_(Int32) BindListen();

    CARAPI Accept(
        /* [in] */ Int32 timeout,
        /* [out] */ IBluetoothSocket** socket);

    CARAPI Available(
        /* [out] */ Int32* result);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* count);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* count);

    CARAPI Close();

    CARAPI_(void) RemoveChannel();

    CARAPI_(Int32) GetPort();

private:
    BluetoothSocket(
        /* [in] */ BluetoothSocket* s);

    BluetoothSocket(
        /* [in] */ Int32 type,
        /* [in] */ Int32 fd,
        /* [in] */ Boolean auth,
        /* [in] */ Boolean encrypt,
        /* [in] */ const String& address,
        /* [in] */ Int32 port);

    CARAPI AcceptSocket(
        /* [in] */ const String& remoteAddr,
        /* [out] */ BluetoothSocket** socket);

    CARAPI_(Int32) GetSecurityFlags();

    CARAPI_(String) ConvertAddr(
        /* [in] */ ArrayOf<byte>* addr);

    CARAPI WaitSocketSignal(
        /* [in] */ IInputStream* is,
        /* [out] */ String* addr);

    CARAPI ReadAll(
        /* [in] */ IInputStream* is,
        /* [in] */ ArrayOf<byte>* b,
        /* [out] */ Int32* length);

    CARAPI ReadInt32(
        /* [in] */ IInputStream* is,
        /* [out] */ Int32* length);

public:
    /** Keep TYPE_ fields in sync with BluetoothSocket.cpp */
    /*package*/ static const Int32 TYPE_RFCOMM = 1;
    /*package*/ static const Int32 TYPE_SCO = 2;
    /*package*/ static const Int32 TYPE_L2CAP = 3;

    /*package*/ static const Int32 _EBADFD = 77;
    /*package*/ static const Int32 _EADDRINUSE = 98;

    /*package*/ static const Int32 SEC_FLAG_ENCRYPT = 1;
    /*package*/ static const Int32 SEC_FLAG_AUTH = 1 << 1;

private:
    const static String TAG;
    const static Boolean DBG = TRUE;
    const static Boolean VDBG = FALSE;
    static Int32 PROXY_CONNECTION_TIMEOUT;
    static Int32 SOCK_SIGNAL_SIZE;

    Int32 mType;  /* one of TYPE_RFCOMM etc */
    AutoPtr<IBluetoothDevice> mDevice;    /* remote device */

    String mAddress;    /* remote address */
    Boolean mAuth;
    Boolean mEncrypt;
    AutoPtr<BluetoothInputStream> mInputStream;
    AutoPtr<BluetoothOutputStream> mOutputStream;
    AutoPtr<IParcelUuid> mUuid;
    AutoPtr<IParcelFileDescriptor> mPfd;
    AutoPtr<ILocalSocket> mSocket;
    AutoPtr<IInputStream> mSocketIS;
    AutoPtr<IOutputStream> mSocketOS;
    Int32 mPort;  /* RFCOMM channel or L2CAP psm */
    Int32 mFd;
    String mServiceName;

    /** prevents all native calls after destroyNative() */
    SocketState mSocketState;
    Object mLock;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSOCKET_H__
