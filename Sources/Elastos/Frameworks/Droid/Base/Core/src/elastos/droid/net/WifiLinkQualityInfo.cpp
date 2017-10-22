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

#include "elastos/droid/net/WifiLinkQualityInfo.h"
#include "elastos/droid/net/CWifiLinkQualityInfo.h"
#include "elastos/droid/net/LinkQualityInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(WifiLinkQualityInfo, LinkQualityInfo, IWifiLinkQualityInfo)

WifiLinkQualityInfo::WifiLinkQualityInfo()
    : mType(UNKNOWN_INT)
    , mRssi(UNKNOWN_INT)
    , mTxGood(UNKNOWN_LONG)
    , mTxBad(UNKNOWN_LONG)
{}

ECode WifiLinkQualityInfo::constructor()
{
    return NOERROR;
}

ECode WifiLinkQualityInfo::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mType;
    return NOERROR;
}

ECode WifiLinkQualityInfo::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode WifiLinkQualityInfo::GetBssid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mBssid;
    return NOERROR;
}

ECode WifiLinkQualityInfo::SetBssid(
    /* [in] */ const String& bssid)
{
    mBssid = bssid;
    return NOERROR;
}

ECode WifiLinkQualityInfo::GetRssi(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mRssi;
    return NOERROR;
}

ECode WifiLinkQualityInfo::SetRssi(
    /* [in] */ Int32 rssi)
{
    mRssi = rssi;
    return NOERROR;
}

ECode WifiLinkQualityInfo::GetTxGood(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTxGood;
    return NOERROR;
}

ECode WifiLinkQualityInfo::SetTxGood(
    /* [in] */ Int64 txGood)
{
    mTxGood = txGood;
    return NOERROR;
}

ECode WifiLinkQualityInfo::GetTxBad(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTxBad;
    return NOERROR;
}

ECode WifiLinkQualityInfo::SetTxBad(
    /* [in] */ Int64 txBad)
{
    mTxBad = txBad;
    return NOERROR;
}

ECode WifiLinkQualityInfo::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    LinkQualityInfo::ReadFromParcel(parcel);
    parcel->ReadInt32(&mType);
    parcel->ReadString(&mBssid);
    parcel->ReadInt32(&mRssi);
    parcel->ReadInt64(&mTxGood);
    parcel->ReadInt64(&mTxBad);
    return NOERROR;
}

ECode WifiLinkQualityInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    LinkQualityInfo::WriteToParcel(dest);
    dest->WriteInt32(mType);
    dest->WriteString(mBssid);
    dest->WriteInt32(mRssi);
    dest->WriteInt64(mTxGood);
    dest->WriteInt64(mTxBad);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
