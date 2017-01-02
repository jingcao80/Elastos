//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/bluetooth/BluetoothServerSocket.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Os::IMessage;
using Elastos::IO::EIID_ICloseable;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_INTERFACE_IMPL_2(BluetoothServerSocket, Object, IBluetoothServerSocket, ICloseable);

BluetoothServerSocket::BluetoothServerSocket()
{
}

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

