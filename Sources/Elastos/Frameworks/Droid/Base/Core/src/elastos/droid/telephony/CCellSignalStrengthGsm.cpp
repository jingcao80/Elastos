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

#include "elastos/droid/telephony/CCellSignalStrengthGsm.h"
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

const String CCellSignalStrengthGsm::TAG("CCellSignalStrengthGsm");
const Boolean CCellSignalStrengthGsm::DBG = FALSE;

const Int32 CCellSignalStrengthGsm::GSM_SIGNAL_STRENGTH_GREAT = 12;
const Int32 CCellSignalStrengthGsm::GSM_SIGNAL_STRENGTH_GOOD = 8;
const Int32 CCellSignalStrengthGsm::GSM_SIGNAL_STRENGTH_MODERATE = 5;

CAR_INTERFACE_IMPL_2(CCellSignalStrengthGsm, CellSignalStrength, ICellSignalStrengthGsm, IParcelable)

CAR_OBJECT_IMPL(CCellSignalStrengthGsm)

CCellSignalStrengthGsm::CCellSignalStrengthGsm()
    : mSignalStrength(0)
    , mBitErrorRate(0)
{
}

CCellSignalStrengthGsm::~CCellSignalStrengthGsm()
{
}

ECode CCellSignalStrengthGsm::constructor()
{
    return SetDefaultValues();
}

ECode CCellSignalStrengthGsm::constructor(
    /* [in] */ Int32 ss,
    /* [in] */ Int32 ber)
{
    return Initialize(ss, ber);
}

ECode CCellSignalStrengthGsm::constructor(
    /* [in] */ ICellSignalStrengthGsm* css)
{
    return CopyFrom(css);
}

ECode CCellSignalStrengthGsm::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mSignalStrength);
    source->ReadInt32(&mBitErrorRate);
    return NOERROR;
}

ECode CCellSignalStrengthGsm::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    String str;
    ToString(&str);
    if (DBG) Log(String("writeToParcel(Parcel, int): ") + str);
    dest->WriteInt32(mSignalStrength);
    dest->WriteInt32(mBitErrorRate);
    return NOERROR;
}

ECode CCellSignalStrengthGsm::SetDefaultValues()
{
    mSignalStrength = Elastos::Core::Math::INT32_MAX_VALUE;
    mBitErrorRate = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

ECode CCellSignalStrengthGsm::GetLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);

    // ASU ranges from 0 to 31 - TS 27.007 Sec 8.5
    // asu = 0 (-113dB or less) is very weak
    // signal, its better to show 0 bars to the user in such cases.
    // asu = 99 is a special case, where the signal strength is unknown.
    Int32 asu = mSignalStrength;
    if (asu <= 2 || asu == 99) *level = SIGNAL_STRENGTH_NONE_OR_UNKNOWN;
    else if (asu >= GSM_SIGNAL_STRENGTH_GREAT) *level = SIGNAL_STRENGTH_GREAT;
    else if (asu >= GSM_SIGNAL_STRENGTH_GOOD)  *level = SIGNAL_STRENGTH_GOOD;
    else if (asu >= GSM_SIGNAL_STRENGTH_MODERATE)  *level = SIGNAL_STRENGTH_MODERATE;
    else *level = SIGNAL_STRENGTH_POOR;
    if (DBG) Log(String("getLevel=") + StringUtils::ToString(*level));
    return NOERROR;
}

ECode CCellSignalStrengthGsm::GetAsuLevel(
    /* [out] */ Int32* asuLevel)
{
    VALIDATE_NOT_NULL(asuLevel);
    // ASU ranges from 0 to 31 - TS 27.007 Sec 8.5
    // asu = 0 (-113dB or less) is very weak
    // signal, its better to show 0 bars to the user in such cases.
    // asu = 99 is a special case, where the signal strength is unknown.
    Int32 level = mSignalStrength;
    if (DBG) Log(String("getAsuLevel=") + StringUtils::ToString(level));
    *asuLevel = level;
    return NOERROR;
}

ECode CCellSignalStrengthGsm::GetDbm(
    /* [out] */ Int32* dbm)
{
    VALIDATE_NOT_NULL(dbm);
    Int32 level = mSignalStrength;
    Int32 asu = (level == 99 ? Elastos::Core::Math::INT32_MAX_VALUE : level);
    if (asu != Elastos::Core::Math::INT32_MAX_VALUE) {
        *dbm = -113 + (2 * asu);
    } else {
        *dbm = Elastos::Core::Math::INT32_MAX_VALUE;
    }
    if (DBG) Log(String("getDbm=") + StringUtils::ToString(*dbm));
    return NOERROR;
}

ECode CCellSignalStrengthGsm::Copy(
    /* [out] */ ICellSignalStrength** cssReturn)
{
    VALIDATE_NOT_NULL(cssReturn);
    AutoPtr<ICellSignalStrengthGsm> css;
    CCellSignalStrengthGsm::New(this, (ICellSignalStrengthGsm**)&css);
    *cssReturn = ICellSignalStrength::Probe(css);
    REFCOUNT_ADD(*cssReturn);
    return NOERROR;
}

ECode CCellSignalStrengthGsm::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 primeNum = 31;
    *hashCode = (mSignalStrength * primeNum) + (mBitErrorRate * primeNum);
    return NOERROR;
}

ECode CCellSignalStrengthGsm::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<CCellSignalStrengthGsm> s;

    //try {
    s = (CCellSignalStrengthGsm*)ICellSignalStrengthGsm::Probe(o);
    // } catch (ClassCastException ex) {
    //     return FALSE;
    // }

    if (o == NULL) {
        *res = FALSE;
        return NOERROR;
    }
    Int32 signalStrength, bitErrorRate;
    s->GetSignalStrength(&signalStrength);
    s->GetBitErrorRate(&bitErrorRate);
    *res = (mSignalStrength == signalStrength && mBitErrorRate == bitErrorRate);
    return NOERROR;
}

ECode CCellSignalStrengthGsm::Initialize(
    /* [in] */ Int32 ss,
    /* [in] */ Int32 ber)
{
    mSignalStrength = ss;
    mBitErrorRate = ber;
    return NOERROR;
}

ECode CCellSignalStrengthGsm::Copy(
    /* [out] */ ICellSignalStrengthGsm** css)
{
    VALIDATE_NOT_NULL(css);
    CCellSignalStrengthGsm::New(this, css);
    return NOERROR;
}

ECode CCellSignalStrengthGsm::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;

    sb.Append("CellSignalStrengthGsm:");
    sb.Append(" ss="); sb.Append(mSignalStrength);
    sb.Append(" ber="); sb.Append(mBitErrorRate);
    *str = sb.ToString();
    return NOERROR;
}

ECode CCellSignalStrengthGsm::GetSignalStrength(
    /* [out] */ Int32* signalStrength)
{
    VALIDATE_NOT_NULL(signalStrength);
    *signalStrength = mSignalStrength;
    return NOERROR;
}

ECode CCellSignalStrengthGsm::SetSignalStrength(
    /* [in] */ Int32 signalStrength)
{
    mSignalStrength = signalStrength;
    return NOERROR;
}

ECode CCellSignalStrengthGsm::GetBitErrorRate(
    /* [out] */ Int32* bitErrorRate)
{
    VALIDATE_NOT_NULL(bitErrorRate);
    *bitErrorRate = mBitErrorRate;
    return NOERROR;
}

ECode CCellSignalStrengthGsm::SetBitErrorRate(
    /* [in] */ Int32 bitErrorRate)
{
    mBitErrorRate = bitErrorRate;
    return NOERROR;
}

ECode CCellSignalStrengthGsm::Log(
    /* [in] */ const String& s)
{
    Logger::W(TAG, s);
    return NOERROR;
}

ECode CCellSignalStrengthGsm::CopyFrom(
    /* [in] */ ICellSignalStrengthGsm* css)
{
    AutoPtr<CCellSignalStrengthGsm> s = (CCellSignalStrengthGsm*)css;

    s->GetSignalStrength(&mSignalStrength);
    s->GetBitErrorRate(&mBitErrorRate);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

