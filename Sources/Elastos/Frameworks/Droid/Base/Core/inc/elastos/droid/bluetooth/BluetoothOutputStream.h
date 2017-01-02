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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHOUTPUTSTREAM_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHOUTPUTSTREAM_H__

//#include "Elastos.Droid.Core_server.h"
#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/io/OutputStream.h>

using Elastos::IO::OutputStream;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothSocket;

class BluetoothOutputStream
    : public OutputStream
{
public:
    BluetoothOutputStream(
        /* [in] */ BluetoothSocket* socket);

    CARAPI Close();

    CARAPI Flush();

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI WriteBytes(
        /* [in] */ const ArrayOf<Byte> & buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

//    CARAPI WriteBytes(
//        /* [in] */ const ArrayOf<Byte>& buffer);
//
//    CARAPI CheckError(
//        /* [out] */ Boolean* hasError);
//
//    CARAPI GetLock(
//        /* [out] */ IInterface** lockobj);
//
private:
    BluetoothSocket* mSocket;
};

}
}
}

#endif // __CBLUETOOTHOUTPUTSTREAM_H__
