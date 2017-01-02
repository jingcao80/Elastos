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

#include "elastos/droid/bluetooth/le/ScanResult.h"
#include "elastos/droid/bluetooth/le/ScanRecord.h"
#include "elastos/droid/bluetooth/CBluetoothDevice.h"
#include "elastos/utility/Objects.h"

using Elastos::Droid::Bluetooth::CBluetoothDevice;
using Elastos::Core::StringUtils;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                              ScanResult
//=====================================================================
CAR_INTERFACE_IMPL_2(ScanResult, Object, IScanResult, IParcelable);

ScanResult::ScanResult()
{
}

ECode ScanResult::constructor()
{
    return NOERROR;
}

ECode ScanResult::constructor(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ IScanRecord* scanRecord,
    /* [in] */ Int32 rssi,
    /* [in] */ Int64 timestampNanos)
{
    mDevice = device;
    mScanRecord = scanRecord;
    mRssi = rssi;
    mTimestampNanos = timestampNanos;
    return NOERROR;
}

ECode ScanResult::constructor(
    /* [in] */ IParcel* in)
{
    ReadFromParcel(in);
    return NOERROR;
}

ECode ScanResult::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mDevice != NULL) {
        dest->WriteInt32(1);
        IParcelable::Probe(mDevice)->WriteToParcel(dest);//, flags);
    } else {
        dest->WriteInt32(0);
    }
    if (mScanRecord != NULL) {
        dest->WriteInt32(1);
        //dest.writeByteArray(mScanRecord.getBytes());
        AutoPtr<ArrayOf<Byte> > bytes;
        mScanRecord->GetBytes((ArrayOf<Byte>**)&bytes);
        dest->WriteArrayOf((Handle32)bytes.Get());
    } else {
        dest->WriteInt32(0);
    }
    dest->WriteInt32(mRssi);
    dest->WriteInt64(mTimestampNanos);
    return NOERROR;
}

ECode ScanResult::GetDevice(
    /* [out] */ IBluetoothDevice** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDevice;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ScanResult::GetScanRecord(
    /* [out] */ IScanRecord** record)
{
    VALIDATE_NOT_NULL(record);
    *record = mScanRecord;
    REFCOUNT_ADD(*record);
    return NOERROR;
}

ECode ScanResult::GetRssi(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRssi;
    return NOERROR;
}

ECode ScanResult::GetTimestampNanos(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTimestampNanos;
    return NOERROR;
}

ECode ScanResult::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    //TODO return Objects.hash(mDevice, mRssi, mScanRecord, mTimestampNanos);
    assert(0);
    return NOERROR;
}

ECode ScanResult::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (TO_IINTERFACE(this) == obj) {
        return TRUE;
    }
    //if (obj == NULL|| getClass() != obj.getClass())
    if (obj == NULL|| IScanResult::Probe(obj) != NULL) {
        return FALSE;
    }
    AutoPtr<ScanResult> other = (ScanResult*)(IScanResult::Probe(obj));
    *result = Objects::Equals(mDevice, other->mDevice) && (mRssi == other->mRssi) &&
            Objects::Equals(mScanRecord, other->mScanRecord)
            && (mTimestampNanos == other->mTimestampNanos);
    return NOERROR;
}

ECode ScanResult::ToString(
    /* [out] */ String* info)
{
    *info = String("ScanResult{ mDevice=xxx") //+ mDevice
            + String(", mScanRecord=xxx")// + Objects::ToString(mScanRecord)
            + String(", mRssi=") + StringUtils::ToString(mRssi)
            + String(", mTimestampNanos=")
            + StringUtils::ToString(mTimestampNanos)
            + String("}");
    return NOERROR;
}

ECode ScanResult::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 flag = 0;
    if ((in->ReadInt32(&flag), flag) == 1) {
        AutoPtr<IBluetoothDevice> btDevice;
        CBluetoothDevice::New(String(""), (IBluetoothDevice**)&btDevice);
        IParcelable::Probe(btDevice)->ReadFromParcel(in);
        //mDevice = BluetoothDevice.CREATOR.createFromParcel(in);
        mDevice = btDevice;
    }
    flag = 0;
    if (in->ReadInt32(&flag) == 1) {
        AutoPtr<ArrayOf<Byte> > array;
        in->ReadArrayOf((Handle32*)&array);
        mScanRecord = ScanRecord::ParseFromBytes(array);
    }
    in->ReadInt32(&mRssi);
    in->ReadInt64(&mTimestampNanos);
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
