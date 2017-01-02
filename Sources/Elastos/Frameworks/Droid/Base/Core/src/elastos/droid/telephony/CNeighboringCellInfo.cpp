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

#include "elastos/droid/telephony/CNeighboringCellInfo.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CNeighboringCellInfo, Object, INeighboringCellInfo, IParcelable)

CAR_OBJECT_IMPL(CNeighboringCellInfo)

CNeighboringCellInfo::CNeighboringCellInfo()
    : mRssi(UNKNOWN_RSSI)
    , mCid(UNKNOWN_CID)
    , mLac(UNKNOWN_CID)
    , mPsc(UNKNOWN_CID)
    , mNetworkType(ITelephonyManager::NETWORK_TYPE_UNKNOWN)
{
}

CNeighboringCellInfo::~CNeighboringCellInfo()
{
}

ECode CNeighboringCellInfo::constructor()
{
    return NOERROR;
}

ECode CNeighboringCellInfo::constructor(
    /* [in] */ Int32 rssi,
    /* [in] */ Int32 cid)
{
    mRssi = rssi;
    mCid = cid;
    return NOERROR;
}

ECode CNeighboringCellInfo::constructor(
    /* [in] */ Int32 rssi,
    /* [in] */ const String& location,
    /* [in] */ Int32 radioType)
{
    String str = location;
    // set default value
    mRssi = rssi;
    mNetworkType = ITelephonyManager::NETWORK_TYPE_UNKNOWN;
    mPsc = INeighboringCellInfo::UNKNOWN_CID;
    mLac = INeighboringCellInfo::UNKNOWN_CID;
    mCid = INeighboringCellInfo::UNKNOWN_CID;


    // pad location string with leading "0"
    Int32 l = str.GetLength();
    if (l > 8) return NOERROR;
    if (l < 8) {
        for (Int32 i = 0; i < (8-l); i++) {
            str = String("0") + str;
        }
    }
    // TODO - handle LTE and eHRPD (or find they can't be supported)
    //try {// set LAC/CID or PSC based on radioType
    switch (radioType) {
    case ITelephonyManager::NETWORK_TYPE_GPRS:
    case ITelephonyManager::NETWORK_TYPE_EDGE:
        mNetworkType = radioType;
        // check if 0xFFFFFFFF for INeighboringCellInfo::UNKNOWN_CID
        if (!str.EqualsIgnoreCase("FFFFFFFF")) {
            mCid = StringUtils::ParseInt32(str.Substring(4), 16);
            mLac = StringUtils::ParseInt32(str.Substring(0, 4), 16);
        }
        break;
    case ITelephonyManager::NETWORK_TYPE_UMTS:
    case ITelephonyManager::NETWORK_TYPE_HSDPA:
    case ITelephonyManager::NETWORK_TYPE_HSUPA:
    case ITelephonyManager::NETWORK_TYPE_HSPA:
        mNetworkType = radioType;
        mPsc = StringUtils::ParseInt32(str, 16);
        break;
    }
    // } catch (NumberFormatException e) {
    //     // parsing location error
    //     mPsc = INeighboringCellInfo::UNKNOWN_CID;
    //     mLac = INeighboringCellInfo::UNKNOWN_CID;
    //     mCid = INeighboringCellInfo::UNKNOWN_CID;
    //     mNetworkType = ITelephonyManager::ITelephonyManager::NETWORK_TYPE_UNKNOWN;
    // }

    return NOERROR;
}
ECode CNeighboringCellInfo::GetRssi(
    /* [out] */ Int32* rssi)
{
    VALIDATE_NOT_NULL(rssi);
    *rssi = mRssi;
    return NOERROR;
}

ECode CNeighboringCellInfo::GetLac(
    /* [out] */ Int32* lac)
{
    VALIDATE_NOT_NULL(lac);
    *lac = mLac;
    return NOERROR;
}

ECode CNeighboringCellInfo::GetCid(
    /* [out] */ Int32* cid)
{
    VALIDATE_NOT_NULL(cid);
    *cid = mCid;
    return NOERROR;
}

ECode CNeighboringCellInfo::GetPsc(
    /* [out] */ Int32* psc)
{
    VALIDATE_NOT_NULL(psc);
    *psc = mPsc;
    return NOERROR;
}

ECode CNeighboringCellInfo::GetNetworkType(
    /* [out] */ Int32* networkType)
{
    VALIDATE_NOT_NULL(networkType);
    *networkType = mNetworkType;
    return NOERROR;
}

ECode CNeighboringCellInfo::SetCid(
    /* [in] */ Int32 cid)
{
    mCid = cid;
    return NOERROR;
}

ECode CNeighboringCellInfo::SetRssi(
    /* [in] */ Int32 rssi)
{
    mRssi = rssi;
    return NOERROR;
}

ECode CNeighboringCellInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;

    sb.Append("[");
    if (mPsc != INeighboringCellInfo::UNKNOWN_CID) {
        sb.Append(StringUtils::ToHexString(mPsc));
        sb.Append("@");
        sb.Append(((mRssi == INeighboringCellInfo::UNKNOWN_RSSI)? String("-") : StringUtils::ToString(mRssi)));
    } else if(mLac != INeighboringCellInfo::UNKNOWN_CID && mCid != INeighboringCellInfo::UNKNOWN_CID) {
        sb.Append(StringUtils::ToHexString(mLac));
        sb.Append(StringUtils::ToHexString(mCid));
        sb.Append("@");
        sb.Append(((mRssi == INeighboringCellInfo::UNKNOWN_RSSI)? String("-") : StringUtils::ToString(mRssi)));
    }
    sb.Append("]");
    *str = sb.ToString();
    return NOERROR;
}

ECode CNeighboringCellInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mRssi);
    source->ReadInt32(&mLac);
    source->ReadInt32(&mCid);
    source->ReadInt32(&mPsc);
    source->ReadInt32(&mNetworkType);
    return NOERROR;
}

ECode CNeighboringCellInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mRssi);
    dest->WriteInt32(mLac);
    dest->WriteInt32(mCid);
    dest->WriteInt32(mPsc);
    dest->WriteInt32(mNetworkType);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

