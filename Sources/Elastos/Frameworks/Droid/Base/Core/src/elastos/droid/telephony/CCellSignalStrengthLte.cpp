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

#include "elastos/droid/telephony/CCellSignalStrengthLte.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellSignalStrengthLte::TAG("CellSignalStrengthLte");
const Boolean CCellSignalStrengthLte::DBG = FALSE;

CAR_INTERFACE_IMPL_2(CCellSignalStrengthLte, CellSignalStrength, ICellSignalStrengthLte, IParcelable)

CAR_OBJECT_IMPL(CCellSignalStrengthLte)

CCellSignalStrengthLte::CCellSignalStrengthLte()
    : mSignalStrength(0)
    , mRsrp(0)
    , mRsrq(0)
    , mRssnr(0)
    , mCqi(0)
    , mTimingAdvance(0)
{
}

CCellSignalStrengthLte::~CCellSignalStrengthLte()
{
}

ECode CCellSignalStrengthLte::constructor()
{
    return SetDefaultValues();
}

ECode CCellSignalStrengthLte::constructor(
    /* [in] */ Int32 signalStrength,
    /* [in] */ Int32 rsrp,
    /* [in] */ Int32 rsrq,
    /* [in] */ Int32 rssnr,
    /* [in] */ Int32 cqi,
    /* [in] */ Int32 timingAdvance)
{
    return Initialize(signalStrength, rsrp, rsrq, rssnr, cqi, timingAdvance);
}

ECode CCellSignalStrengthLte::constructor(
    /* [in] */ ICellSignalStrengthLte* css)
{
    return CopyFrom(css);
}

ECode CCellSignalStrengthLte::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mSignalStrength);
    source->ReadInt32(&mRsrp);
    source->ReadInt32(&mRsrq);
    source->ReadInt32(&mRssnr);
    source->ReadInt32(&mCqi);
    source->ReadInt32(&mTimingAdvance);
    return NOERROR;
}

ECode CCellSignalStrengthLte::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    String str;
    ToString(&str);
    if (DBG) Log(String("writeToParcel(Parcel, int): ") + str);
    dest->WriteInt32(mSignalStrength);
    dest->WriteInt32(mRsrp);
    dest->WriteInt32(mRsrq);
    dest->WriteInt32(mRssnr);
    dest->WriteInt32(mCqi);
    dest->WriteInt32(mTimingAdvance);
    return NOERROR;
}

ECode CCellSignalStrengthLte::SetDefaultValues()
{
    mSignalStrength = Elastos::Core::Math::INT32_MAX_VALUE;
    mRsrp = Elastos::Core::Math::INT32_MAX_VALUE;
    mRsrq = Elastos::Core::Math::INT32_MAX_VALUE;
    mRssnr = Elastos::Core::Math::INT32_MAX_VALUE;
    mCqi = Elastos::Core::Math::INT32_MAX_VALUE;
    mTimingAdvance = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    Int32 levelRsrp = 0;
    Int32 levelRssnr = 0;

    if (mRsrp == Elastos::Core::Math::INT32_MAX_VALUE) levelRsrp = 0;
    else if (mRsrp >= -95) levelRsrp = ICellSignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (mRsrp >= -105) levelRsrp = ICellSignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (mRsrp >= -115) levelRsrp = ICellSignalStrength::SIGNAL_STRENGTH_MODERATE;
    else levelRsrp = ICellSignalStrength::SIGNAL_STRENGTH_POOR;

    // See RIL_LTE_SignalStrength in ril.h
    if (mRssnr == Elastos::Core::Math::INT32_MAX_VALUE) levelRssnr = 0;
    else if (mRssnr >= 45) levelRssnr = ICellSignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (mRssnr >= 10) levelRssnr = ICellSignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (mRssnr >= -30) levelRssnr = ICellSignalStrength::SIGNAL_STRENGTH_MODERATE;
    else levelRssnr = ICellSignalStrength::SIGNAL_STRENGTH_POOR;

    if (mRsrp == Elastos::Core::Math::INT32_MAX_VALUE)
        *level = levelRssnr;
    else if (mRssnr == Elastos::Core::Math::INT32_MAX_VALUE)
        *level = levelRsrp;
    else
        *level = (levelRssnr < levelRsrp) ? levelRssnr : levelRsrp;

    if (DBG) {
        StringBuilder sb;
        sb.Append("Lte rsrp level: ");
        sb.Append(levelRsrp);
        sb.Append(" snr level:");
        sb.Append(levelRssnr);
        sb.Append(" level: ");
        sb.Append(*level);

        Log(sb.ToString());
    }
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetAsuLevel(
    /* [out] */ Int32* asuLevel)
{
    VALIDATE_NOT_NULL(asuLevel);
    Int32 lteAsuLevel = 99;
    Int32 lteDbm;
    GetDbm(&lteDbm);
    if (lteDbm <= -140) lteAsuLevel = 0;
    else if (lteDbm >= -43) lteAsuLevel = 97;
    else lteAsuLevel = lteDbm + 140;
    if (DBG) Log(String("Lte Asu level: ") + StringUtils::ToString(lteAsuLevel));
    *asuLevel = lteAsuLevel;
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetDbm(
    /* [out] */ Int32* dbm)
{
    VALIDATE_NOT_NULL(dbm);
    *dbm = mRsrp;
    return NOERROR;
}

ECode CCellSignalStrengthLte::Copy(
    /* [out] */ ICellSignalStrength** cssReturn)
{
    VALIDATE_NOT_NULL(cssReturn);
    AutoPtr<ICellSignalStrengthLte> css;
    CCellSignalStrengthLte::New(this, (ICellSignalStrengthLte**)&css);
    *cssReturn = ICellSignalStrength::Probe(css);
    REFCOUNT_ADD(*cssReturn);
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 primeNum = 31;
    *hashCode = (mSignalStrength * primeNum) + (mRsrp * primeNum)
            + (mRsrq * primeNum) + (mRssnr * primeNum) + (mCqi * primeNum)
            + (mTimingAdvance * primeNum);
    return NOERROR;
}

ECode CCellSignalStrengthLte::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<CCellSignalStrengthLte> s;

    //try {
    s = (CCellSignalStrengthLte*)ICellSignalStrengthLte::Probe(o);
    //} catch (ClassCastException ex) {
    //    return false;
    //}

    if (o == NULL) {
        *res = FALSE;
    }

    Int32 signalStrength;
    s->GetSignalStrength(&signalStrength);
    Int32 rsrp;
    s->GetRsrp(&rsrp);
    Int32 rsrq;
    s->GetRsrq(&rsrq);
    Int32 rssnr;
    s->GetRssnr(&rssnr);
    Int32 cqi;
    s->GetCqi(&cqi);
    Int32 timingAdvance;
    s->GetTimingAdvance(&timingAdvance);

    *res =  (mSignalStrength == signalStrength
            && mRsrp == rsrp
            && mRsrq == rsrq
            && mRssnr == rssnr
            && mCqi == cqi
            && mTimingAdvance == timingAdvance);

    return NOERROR;
}

ECode CCellSignalStrengthLte::Initialize(
    /* [in] */ Int32 lteSignalStrength,
    /* [in] */ Int32 rsrp,
    /* [in] */ Int32 rsrq,
    /* [in] */ Int32 rssnr,
    /* [in] */ Int32 cqi,
    /* [in] */ Int32 timingAdvance)
{
    mSignalStrength = lteSignalStrength;
    mRsrp = rsrp;
    mRsrq = rsrq;
    mRssnr = rssnr;
    mCqi = cqi;
    mTimingAdvance = timingAdvance;
    return NOERROR;
}

ECode CCellSignalStrengthLte::Initialize(
    /* [in] */ ISignalStrength* ss,
    /* [in] */ Int32 timingAdvance)
{
    Int32 lteSignalStrenght; ss->GetLteSignalStrength(&lteSignalStrenght);
    Int32 lteRsrp; ss->GetLteRsrp(&lteRsrp);
    Int32 lteRsrq; ss->GetLteRsrq(&lteRsrq);
    Int32 lteRssnr; ss->GetLteRssnr(&lteRssnr);
    Int32 lteCqi; ss->GetLteCqi(&lteCqi);

    mSignalStrength = lteSignalStrenght;
    mRsrp = lteRsrp;
    mRsrq = lteRsrq;
    mRssnr = lteRssnr;
    mCqi = lteCqi;
    mTimingAdvance = timingAdvance;

    return NOERROR;
}

ECode CCellSignalStrengthLte::Copy(
    /* [out] */ ICellSignalStrengthLte** css)
{
    VALIDATE_NOT_NULL(css);
    CCellSignalStrengthLte::New(this, css);
    return NOERROR;
}

ECode CCellSignalStrengthLte::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb;
    sb.Append("CellSignalStrengthLte:");
    sb.Append(" ss="); sb.Append(mSignalStrength);
    sb.Append(" rsrp="); sb.Append(mRsrp);
    sb.Append(" rsrq="); sb.Append(mRsrq);
    sb.Append(" rssnr="); sb.Append(mRssnr);
    sb.Append(" cqi="); sb.Append(mCqi);
    sb.Append(" ta="); sb.Append(mTimingAdvance);
    *str = sb.ToString();
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetSignalStrength(
    /* [out] */ Int32* signalStrength)
{
    VALIDATE_NOT_NULL(signalStrength);
    *signalStrength = mSignalStrength;
    return NOERROR;
}

ECode CCellSignalStrengthLte::SetSignalStrength(
    /* [in] */ Int32 signalStrength)
{
    mSignalStrength = signalStrength;
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetRsrp(
    /* [out] */ Int32* rsrp)
{
    VALIDATE_NOT_NULL(rsrp);
    *rsrp = mRsrp;
    return NOERROR;
}

ECode CCellSignalStrengthLte::SetRsrp(
    /* [in] */ Int32 rsrp)
{
    mRsrp = rsrp;
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetRsrq(
    /* [out] */ Int32* rsrq)
{
    VALIDATE_NOT_NULL(rsrq);
    *rsrq = mRsrq;
    return NOERROR;
}

ECode CCellSignalStrengthLte::SetRsrq(
    /* [in] */ Int32 rsrq)
{
    mRsrq = rsrq;
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetRssnr(
    /* [out] */ Int32* rssnr)
{
    VALIDATE_NOT_NULL(rssnr);
    *rssnr = mRssnr;
    return NOERROR;
}

ECode CCellSignalStrengthLte::SetRssnr(
    /* [in] */ Int32 rssnr)
{
    mRssnr = rssnr;
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetCqi(
    /* [out] */ Int32* cqi)
{
    VALIDATE_NOT_NULL(cqi);
    *cqi = mCqi;
    return NOERROR;
}

ECode CCellSignalStrengthLte::SetCqi(
    /* [in] */ Int32 cqi)
{
    mCqi = cqi;
    return NOERROR;
}

ECode CCellSignalStrengthLte::GetTimingAdvance(
    /* [out] */ Int32* timingAdvance)
{
    VALIDATE_NOT_NULL(timingAdvance);
    *timingAdvance = mTimingAdvance;
    return NOERROR;
}

ECode CCellSignalStrengthLte::SetTimingAdvance(
    /* [in] */ Int32 timingAdvance)
{
    mTimingAdvance = timingAdvance;
    return NOERROR;
}

ECode CCellSignalStrengthLte::Log(
    /* [in] */ const String& s)
{
    Logger::W(TAG, s);
    return NOERROR;
}

ECode CCellSignalStrengthLte::CopyFrom(
    /* [in] */ ICellSignalStrengthLte* css)
{
    AutoPtr<CCellSignalStrengthLte> c = (CCellSignalStrengthLte*)css;
    c->GetSignalStrength(&mSignalStrength);
    c->GetRsrp(&mRsrp);
    c->GetRsrq(&mRsrq);
    c->GetRssnr(&mRssnr);
    c->GetCqi(&mCqi);
    c->GetTimingAdvance(&mTimingAdvance);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

