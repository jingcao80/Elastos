
#include "elastos/droid/telephony/CCellSignalStrengthCdma.h"
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

const String CCellSignalStrengthCdma::TAG("CellSignalStrengthCdma");
const Boolean CCellSignalStrengthCdma::DBG = FALSE;

CAR_INTERFACE_IMPL_2(CCellSignalStrengthCdma, CellSignalStrength, ICellSignalStrengthCdma, IParcelable)

CAR_OBJECT_IMPL(CCellSignalStrengthCdma)

CCellSignalStrengthCdma::CCellSignalStrengthCdma()
    : mCdmaDbm(0)
    , mCdmaEcio(0)
    , mEvdoDbm(0)
    , mEvdoEcio(0)
    , mEvdoSnr(0)
{
}

CCellSignalStrengthCdma::~CCellSignalStrengthCdma()
{
}

ECode CCellSignalStrengthCdma::constructor()
{
    return SetDefaultValues();
}

ECode CCellSignalStrengthCdma::constructor(
    /* [in] */ Int32 cdmaDbm,
    /* [in] */ Int32 cdmaEcio,
    /* [in] */ Int32 evdoDbm,
    /* [in] */ Int32 evdoEcio,
    /* [in] */ Int32 evdoSnr)
{
    return Initialize(cdmaDbm, cdmaEcio, evdoDbm, evdoEcio, evdoSnr);;
}

ECode CCellSignalStrengthCdma::constructor(
    /* [in] */ ICellSignalStrengthCdma* css)
{
    return CopyFrom(css);
}

ECode CCellSignalStrengthCdma::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mCdmaDbm);
    source->ReadInt32(&mCdmaEcio);
    source->ReadInt32(&mEvdoDbm);
    source->ReadInt32(&mEvdoEcio);
    source->ReadInt32(&mEvdoSnr);
    return NOERROR;
}

ECode CCellSignalStrengthCdma::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (DBG) {
        String str;
        ToString(&str);
        Log(String("writeToParcel(Parcel, int): ") + str);
    }
    dest->WriteInt32(mCdmaDbm);
    dest->WriteInt32(mCdmaEcio);
    dest->WriteInt32(mEvdoDbm);
    dest->WriteInt32(mEvdoEcio);
    dest->WriteInt32(mEvdoSnr);
    return NOERROR;
}

ECode CCellSignalStrengthCdma::SetDefaultValues()
{
    mCdmaDbm = Elastos::Core::Math::INT32_MAX_VALUE;
    mCdmaEcio = Elastos::Core::Math::INT32_MAX_VALUE;
    mEvdoDbm = Elastos::Core::Math::INT32_MAX_VALUE;
    mEvdoEcio = Elastos::Core::Math::INT32_MAX_VALUE;
    mEvdoSnr = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    Int32 cdmaLevel, evdoLevel;
    GetCdmaLevel(&cdmaLevel);
    GetEvdoLevel(&evdoLevel);
    if (evdoLevel == ICellSignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
        /* We don't know evdo, use cdma */
        GetCdmaLevel(level);
    }
    else if (cdmaLevel == ICellSignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
        /* We don't know cdma, use evdo */
        GetEvdoLevel(level);
    }
    else {
        /* We know both, use the lowest level */
        *level = cdmaLevel < evdoLevel ? cdmaLevel : evdoLevel;
    }
    if (DBG) Log(String("getLevel=") + StringUtils::ToString(*level));
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetAsuLevel(
    /* [out] */ Int32* asuLevel)
{
    VALIDATE_NOT_NULL(asuLevel);
    Int32 cdmaDbm;
    GetCdmaDbm(&cdmaDbm);
    Int32 cdmaEcio;
    GetCdmaEcio(&cdmaEcio);
    Int32 cdmaAsuLevel;
    Int32 ecioAsuLevel;

    if (cdmaDbm >= -75) cdmaAsuLevel = 16;
    else if (cdmaDbm >= -82) cdmaAsuLevel = 8;
    else if (cdmaDbm >= -90) cdmaAsuLevel = 4;
    else if (cdmaDbm >= -95) cdmaAsuLevel = 2;
    else if (cdmaDbm >= -100) cdmaAsuLevel = 1;
    else cdmaAsuLevel = 99;

    // Ec/Io are in dB*10
    if (cdmaEcio >= -90) ecioAsuLevel = 16;
    else if (cdmaEcio >= -100) ecioAsuLevel = 8;
    else if (cdmaEcio >= -115) ecioAsuLevel = 4;
    else if (cdmaEcio >= -130) ecioAsuLevel = 2;
    else if (cdmaEcio >= -150) ecioAsuLevel = 1;
    else ecioAsuLevel = 99;

    Int32 level = (cdmaAsuLevel < ecioAsuLevel) ? cdmaAsuLevel : ecioAsuLevel;
    if (DBG) Log(String("getAsuLevel=") + StringUtils::ToString(level));
    *asuLevel = level;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetDbm(
    /* [out] */ Int32* dbm)
{
    VALIDATE_NOT_NULL(dbm);
    Int32 cdmaDbm;
    GetCdmaDbm(&cdmaDbm);
    Int32 evdoDbm;
    GetEvdoDbm(&evdoDbm);

    // Use the lower value to be conservative
    *dbm = (cdmaDbm < evdoDbm) ? cdmaDbm : evdoDbm;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 primeNum = 31;
    *hashCode = ((mCdmaDbm * primeNum) + (mCdmaEcio * primeNum)
            + (mEvdoDbm * primeNum) + (mEvdoEcio * primeNum) + (mEvdoSnr * primeNum));

    return NOERROR;
}

ECode CCellSignalStrengthCdma::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<ICellSignalStrengthCdma> s;

    //try {
    s = ICellSignalStrengthCdma::Probe(o);
    if (s == NULL) {
        *res = FALSE;
        return NOERROR;
    }
    // } catch (ClassCastException ex) {
    //     return false;
    // }

    if (o == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 cdmaDbm, cdmaEcio, evdoDbm, evdoEcio, evdoSnr;
    s->GetCdmaDbm(&cdmaDbm);
    s->GetCdmaEcio(&cdmaEcio);
    s->GetEvdoDbm(&evdoDbm);
    s->GetEvdoEcio(&evdoEcio);
    s->GetEvdoSnr(&evdoSnr);
    *res = (mCdmaDbm == cdmaDbm
            && mCdmaEcio == cdmaEcio
            && mEvdoDbm == evdoDbm
            && mEvdoEcio == evdoEcio
            && mEvdoSnr == evdoSnr);
    return NOERROR;
}

ECode CCellSignalStrengthCdma::Initialize(
    /* [in] */ Int32 cdmaDbm,
    /* [in] */ Int32 cdmaEcio,
    /* [in] */ Int32 evdoDbm,
    /* [in] */ Int32 evdoEcio,
    /* [in] */ Int32 evdoSnr)
{
    mCdmaDbm = cdmaDbm;
    mCdmaEcio = cdmaEcio;
    mEvdoDbm = evdoDbm;
    mEvdoEcio = evdoEcio;
    mEvdoSnr = evdoSnr;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::Copy(
    /* [out] */ ICellSignalStrength** css)
{
    VALIDATE_NOT_NULL(css);
    AutoPtr<ICellSignalStrengthCdma> cssTmp;
    CCellSignalStrengthCdma::New(this, (ICellSignalStrengthCdma**)&cssTmp);
    *css = ICellSignalStrength::Probe(cssTmp);
    REFCOUNT_ADD(*css);
    return NOERROR;
}


ECode CCellSignalStrengthCdma::Copy(
    /* [out] */ ICellSignalStrengthCdma** css)
{
    VALIDATE_NOT_NULL(css);
    CCellSignalStrengthCdma::New(this, css);
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetCdmaLevel(
    /* [out] */ Int32* cdmaLevel)
{
    VALIDATE_NOT_NULL(cdmaLevel);
    Int32 cdmaDbm;
    GetCdmaDbm(&cdmaDbm);
    Int32 cdmaEcio;
    GetCdmaEcio(&cdmaEcio);
    Int32 levelDbm;
    Int32 levelEcio;

    if (cdmaDbm >= -75) levelDbm = SIGNAL_STRENGTH_GREAT;
    else if (cdmaDbm >= -85) levelDbm = SIGNAL_STRENGTH_GOOD;
    else if (cdmaDbm >= -95) levelDbm = SIGNAL_STRENGTH_MODERATE;
    else if (cdmaDbm >= -100) levelDbm = SIGNAL_STRENGTH_POOR;
    else levelDbm = SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    // Ec/Io are in dB*10
    if (cdmaEcio >= -90) levelEcio = SIGNAL_STRENGTH_GREAT;
    else if (cdmaEcio >= -110) levelEcio = SIGNAL_STRENGTH_GOOD;
    else if (cdmaEcio >= -130) levelEcio = SIGNAL_STRENGTH_MODERATE;
    else if (cdmaEcio >= -150) levelEcio = SIGNAL_STRENGTH_POOR;
    else levelEcio = SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    Int32 level = (levelDbm < levelEcio) ? levelDbm : levelEcio;
    if (DBG) Log(String("getCdmaLevel=") + StringUtils::ToString(level));
    *cdmaLevel = level;

    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetEvdoLevel(
    /* [out] */ Int32* evdoLevel)
{
    VALIDATE_NOT_NULL(evdoLevel);
    Int32 evdoDbm;
    GetEvdoDbm(&evdoDbm);
    Int32 evdoSnr;
    GetEvdoSnr(&evdoSnr);
    Int32 levelEvdoDbm;
    Int32 levelEvdoSnr;

    if (evdoDbm >= -65) levelEvdoDbm = SIGNAL_STRENGTH_GREAT;
    else if (evdoDbm >= -75) levelEvdoDbm = SIGNAL_STRENGTH_GOOD;
    else if (evdoDbm >= -90) levelEvdoDbm = SIGNAL_STRENGTH_MODERATE;
    else if (evdoDbm >= -105) levelEvdoDbm = SIGNAL_STRENGTH_POOR;
    else levelEvdoDbm = SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    if (evdoSnr >= 7) levelEvdoSnr = SIGNAL_STRENGTH_GREAT;
    else if (evdoSnr >= 5) levelEvdoSnr = SIGNAL_STRENGTH_GOOD;
    else if (evdoSnr >= 3) levelEvdoSnr = SIGNAL_STRENGTH_MODERATE;
    else if (evdoSnr >= 1) levelEvdoSnr = SIGNAL_STRENGTH_POOR;
    else levelEvdoSnr = SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    Int32 level = (levelEvdoDbm < levelEvdoSnr) ? levelEvdoDbm : levelEvdoSnr;
    if (DBG) Log(String("getEvdoLevel=") + StringUtils::ToString(level));
    *evdoLevel = level;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetCdmaDbm(
    /* [out] */ Int32* cdmaDbm)
{
    VALIDATE_NOT_NULL(cdmaDbm);
    *cdmaDbm = mCdmaDbm;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::SetCdmaDbm(
    /* [in] */ Int32 cdmaDbm)
{
    mCdmaDbm = cdmaDbm;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetCdmaEcio(
    /* [out] */ Int32* cdmaEcio)
{
    VALIDATE_NOT_NULL(cdmaEcio);
    *cdmaEcio = mCdmaEcio;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::SetCdmaEcio(
    /* [in] */ Int32 cdmaEcio)
{
    mCdmaEcio = cdmaEcio;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetEvdoDbm(
    /* [out] */ Int32* evdoDbm)
{
    VALIDATE_NOT_NULL(evdoDbm);
    *evdoDbm = mEvdoDbm;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::SetEvdoDbm(
    /* [in] */ Int32 evdoDbm)
{
    mEvdoDbm = evdoDbm;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetEvdoEcio(
    /* [out] */ Int32* evdoEcio)
{
    VALIDATE_NOT_NULL(evdoEcio);
    *evdoEcio = mEvdoEcio;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::SetEvdoEcio(
    /* [in] */ Int32 evdoEcio)
{
    mEvdoEcio = evdoEcio;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::GetEvdoSnr(
    /* [out] */ Int32* evdoSnr)
{
    VALIDATE_NOT_NULL(evdoSnr);
    *evdoSnr = mEvdoSnr;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::SetEvdoSnr(
    /* [in] */ Int32 evdoSnr)
{
    mEvdoSnr = evdoSnr;
    return NOERROR;
}

ECode CCellSignalStrengthCdma::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("CellSignalStrengthCdma:");
    sb.Append(" cdmaDbm=:");
    sb.Append(mCdmaDbm);
    sb.Append(" cdmaEcio=:");
    sb.Append(mCdmaEcio);
    sb.Append(" evdoDbm=:");
    sb.Append(mEvdoDbm);
    sb.Append(" evdoEcio=:");
    sb.Append(mEvdoEcio);
    sb.Append(" evdoSnr=:");
    sb.Append(mEvdoSnr);
    *str = sb.ToString();
    return NOERROR;
}

ECode CCellSignalStrengthCdma::Log(
    /* [in] */ const String& s)
{
    Logger::W(TAG, s);
    return NOERROR;
}

ECode CCellSignalStrengthCdma::CopyFrom(
    /* [in] */ ICellSignalStrengthCdma* css)
{
    css->GetCdmaDbm(&mCdmaDbm);
    css->GetCdmaEcio(&mCdmaEcio);
    css->GetEvdoDbm(&mEvdoDbm);
    css->GetEvdoEcio(&mEvdoEcio);
    css->GetEvdoSnr(&mEvdoSnr);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
