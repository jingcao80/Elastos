
#include "elastos/droid/telephony/CCellSignalStrengthWcdma.h"
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

const String CCellSignalStrengthWcdma::TAG("CellSignalStrengthWcdma");
const Boolean CCellSignalStrengthWcdma::DBG = FALSE;

const Int32 CCellSignalStrengthWcdma::WCDMA_SIGNAL_STRENGTH_GREAT = 12;
const Int32 CCellSignalStrengthWcdma::WCDMA_SIGNAL_STRENGTH_GOOD = 8;
const Int32 CCellSignalStrengthWcdma::WCDMA_SIGNAL_STRENGTH_MODERATE = 5;

CAR_INTERFACE_IMPL_2(CCellSignalStrengthWcdma, CellSignalStrength, ICellSignalStrengthWcdma, IParcelable)

CAR_OBJECT_IMPL(CCellSignalStrengthWcdma)

CCellSignalStrengthWcdma::CCellSignalStrengthWcdma()
    : mSignalStrength(0)
    , mBitErrorRate(0)
{
}

CCellSignalStrengthWcdma::~CCellSignalStrengthWcdma()
{
}

ECode CCellSignalStrengthWcdma::constructor()
{
    return SetDefaultValues();
}

ECode CCellSignalStrengthWcdma::constructor(
    /* [in] */ Int32 ss,
    /* [in] */ Int32 ber)
{
    return Initialize(ss, ber);
}

ECode CCellSignalStrengthWcdma::constructor(
    /* [in] */ ICellSignalStrengthWcdma* css)
{
    return CopyFrom(css);
}

ECode CCellSignalStrengthWcdma::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mSignalStrength);
    source->ReadInt32(&mBitErrorRate);
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    String str;
    ToString(&str);
    if (DBG) Log(String("writeToParcel(Parcel, int): ") + str);
    dest->WriteInt32(mSignalStrength);
    dest->WriteInt32(mBitErrorRate);
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::Initialize(
    /* [in] */ Int32 ss,
    /* [in] */ Int32 ber)
{
    mSignalStrength = ss;
    mBitErrorRate = ber;
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::Copy(
    /* [out] */ ICellSignalStrengthWcdma** css)
{
    VALIDATE_NOT_NULL(css);
    CCellSignalStrengthWcdma::New(this, css);
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::SetDefaultValues()
{
    mSignalStrength = Elastos::Core::Math::INT32_MAX_VALUE;
    mBitErrorRate = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::GetLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);

    // ASU ranges from 0 to 31 - TS 27.007 Sec 8.5
    // asu = 0 (-113dB or less) is very weak
    // signal, its better to show 0 bars to the user in such cases.
    // asu = 99 is a special case, where the signal strength is unknown.
    Int32 asu = mSignalStrength;
    if (asu <= 2 || asu == 99) *level = ICellSignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;
    else if (asu >= WCDMA_SIGNAL_STRENGTH_GREAT) *level = ICellSignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (asu >= WCDMA_SIGNAL_STRENGTH_GOOD)  *level = ICellSignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (asu >= WCDMA_SIGNAL_STRENGTH_MODERATE)  *level = ICellSignalStrength::SIGNAL_STRENGTH_MODERATE;
    else *level = ICellSignalStrength::SIGNAL_STRENGTH_POOR;
    if (DBG) Log(String("getLevel=") + StringUtils::ToString(*level));

    return NOERROR;
}

ECode CCellSignalStrengthWcdma::GetAsuLevel(
    /* [out] */ Int32* asuLevel)
{
    VALIDATE_NOT_NULL(asuLevel);

    // ASU ranges from 0 to 31 - TS 27.007 Sec 8.5
    // asu = 0 (-113dB or less) is very weak
    // signal, its better to show 0 bars to the user in such cases.
    // asu = 99 is a special case, where the signal strength is unknown.
    *asuLevel = mSignalStrength;
    if (DBG) Log(String("getAsuLevel=") + StringUtils::ToString(*asuLevel));
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::GetDbm(
    /* [out] */ Int32* dbm)
{
    VALIDATE_NOT_NULL(dbm);

    Int32 level = mSignalStrength;
    Int32 asu = (level == 99 ? Elastos::Core::Math::INT32_MAX_VALUE : level);
    if (asu != Elastos::Core::Math::INT32_MAX_VALUE) {
        *dbm = -113 + (2 * asu);
    }
    else {
        *dbm = Elastos::Core::Math::INT32_MAX_VALUE;
    }
    if (DBG) Log(String("getDbm=") + StringUtils::ToString(*dbm));
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 primeNum = 31;
    *hashCode = (mSignalStrength * primeNum) + (mBitErrorRate * primeNum);
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<CCellSignalStrengthWcdma> s;

    //try {
    s = (CCellSignalStrengthWcdma*)ICellSignalStrengthWcdma::Probe(o);
    //} catch (ClassCastException ex) {
    //    return false;
    //}

    if (o == NULL) {
        *res = FALSE;
    }

    Int32 signalStrength;
    s->GetSignalStrength(&signalStrength);
    Int32 ber;
    s->GetBitErrorRate(&ber);

    *res = (mSignalStrength == signalStrength
            && mBitErrorRate == ber);

    return NOERROR;
}

ECode CCellSignalStrengthWcdma::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb;
    sb.Append("CellSignalStrengthWcdma:");
    sb.Append(" ss="); sb.Append(mSignalStrength);
    sb.Append(" ber="); sb.Append(mBitErrorRate);
    *str = sb.ToString();
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::GetSignalStrength(
    /* [out] */ Int32* signalStrength)
{
    VALIDATE_NOT_NULL(signalStrength);
    *signalStrength = mSignalStrength;
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::SetSignalStrength(
    /* [in] */ Int32 signalStrength)
{
    mSignalStrength = signalStrength;
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::GetBitErrorRate(
    /* [out] */ Int32* bitErrorRate)
{
    VALIDATE_NOT_NULL(bitErrorRate);
    *bitErrorRate = mBitErrorRate;
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::SetBitErrorRate(
    /* [in] */ Int32 bitErrorRate)
{
    mBitErrorRate = bitErrorRate;
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::Log(
    /* [in] */ const String& s)
{
    Logger::W(TAG, s);
    return NOERROR;
}

ECode CCellSignalStrengthWcdma::CopyFrom(
    /* [in] */ ICellSignalStrengthWcdma* css)
{
    AutoPtr<CCellSignalStrengthWcdma> c = (CCellSignalStrengthWcdma*)css;
    c->GetSignalStrength(&mSignalStrength);
    c->GetBitErrorRate(&mBitErrorRate);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

