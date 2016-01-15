
#include "BluetoothServerSocket.h"

using Elastos::IO::EIID_ICloseable;
using Elastos::Droid::Os::IMessage;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

BluetoothServerSocket::BluetoothServerSocket(
    /* [in] */ Int32 type,
    /* [in] */ Boolean auth,
    /* [in] */ Boolean encrypt,
    /* [in] */ Int32 port)
    : mMessage(0)
    , mChannel(port)
{
    mSocket = new BluetoothSocket(type, -1, auth, encrypt, NULL, port, NULL);
}

BluetoothServerSocket::BluetoothServerSocket(
    /* [in] */ Int32 type,
    /* [in] */ Boolean auth,
    /* [in] */ Boolean encrypt,
    /* [in] */ IParcelUuid* uuid)
    : mMessage(0)
{
    mSocket = new BluetoothSocket(type, -1, auth, encrypt, NULL, -1, uuid);
    mChannel = mSocket->GetPort();
}

CAR_INTERFACE_IMPL_2(BluetoothServerSocket, IBluetoothServerSocket, ICloseable)

ECode BluetoothServerSocket::Accept(
    /* [out] */ IBluetoothSocket** socket)
{
    return Accept(-1, socket);
}

ECode BluetoothServerSocket::Accept(
    /* [in] */ Int32 timeout,
    /* [out] */ IBluetoothSocket** socket)
{
    VALIDATE_NOT_NULL(socket)
    return mSocket->Accept(timeout, socket);
}

ECode BluetoothServerSocket::Close()
{
    AutoLock lock(mLock);
    if (mHandler != NULL) {
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(mMessage, (IMessage**)&msg);
        msg->SendToTarget();
    }
    mSocket->Close();
    return NOERROR;
}

void BluetoothServerSocket::SetCloseHandler(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 message)
{
    mHandler = handler;
    mMessage = message;
}

void BluetoothServerSocket::SetServiceName(
    /* [in] */ const String& serviceName)
{
    mSocket->SetServiceName(serviceName);
}

ECode BluetoothServerSocket::GetChannel(
    /* [out] */ Int32* channel)
{
    VALIDATE_NOT_NULL(channel)
    *channel = mChannel;
    return NOERROR;
}

} // Bluetooth
} // Droid
} // Elastos

