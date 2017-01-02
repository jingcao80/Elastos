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
