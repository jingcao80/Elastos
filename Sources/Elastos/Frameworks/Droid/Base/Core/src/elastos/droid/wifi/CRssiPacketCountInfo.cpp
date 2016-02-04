
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CRssiPacketCountInfo.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CRssiPacketCountInfo, Object, IRssiPacketCountInfo)

CAR_OBJECT_IMPL(CRssiPacketCountInfo)

ECode CRssiPacketCountInfo::constructor()
{
    mRssi = mTxgood = mTxbad = mRxgood = 0;

    return NOERROR;
}

ECode CRssiPacketCountInfo::constructor(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);

    FAIL_RETURN(parcel->ReadInt32(&mRssi));
    FAIL_RETURN(parcel->ReadInt32(&mTxgood));
    FAIL_RETURN(parcel->ReadInt32(&mTxbad));
    FAIL_RETURN(parcel->ReadInt32(&mRxgood));

    return NOERROR;
}

ECode CRssiPacketCountInfo::GetRssi(
    /* [out] */ Int32* rssi)
{
    VALIDATE_NOT_NULL(rssi);
    *rssi = mRssi;
    return NOERROR;
}

ECode CRssiPacketCountInfo::SetRssi(
    /* [in] */ Int32 rssi)
{
    mRssi = rssi;
    return NOERROR;
}

ECode CRssiPacketCountInfo::GetTxgood(
    /* [out] */ Int32* txgood)
{
    VALIDATE_NOT_NULL(txgood);
    *txgood = mTxgood;
    return NOERROR;
}

ECode CRssiPacketCountInfo::SetTxgood(
    /* [in] */ Int32 txgood)
{
    mTxgood = txgood;
    return NOERROR;
}

ECode CRssiPacketCountInfo::GetTxbad(
    /* [out] */ Int32* txbad)
{
    VALIDATE_NOT_NULL(txbad);
    *txbad = mTxbad;
    return NOERROR;
}

ECode CRssiPacketCountInfo::SetTxbad(
    /* [in] */ Int32 txbad)
{
    mTxbad = txbad;
    return NOERROR;
}

ECode CRssiPacketCountInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return constructor(source);
}

ECode CRssiPacketCountInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mRssi);
    dest->WriteInt32(mTxgood);
    dest->WriteInt32(mTxbad);
    dest->WriteInt32(mRxgood);
    return NOERROR;
}

ECode CRssiPacketCountInfo::GetRxgood(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxgood;
    return NOERROR;
}

ECode CRssiPacketCountInfo::SetRxgood(
    /* [in] */ Int32 rxgood)
{
    mRxgood = rxgood;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
