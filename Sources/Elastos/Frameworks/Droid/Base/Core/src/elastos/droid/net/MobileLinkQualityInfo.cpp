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

#include "elastos/droid/net/MobileLinkQualityInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(MobileLinkQualityInfo, LinkQualityInfo, IMobileLinkQualityInfo)

MobileLinkQualityInfo::MobileLinkQualityInfo()
    : mMobileNetworkType(ILinkQualityInfo::UNKNOWN_INT)
    , mRssi(ILinkQualityInfo::UNKNOWN_INT)
    , mGsmErrorRate(ILinkQualityInfo::UNKNOWN_INT)
    , mCdmaDbm(ILinkQualityInfo::UNKNOWN_INT)
    , mCdmaEcio(ILinkQualityInfo::UNKNOWN_INT)
    , mEvdoDbm(ILinkQualityInfo::UNKNOWN_INT)
    , mEvdoEcio(ILinkQualityInfo::UNKNOWN_INT)
    , mEvdoSnr(ILinkQualityInfo::UNKNOWN_INT)
    , mLteSignalStrength(ILinkQualityInfo::UNKNOWN_INT)
    , mLteRsrp(ILinkQualityInfo::UNKNOWN_INT)
    , mLteRsrq(ILinkQualityInfo::UNKNOWN_INT)
    , mLteRssnr(ILinkQualityInfo::UNKNOWN_INT)
    , mLteCqi(ILinkQualityInfo::UNKNOWN_INT)
{}

ECode MobileLinkQualityInfo::CreateFromParcelBody(
    /* [in] */ IParcel* in,
    /* [out] */ IMobileLinkQualityInfo** result)
{
    AutoPtr<MobileLinkQualityInfo> li = new MobileLinkQualityInfo();
    li->ReadFromParcel(in);
    *result = li;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetMobileNetworkType(
    /* [out] */ Int32* result)
{
    *result = mMobileNetworkType;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetMobileNetworkType(
    /* [in] */ Int32 mobileNetworkType)
{
    mMobileNetworkType = mobileNetworkType;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetRssi(
    /* [out] */ Int32* result)
{
    *result = mRssi;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetRssi(
    /* [in] */ Int32 Rssi)
{
    mRssi = Rssi;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetGsmErrorRate(
    /* [out] */ Int32* result)
{
    *result = mGsmErrorRate;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetGsmErrorRate(
    /* [in] */ Int32 gsmErrorRate)
{
    mGsmErrorRate = gsmErrorRate;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetCdmaDbm(
    /* [out] */ Int32* result)
{
    *result = mCdmaDbm;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetCdmaDbm(
    /* [in] */ Int32 cdmaDbm)
{
    mCdmaDbm = cdmaDbm;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetCdmaEcio(
    /* [out] */ Int32* result)
{
    *result = mCdmaEcio;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetCdmaEcio(
    /* [in] */ Int32 cdmaEcio)
{
    mCdmaEcio = cdmaEcio;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetEvdoDbm(
    /* [out] */ Int32* result)
{
    *result = mEvdoDbm;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetEvdoDbm(
    /* [in] */ Int32 evdoDbm)
{
    mEvdoDbm = evdoDbm;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetEvdoEcio(
    /* [out] */ Int32* result)
{
    *result = mEvdoEcio;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetEvdoEcio(
    /* [in] */ Int32 evdoEcio)
{
    mEvdoEcio = evdoEcio;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetEvdoSnr(
    /* [out] */ Int32* result)
{
    *result = mEvdoSnr;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetEvdoSnr(
    /* [in] */ Int32 evdoSnr)
{
    mEvdoSnr = evdoSnr;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetLteSignalStrength(
    /* [out] */ Int32* result)
{
    *result = mLteSignalStrength;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetLteSignalStrength(
    /* [in] */ Int32 lteSignalStrength)
{
    mLteSignalStrength = lteSignalStrength;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetLteRsrp(
    /* [out] */ Int32* result)
{
    *result = mLteRsrp;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetLteRsrp(
    /* [in] */ Int32 lteRsrp)
{
    mLteRsrp = lteRsrp;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetLteRsrq(
    /* [out] */ Int32* result)
{
    *result = mLteRsrq;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetLteRsrq(
    /* [in] */ Int32 lteRsrq)
{
    mLteRsrq = lteRsrq;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetLteRssnr(
    /* [out] */ Int32* result)
{
    *result = mLteRssnr;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetLteRssnr(
    /* [in] */ Int32 lteRssnr)
{
    mLteRssnr = lteRssnr;
    return NOERROR;
}

ECode MobileLinkQualityInfo::GetLteCqi(
    /* [out] */ Int32* result)
{
    *result = mLteCqi;
    return NOERROR;
}

ECode MobileLinkQualityInfo::SetLteCqi(
    /* [in] */ Int32 lteCqi)
{
    mLteCqi = lteCqi;
    return NOERROR;
}

ECode MobileLinkQualityInfo::ReadFromParcel(
        /* [in] */ IParcel* in)
{
    LinkQualityInfo::ReadFromParcel(in);
    in->ReadInt32(&(mMobileNetworkType));
    in->ReadInt32(&(mRssi));
    in->ReadInt32(&(mGsmErrorRate));
    in->ReadInt32(&(mCdmaDbm));
    in->ReadInt32(&(mCdmaEcio));
    in->ReadInt32(&(mEvdoDbm));
    in->ReadInt32(&(mEvdoEcio));
    in->ReadInt32(&(mEvdoSnr));
    in->ReadInt32(&(mLteSignalStrength));
    in->ReadInt32(&(mLteRsrp));
    in->ReadInt32(&(mLteRsrq));
    in->ReadInt32(&(mLteRssnr));
    in->ReadInt32(&(mLteCqi));
    return NOERROR;
}

ECode MobileLinkQualityInfo::WriteToParcel(
        /* [in] */ IParcel* dest)
{
    LinkQualityInfo::WriteToParcel(dest);
    dest->WriteInt32(mMobileNetworkType);
    dest->WriteInt32(mRssi);
    dest->WriteInt32(mGsmErrorRate);
    dest->WriteInt32(mCdmaDbm);
    dest->WriteInt32(mCdmaEcio);
    dest->WriteInt32(mEvdoDbm);
    dest->WriteInt32(mEvdoEcio);
    dest->WriteInt32(mEvdoSnr);
    dest->WriteInt32(mLteSignalStrength);
    dest->WriteInt32(mLteRsrp);
    dest->WriteInt32(mLteRsrq);
    dest->WriteInt32(mLteRssnr);
    dest->WriteInt32(mLteCqi);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
