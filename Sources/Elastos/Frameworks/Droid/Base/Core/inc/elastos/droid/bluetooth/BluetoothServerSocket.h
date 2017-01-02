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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSERVERSOCKET_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHSERVERSOCKET_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/bluetooth/BluetoothSocket.h"

using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothServerSocket
    : public Object
    , public IBluetoothServerSocket
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    BluetoothServerSocket();

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
