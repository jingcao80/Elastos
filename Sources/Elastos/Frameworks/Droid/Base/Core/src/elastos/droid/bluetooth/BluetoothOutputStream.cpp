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

#include "elastos/droid/bluetooth/BluetoothOutputStream.h"
#include "elastos/droid/bluetooth/BluetoothSocket.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

BluetoothOutputStream::BluetoothOutputStream(
    /* [in] */ BluetoothSocket* s)
    : mSocket(s)
{}

ECode BluetoothOutputStream::Close()
{
    return ICloseable::Probe(mSocket)->Close();
}

ECode BluetoothOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr< ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(1);
    (*b)[0] = (Byte)oneByte;
    Int32 count;
    return mSocket->Write(b, 0, 1, &count);
}

ECode BluetoothOutputStream::WriteBytes(
    /* [in] */ const ArrayOf<Byte> & buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    // if (b == NULL) {
    //     throw new NullPointerException("buffer is null");
    // }
    if ((offset | count) < 0 || count > buffer.GetLength() - offset) {
//        throw new IndexOutOfBoundsException("invalid offset or length");
        Slogger::E("BluetoothOutputStream", "invalid offset or length");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    Int32 num;
    return mSocket->Write(const_cast<ArrayOf<Byte>*>(&buffer), offset, count, &num);
}

ECode BluetoothOutputStream::Flush()
{
    return IFlushable::Probe(mSocket)->Flush();
    //return NOERROR;
}

//ECode BluetoothOutputStream::WriteBytes(
//    /* [in] */ const ArrayOf<Byte> & buffer)
//{
//    return OutputStream::WriteBytes(buffer);
//}
//
//ECode BluetoothOutputStream::CheckError(
//    /* [out] */ Boolean* hasError)
//{
//    return OutputStream::CheckError(hasError);
//}
//
//ECode BluetoothOutputStream::GetLock(
//    /* [out] */ IInterface** lockobj)
//{
//    VALIDATE_NOT_NULL(lockobj);
//    AutoPtr<IInterface> obj = OutputStream::GetLock();
//    *lockobj = obj;
//    REFCOUNT_ADD(*lockobj);
//    return NOERROR;
//}

} // Bluetooth
} // Droid
} // Elastos
