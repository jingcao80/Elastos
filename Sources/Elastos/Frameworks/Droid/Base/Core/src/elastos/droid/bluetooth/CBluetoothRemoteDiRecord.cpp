#include "elastos/droid/bluetooth/CBluetoothRemoteDiRecord.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CAR_OBJECT_IMPL(CBluetoothRemoteDiRecord)

CAR_INTERFACE_IMPL_2(CBluetoothRemoteDiRecord, Object, IBluetoothRemoteDiRecord, IParcelable)

CBluetoothRemoteDiRecord::CBluetoothRemoteDiRecord()
    : mVendorId(0)
    , mVendorIdSource(0)
    , mProductId(0)
    , mProductVersion(0)
    , mSpecificationId(0)
{}

ECode CBluetoothRemoteDiRecord::constructor()
{
    return NOERROR;
}

ECode CBluetoothRemoteDiRecord::constructor(
    /* [in] */ Int32 vendorId,
    /* [in] */ Int32 vendorIdSource,
    /* [in] */ Int32 productId,
    /* [in] */ Int32 productVersion,
    /* [in] */ Int32 specificationId)
{
    mVendorId= vendorId;
    mVendorIdSource= vendorIdSource;
    mProductId= productId;
    mProductVersion= productVersion;
    mSpecificationId= specificationId;
    return NOERROR;
}

ECode CBluetoothRemoteDiRecord::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mVendorId);
    source->ReadInt32(&mVendorIdSource);
    source->ReadInt32(&mProductId);
    source->ReadInt32(&mProductVersion);
    source->ReadInt32(&mSpecificationId);
    return NOERROR;
}

ECode CBluetoothRemoteDiRecord::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mVendorId);
    dest->WriteInt32(mVendorIdSource);
    dest->WriteInt32(mProductId);
    dest->WriteInt32(mProductVersion);
    dest->WriteInt32(mSpecificationId);
    return NOERROR;
}

ECode CBluetoothRemoteDiRecord::GetVendorId(
    /* [out] */ Int32* vendorId)
{
    AutoLock lock(mObject);
    *vendorId = mVendorId;
    return NOERROR;
}

ECode CBluetoothRemoteDiRecord::GetVendorIdSource(
    /* [out] */ Int32* vendorIdSource)
{
    AutoLock lock(mObject);
    *vendorIdSource = mVendorIdSource;
    return NOERROR;
}

ECode CBluetoothRemoteDiRecord::GetProductId(
    /* [out] */ Int32* productId)
{
    AutoLock lock(mObject);
    *productId = mProductId;
    return NOERROR;
}

ECode CBluetoothRemoteDiRecord::GetProductVersion(
    /* [out] */ Int32* productVersion)
{
    AutoLock lock(mObject);
    *productVersion = mProductVersion;
    return NOERROR;
}

ECode CBluetoothRemoteDiRecord::GetSpecificationId(
    /* [out] */ Int32* specificationId)
{
    AutoLock lock(mObject);
    *specificationId = mSpecificationId;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos