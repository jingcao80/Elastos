
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHOUTPUTSTREAM_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHOUTPUTSTREAM_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/io/OutputStream.h>

using Elastos::IO::IOutputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::OutputStream;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothSocket;

class BluetoothOutputStream
    : public ElRefBase
    , public IOutputStream
    , public IFlushable
    , public OutputStream
{
public:
    BluetoothOutputStream(
        /* [in] */ BluetoothSocket* socket);

    CAR_INTERFACE_DECL()

    CARAPI Close();

    CARAPI Flush();

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI WriteBytes(
        /* [in] */ const ArrayOf<Byte>& buffer);

    CARAPI WriteBytes(
        /* [in] */ const ArrayOf<Byte> & buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI CheckError(
        /* [out] */ Boolean* hasError);

    CARAPI GetLock(
        /* [out] */ IInterface** lockobj);

private:
    BluetoothSocket* mSocket;
};

}
}
}

#endif // __CBLUETOOTHOUTPUTSTREAM_H__
