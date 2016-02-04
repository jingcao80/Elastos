
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHINPUTSTREAM_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHINPUTSTREAM_H__

//#include "Elastos.Droid.Core_server.h"
#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/io/InputStream.h"

using Elastos::IO::InputStream;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothSocket;

class BluetoothInputStream
    : public InputStream
{
public:
    BluetoothInputStream(
        /* [in] */ BluetoothSocket* s);

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Close();

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI ReadBytes(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

//    CARAPI Mark(
//        /* [in] */ Int32 readLimit);
//
//    CARAPI IsMarkSupported(
//        /* [out] */ Boolean* supported);
//
//    CARAPI ReadBytes(
//        /* [out] */ ArrayOf<Byte>* buffer,
//        /* [out] */ Int32* number);
//
//    CARAPI Reset();
//
//    CARAPI Skip(
//        /* [in] */ Int64 byteCount,
//        /* [out] */ Int64* number);
//
//    CARAPI GetLock(
//        /* [out] */ IInterface** lockobj);
//
private:
    BluetoothSocket* mSocket;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHINPUTSTREAM_H__
