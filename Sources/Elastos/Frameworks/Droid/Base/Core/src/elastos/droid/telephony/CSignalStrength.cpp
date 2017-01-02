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

#include "elastos/droid/telephony/CSignalStrength.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Telephony {

static AutoPtr< ArrayOf<String> > InitSIGNAL_STRENGTH_NAMES() {
    AutoPtr< ArrayOf<String> > ssnames = ArrayOf<String>::Alloc(5);
    (*ssnames)[0] = String("none");
    (*ssnames)[1] = String("poor");
    (*ssnames)[2] = String("moderate");
    (*ssnames)[3] = String("good");
    (*ssnames)[4] = String("great");
    return ssnames;
}

const String CSignalStrength::TAG("CSignalStrength");
const Boolean CSignalStrength::DBG = FALSE;
const AutoPtr< ArrayOf<String> > CSignalStrength::SIGNAL_STRENGTH_NAMES = InitSIGNAL_STRENGTH_NAMES();

CAR_INTERFACE_IMPL_2(CSignalStrength, Object, ISignalStrength, IParcelable)

CAR_OBJECT_IMPL(CSignalStrength)

CSignalStrength::CSignalStrength()
    : mGsmSignalStrength(99)
    , mGsmBitErrorRate(-1)
    , mCdmaDbm(-1)
    , mCdmaEcio(-1)
    , mEvdoDbm(-1)
    , mEvdoEcio(-1)
    , mEvdoSnr(-1)
    , mLteSignalStrength(99)
    , mLteRsrp(ISignalStrength::INVALID)
    , mLteRsrq(ISignalStrength::INVALID)
    , mLteRssnr(ISignalStrength::INVALID)
    , mLteCqi(ISignalStrength::INVALID)
    , mIsGsm(FALSE)
{
}

CSignalStrength::~CSignalStrength()
{
}

ECode CSignalStrength::constructor()
{
    return NOERROR;
}

ECode CSignalStrength::constructor(
    /* [in] */ Boolean gsmFlag)
{

    mGsmSignalStrength = 99;
    mGsmBitErrorRate = -1;
    mCdmaDbm = -1;
    mCdmaEcio = -1;
    mEvdoDbm = -1;
    mEvdoEcio = -1;
    mEvdoSnr = -1;
    mLteSignalStrength = 99;
    mLteRsrp = INVALID;
    mLteRsrq = INVALID;
    mLteRssnr = INVALID;
    mLteCqi = INVALID;
    mIsGsm = gsmFlag;
    return NOERROR;
}

ECode CSignalStrength::constructor(
    /* [in] */ Int32 gsmSignalStrength,
    /* [in] */ Int32 gsmBitErrorRate,
    /* [in] */ Int32 cdmaDbm,
    /* [in] */ Int32 cdmaEcio,
    /* [in] */ Int32 evdoDbm,
    /* [in] */ Int32 evdoEcio,
    /* [in] */ Int32 evdoSnr,
    /* [in] */ Int32 lteSignalStrength,
    /* [in] */ Int32 lteRsrp,
    /* [in] */ Int32 lteRsrq,
    /* [in] */ Int32 lteRssnr,
    /* [in] */ Int32 lteCqi,
    /* [in] */ Boolean gsmFlag)
{
    Initialize(gsmSignalStrength, gsmBitErrorRate, cdmaDbm, cdmaEcio,
            evdoDbm, evdoEcio, evdoSnr, lteSignalStrength, lteRsrp,
            lteRsrq, lteRssnr, lteCqi, gsmFlag);
    return NOERROR;
}

ECode CSignalStrength::constructor(
    /* [in] */ Int32 gsmSignalStrength,
    /* [in] */ Int32 gsmBitErrorRate,
    /* [in] */ Int32 cdmaDbm,
    /* [in] */ Int32 cdmaEcio,
    /* [in] */ Int32 evdoDbm,
    /* [in] */ Int32 evdoEcio,
    /* [in] */ Int32 evdoSnr,
    /* [in] */ Boolean gsmFlag)
{
    Initialize(gsmSignalStrength, gsmBitErrorRate, cdmaDbm, cdmaEcio,
            evdoDbm, evdoEcio, evdoSnr, 99, ISignalStrength::INVALID,
            ISignalStrength::INVALID, ISignalStrength::INVALID, ISignalStrength::INVALID, gsmFlag);
    return NOERROR;
}

ECode CSignalStrength::constructor(
    /* [in] */ ISignalStrength* s)
{
    return CopyFrom(s);
}

ECode CSignalStrength::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mGsmSignalStrength);
    source->ReadInt32(&mGsmBitErrorRate);
    source->ReadInt32(&mCdmaDbm);
    source->ReadInt32(&mCdmaEcio);
    source->ReadInt32(&mEvdoDbm);
    source->ReadInt32(&mEvdoEcio);
    source->ReadInt32(&mEvdoSnr);
    source->ReadInt32(&mLteSignalStrength);
    source->ReadInt32(&mLteRsrp);
    source->ReadInt32(&mLteRsrq);
    source->ReadInt32(&mLteRssnr);
    source->ReadInt32(&mLteCqi);
    Int32 isGsm;
    source->ReadInt32(&isGsm);
    mIsGsm = isGsm;
    return NOERROR;
}

ECode CSignalStrength::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mGsmSignalStrength);
    dest->WriteInt32(mGsmBitErrorRate);
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
    dest->WriteInt32(mIsGsm ? 1 : 0);
    return NOERROR;
}

ECode CSignalStrength::Initialize(
    /* [in] */ Int32 gsmSignalStrength,
    /* [in] */ Int32 gsmBitErrorRate,
    /* [in] */ Int32 cdmaDbm,
    /* [in] */ Int32 cdmaEcio,
    /* [in] */ Int32 evdoDbm,
    /* [in] */ Int32 evdoEcio,
    /* [in] */ Int32 evdoSnr,
    /* [in] */ Boolean gsm)
{
    Initialize(gsmSignalStrength, gsmBitErrorRate, cdmaDbm, cdmaEcio,
            evdoDbm, evdoEcio, evdoSnr, 99, ISignalStrength::INVALID,
            ISignalStrength::INVALID, ISignalStrength::INVALID, ISignalStrength::INVALID, gsm);
    return NOERROR;
}

ECode CSignalStrength::Initialize(
    /* [in] */ Int32 gsmSignalStrength,
    /* [in] */ Int32 gsmBitErrorRate,
    /* [in] */ Int32 cdmaDbm,
    /* [in] */ Int32 cdmaEcio,
    /* [in] */ Int32 evdoDbm,
    /* [in] */ Int32 evdoEcio,
    /* [in] */ Int32 evdoSnr,
    /* [in] */ Int32 lteSignalStrength,
    /* [in] */ Int32 lteRsrp,
    /* [in] */ Int32 lteRsrq,
    /* [in] */ Int32 lteRssnr,
    /* [in] */ Int32 lteCqi,
    /* [in] */ Boolean gsm)
{
    mGsmSignalStrength = gsmSignalStrength;
    mGsmBitErrorRate = gsmBitErrorRate;
    mCdmaDbm = cdmaDbm;
    mCdmaEcio = cdmaEcio;
    mEvdoDbm = evdoDbm;
    mEvdoEcio = evdoEcio;
    mEvdoSnr = evdoSnr;
    mLteSignalStrength = lteSignalStrength;
    mLteRsrp = lteRsrp;
    mLteRsrq = lteRsrq;
    mLteRssnr = lteRssnr;
    mLteCqi = lteCqi;
    mIsGsm = gsm;
    if (DBG) {
        String str;
        ToString(&str);
        Slogger::I(TAG, "initialize: %s", str.string());
    }

    return NOERROR;
}

ECode CSignalStrength::ValidateInput()
{
    if (DBG) Slogger::I(TAG, "Signal before validate=+ this");
    // TS 27.007 8.5
    mGsmSignalStrength = mGsmSignalStrength >= 0 ? mGsmSignalStrength : 99;
    // BER no change;

    mCdmaDbm = mCdmaDbm > 0 ? -mCdmaDbm : -120;
    mCdmaEcio = (mCdmaEcio > 0) ? -mCdmaEcio : -160;

    mEvdoDbm = (mEvdoDbm > 0) ? -mEvdoDbm : -120;
    mEvdoEcio = (mEvdoEcio >= 0) ? -mEvdoEcio : -1;
    mEvdoSnr = ((mEvdoSnr > 0) && (mEvdoSnr <= 8)) ? mEvdoSnr : -1;

    // TS 36.214 Physical Layer Section 5.1.3, TS 36.331 RRC
    mLteSignalStrength = (mLteSignalStrength >= 0) ? mLteSignalStrength : 99;
    mLteRsrp = ((mLteRsrp >= 44) && (mLteRsrp <= 140)) ? -mLteRsrp : ISignalStrength::INVALID;
    mLteRsrq = ((mLteRsrq >= 3) && (mLteRsrq <= 20)) ? -mLteRsrq : ISignalStrength::INVALID;
    mLteRssnr = ((mLteRssnr >= -200) && (mLteRssnr <= 300)) ? mLteRssnr
            : ISignalStrength::INVALID;
    // Cqi no change
    if (DBG) Slogger::I(TAG, "Signal after validate= + this");

    return NOERROR;
}

ECode CSignalStrength::SetGsm(
    /* [in] */ Boolean gsmFlag)
{
    mIsGsm = gsmFlag;
    return NOERROR;
}

ECode CSignalStrength::GetGsmSignalStrength(
    /* [out] */ Int32* gsmSignalStrength)
{
    VALIDATE_NOT_NULL(gsmSignalStrength);
    *gsmSignalStrength = mGsmSignalStrength;
    return NOERROR;
}

ECode CSignalStrength::GetGsmBitErrorRate(
    /* [out] */ Int32* gsmBitErrorRate)
{
    VALIDATE_NOT_NULL(gsmBitErrorRate);
    *gsmBitErrorRate = mGsmBitErrorRate;
    return NOERROR;
}

ECode CSignalStrength::GetCdmaDbm(
    /* [out] */ Int32* cdmaDbm)
{
    VALIDATE_NOT_NULL(cdmaDbm);
    *cdmaDbm = mCdmaDbm;
    return NOERROR;
}

ECode CSignalStrength::GetCdmaEcio(
    /* [out] */ Int32* cdmaEcio)
{
    VALIDATE_NOT_NULL(cdmaEcio);
    *cdmaEcio = mCdmaEcio;
    return NOERROR;
}

ECode CSignalStrength::GetEvdoDbm(
    /* [out] */ Int32* evdoDbm)
{
    VALIDATE_NOT_NULL(evdoDbm);
    *evdoDbm = mEvdoDbm;
    return NOERROR;
}

ECode CSignalStrength::GetEvdoEcio(
    /* [out] */ Int32* evdoEcio)
{
    VALIDATE_NOT_NULL(evdoEcio);
    *evdoEcio = mEvdoEcio;
    return NOERROR;
}

ECode CSignalStrength::GetEvdoSnr(
    /* [out] */ Int32* evdoSnr)
{
    VALIDATE_NOT_NULL(evdoSnr);
    *evdoSnr = mEvdoSnr;
    return NOERROR;
}

ECode CSignalStrength::GetLteSignalStrength(
    /* [out] */ Int32* lteSignalStrength)
{
    VALIDATE_NOT_NULL(lteSignalStrength);
    *lteSignalStrength = mLteSignalStrength;
    return NOERROR;
}

ECode CSignalStrength::GetLteRsrp(
    /* [out] */ Int32* lteRsrp)
{
    VALIDATE_NOT_NULL(lteRsrp);
    *lteRsrp = mLteRsrp;
    return NOERROR;
}

ECode CSignalStrength::GetLteRsrq(
    /* [out] */ Int32* lteRsrq)
{
    VALIDATE_NOT_NULL(lteRsrq);
    *lteRsrq = mLteRsrq;
    return NOERROR;
}

ECode CSignalStrength::GetLteRssnr(
    /* [out] */ Int32* lteRssnr)
{
    VALIDATE_NOT_NULL(lteRssnr);
    *lteRssnr = mLteRssnr;
    return NOERROR;
}

ECode CSignalStrength::GetLteCqi(
    /* [out] */ Int32* lteCqi)
{
    VALIDATE_NOT_NULL(lteCqi);
    *lteCqi = mLteCqi;
    return NOERROR;
}

ECode CSignalStrength::GetLevel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 level;
    if (mIsGsm) {
        GetLteLevel(&level);
        if (level == ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
            GetGsmLevel(&level);
        }
    } else {
        Int32 cdmaLevel;
        GetCdmaLevel(&cdmaLevel);
        Int32 evdoLevel;
        GetEvdoLevel(&evdoLevel);
        if (evdoLevel == ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
            /* We don't know evdo, use cdma */
            level = cdmaLevel;
        } else if (cdmaLevel == ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
            /* We don't know cdma, use evdo */
            level = evdoLevel;
        } else {
            /* We know both, use the lowest level */
            level = cdmaLevel < evdoLevel ? cdmaLevel : evdoLevel;
        }
    }
    if (DBG) Slogger::I(TAG, "getLevel=%d", level);
    *result = level;
    return NOERROR;
}

ECode CSignalStrength::GetAsuLevel(
    /* [out] */ Int32* asuLevel)
{
    VALIDATE_NOT_NULL(asuLevel);
    Int32 lteLevel;
    if (mIsGsm) {
        if ((GetLteLevel(&lteLevel), lteLevel) == ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
            GetGsmAsuLevel(asuLevel);
        } else {
            GetLteAsuLevel(asuLevel);
        }
    } else {
        Int32 cdmaAsuLevel;
        GetCdmaAsuLevel(&cdmaAsuLevel);
        Int32 evdoAsuLevel;
        GetEvdoAsuLevel(&evdoAsuLevel);
        if (evdoAsuLevel == 0) {
            /* We don't know evdo use, cdma */
            *asuLevel = cdmaAsuLevel;
        } else if (cdmaAsuLevel == 0) {
            /* We don't know cdma use, evdo */
            *asuLevel = evdoAsuLevel;
        } else {
            /* We know both, use the lowest level */
            *asuLevel = cdmaAsuLevel < evdoAsuLevel ? cdmaAsuLevel : evdoAsuLevel;
        }
    }
    if (DBG) Slogger::I(TAG, "getAsuLevel=%d", *asuLevel);
    return NOERROR;
}

ECode CSignalStrength::GetDbm(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 dBm;
    Boolean isGsm;
    if(IsGsm(&isGsm), isGsm) {
        GetLteDbm(&dBm);
        if (dBm == INVALID) {
            GetGsmDbm(&dBm);
        }
    } else {
        Int32 cdmaDbm;
        GetCdmaDbm(&cdmaDbm);
        Int32 evdoDbm;
        GetEvdoDbm(&evdoDbm);

        dBm = ((evdoDbm == -120) ? cdmaDbm : ((cdmaDbm == -120) ? evdoDbm
                : (cdmaDbm < evdoDbm ? cdmaDbm : evdoDbm)));
        return NOERROR;
    }
    if (DBG) Slogger::I(TAG, "getDbm=%d", dBm);
    *result = dBm;
    return NOERROR;
}

ECode CSignalStrength::GetGsmDbm(
    /* [out] */ Int32* gsmDbm)
{
    VALIDATE_NOT_NULL(gsmDbm);
    Int32 dBm;

    Int32 gsmSignalStrength;
    GetGsmSignalStrength(&gsmSignalStrength);
    Int32 asu = (gsmSignalStrength == 99 ? -1 : gsmSignalStrength);
    if (asu != -1) {
        dBm = -113 + (2 * asu);
    } else {
        dBm = -1;
    }
    if (DBG) Slogger::I(TAG, "getGsmDbm=%d", dBm);
    *gsmDbm = dBm;
    return NOERROR;
}

ECode CSignalStrength::GetGsmLevel(
    /* [out] */ Int32* gsmLevel)
{
    VALIDATE_NOT_NULL(gsmLevel);
    Int32 level;

    // ASU ranges from 0 to 31 - TS 27.007 Sec 8.5
    // asu = 0 (-113dB or less) is very weak
    // signal, its better to show 0 bars to the user in such cases.
    // asu = 99 is a special case, where the signal strength is unknown.
    Int32 asu;
    GetGsmSignalStrength(&asu);
    if (asu <= 2 || asu == 99) level = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;
    else if (asu >= 12) level = ISignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (asu >= 8)  level = ISignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (asu >= 5)  level = ISignalStrength::SIGNAL_STRENGTH_MODERATE;
    else level = ISignalStrength::SIGNAL_STRENGTH_POOR;
    if (DBG) Slogger::I(TAG, "getGsmLevel=%d", level);
    *gsmLevel = level;
    return NOERROR;
}

ECode CSignalStrength::GetGsmAsuLevel(
    /* [out] */ Int32* gsmAsuLevel)
{
    VALIDATE_NOT_NULL(gsmAsuLevel);

    // ASU ranges from 0 to 31 - TS 27.007 Sec 8.5
    // asu = 0 (-113dB or less) is very weak
    // signal, its better to show 0 bars to the user in such cases.
    // asu = 99 is a special case, where the signal strength is unknown.
    Int32 level;
    GetGsmSignalStrength(&level);
    if (DBG) Slogger::I(TAG, "getGsmAsuLevel=%d", level);
    *gsmAsuLevel = level;

    return NOERROR;
}

ECode CSignalStrength::GetCdmaLevel(
    /* [out] */ Int32* cdmaLevel)
{
    VALIDATE_NOT_NULL(cdmaLevel);
    Int32 cdmaDbm;
    GetCdmaDbm(&cdmaDbm);
    Int32 cdmaEcio;
    GetCdmaEcio(&cdmaEcio);
    Int32 levelDbm;
    Int32 levelEcio;

    if (cdmaDbm >= -75) levelDbm = ISignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (cdmaDbm >= -85) levelDbm = ISignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (cdmaDbm >= -95) levelDbm = ISignalStrength::SIGNAL_STRENGTH_MODERATE;
    else if (cdmaDbm >= -100) levelDbm = ISignalStrength::SIGNAL_STRENGTH_POOR;
    else levelDbm = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    // Ec/Io are in dB*10
    if (cdmaEcio >= -90) levelEcio = ISignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (cdmaEcio >= -110) levelEcio = ISignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (cdmaEcio >= -130) levelEcio = ISignalStrength::SIGNAL_STRENGTH_MODERATE;
    else if (cdmaEcio >= -150) levelEcio = ISignalStrength::SIGNAL_STRENGTH_POOR;
    else levelEcio = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    Int32 level = (levelDbm < levelEcio) ? levelDbm : levelEcio;
    if (DBG) Slogger::I(TAG, "getCdmaLevel=%d", level);
    *cdmaLevel = level;
    return NOERROR;
}

ECode CSignalStrength::GetCdmaAsuLevel(
    /* [out] */ Int32* cdmaAsuLevel)
{
    VALIDATE_NOT_NULL(cdmaAsuLevel);

    Int32 cdmaDbm;
    GetCdmaDbm(&cdmaDbm);
    Int32 cdmaEcio;
    GetCdmaEcio(&cdmaEcio);
    Int32 ecioAsuLevel;

    if (cdmaDbm >= -75) *cdmaAsuLevel = 16;
    else if (cdmaDbm >= -82) *cdmaAsuLevel = 8;
    else if (cdmaDbm >= -90) *cdmaAsuLevel = 4;
    else if (cdmaDbm >= -95) *cdmaAsuLevel = 2;
    else if (cdmaDbm >= -100) *cdmaAsuLevel = 1;
    else *cdmaAsuLevel = 99;

    // Ec/Io are in dB*10
    if (cdmaEcio >= -90) ecioAsuLevel = 16;
    else if (cdmaEcio >= -100) ecioAsuLevel = 8;
    else if (cdmaEcio >= -115) ecioAsuLevel = 4;
    else if (cdmaEcio >= -130) ecioAsuLevel = 2;
    else if (cdmaEcio >= -150) ecioAsuLevel = 1;
    else ecioAsuLevel = 99;

    Int32 level = (*cdmaAsuLevel < ecioAsuLevel) ? *cdmaAsuLevel : ecioAsuLevel;
    if (DBG) Slogger::I(TAG, "getCdmaAsuLevel=%d", level);
    return NOERROR;
}

ECode CSignalStrength::GetEvdoLevel(
    /* [out] */ Int32* evdoLevel)
{
    VALIDATE_NOT_NULL(evdoLevel);

    Int32 evdoDbm;
    GetEvdoDbm(&evdoDbm);
    Int32 evdoSnr;
    GetEvdoSnr(&evdoSnr);
    Int32 levelEvdoDbm;
    Int32 levelEvdoSnr;

    if (evdoDbm >= -65) levelEvdoDbm = ISignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (evdoDbm >= -75) levelEvdoDbm = ISignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (evdoDbm >= -90) levelEvdoDbm = ISignalStrength::SIGNAL_STRENGTH_MODERATE;
    else if (evdoDbm >= -105) levelEvdoDbm = ISignalStrength::SIGNAL_STRENGTH_POOR;
    else levelEvdoDbm = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    if (evdoSnr >= 7) levelEvdoSnr = ISignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (evdoSnr >= 5) levelEvdoSnr = ISignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (evdoSnr >= 3) levelEvdoSnr = ISignalStrength::SIGNAL_STRENGTH_MODERATE;
    else if (evdoSnr >= 1) levelEvdoSnr = ISignalStrength::SIGNAL_STRENGTH_POOR;
    else levelEvdoSnr = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    Int32 level = (levelEvdoDbm < levelEvdoSnr) ? levelEvdoDbm : levelEvdoSnr;
    if (DBG) Slogger::I(TAG, "getEvdoLevel=%d", level);
    *evdoLevel = level;
    return NOERROR;
}

ECode CSignalStrength::GetEvdoAsuLevel(
    /* [out] */ Int32* evdoAsuLevel)
{
    VALIDATE_NOT_NULL(evdoAsuLevel);

    Int32 evdoDbm;
    GetEvdoDbm(&evdoDbm);
    Int32 evdoSnr;
    GetEvdoSnr(&evdoSnr);
    Int32 levelEvdoDbm;
    Int32 levelEvdoSnr;

    if (evdoDbm >= -65) levelEvdoDbm = 16;
    else if (evdoDbm >= -75) levelEvdoDbm = 8;
    else if (evdoDbm >= -85) levelEvdoDbm = 4;
    else if (evdoDbm >= -95) levelEvdoDbm = 2;
    else if (evdoDbm >= -105) levelEvdoDbm = 1;
    else levelEvdoDbm = 99;

    if (evdoSnr >= 7) levelEvdoSnr = 16;
    else if (evdoSnr >= 6) levelEvdoSnr = 8;
    else if (evdoSnr >= 5) levelEvdoSnr = 4;
    else if (evdoSnr >= 3) levelEvdoSnr = 2;
    else if (evdoSnr >= 1) levelEvdoSnr = 1;
    else levelEvdoSnr = 99;

    Int32 level = (levelEvdoDbm < levelEvdoSnr) ? levelEvdoDbm : levelEvdoSnr;
    if (DBG) Slogger::I(TAG, "getEvdoAsuLevel=%d", level);
    *evdoAsuLevel = level;
    return NOERROR;
}

ECode CSignalStrength::GetLteDbm(
    /* [out] */ Int32* lteDbm)
{
    VALIDATE_NOT_NULL(lteDbm);
    *lteDbm = mLteRsrp;
    return NOERROR;
}

ECode CSignalStrength::GetLteLevel(
    /* [out] */ Int32* lteLevel)
{
    VALIDATE_NOT_NULL(lteLevel);
    /*
     * TS 36.214 Physical Layer Section 5.1.3 TS 36.331 RRC RSSI = received
     * signal + noise RSRP = reference signal dBm RSRQ = quality of signal
     * dB= Number of Resource blocksxRSRP/RSSI SNR = gain=signal/noise ratio
     * = -10log P1/P2 dB
     */
    Int32 rssiIconLevel = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN, rsrpIconLevel = -1, snrIconLevel = -1;

    if (mLteRsrp > -44) rsrpIconLevel = -1;
    else if (mLteRsrp >= -85) rsrpIconLevel = ISignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (mLteRsrp >= -95) rsrpIconLevel = ISignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (mLteRsrp >= -105) rsrpIconLevel = ISignalStrength::SIGNAL_STRENGTH_MODERATE;
    else if (mLteRsrp >= -115) rsrpIconLevel = ISignalStrength::SIGNAL_STRENGTH_POOR;
    else if (mLteRsrp >= -140) rsrpIconLevel = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    /*
     * Values are -200 dB to +300 (SNR*10dB) RS_SNR >= 13.0 dB =>4 bars 4.5
     * dB <= RS_SNR < 13.0 dB => 3 bars 1.0 dB <= RS_SNR < 4.5 dB => 2 bars
     * -3.0 dB <= RS_SNR < 1.0 dB 1 bar RS_SNR < -3.0 dB/No Service Antenna
     * Icon Only
     */
    if (mLteRssnr > 300) snrIconLevel = -1;
    else if (mLteRssnr >= 130) snrIconLevel = ISignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (mLteRssnr >= 45) snrIconLevel = ISignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (mLteRssnr >= 10) snrIconLevel = ISignalStrength::SIGNAL_STRENGTH_MODERATE;
    else if (mLteRssnr >= -30) snrIconLevel = ISignalStrength::SIGNAL_STRENGTH_POOR;
    else if (mLteRssnr >= -200)
        snrIconLevel = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;

    if (DBG) Slogger::I(TAG, "getLTELevel - rsrp:%d snr:%d rsrpIconLevel:%d snrIconLevel:%d",mLteRsrp, mLteRssnr, rsrpIconLevel, snrIconLevel);

    /* Choose a measurement type to use for notification */
    if (snrIconLevel != -1 && rsrpIconLevel != -1) {
        /*
         * The number of bars displayed shall be the smaller of the bars
         * associated with LTE RSRP and the bars associated with the LTE
         * RS_SNR
         */
        *lteLevel = (rsrpIconLevel < snrIconLevel ? rsrpIconLevel : snrIconLevel);
        return NOERROR;
    }

    if (snrIconLevel != -1) {
        *lteLevel = snrIconLevel;
        return NOERROR;
    }

    if (rsrpIconLevel != -1) {
        *lteLevel = rsrpIconLevel;
        return NOERROR;
    }

    /* Valid values are (0-63, 99) as defined in TS 36.331 */
    if (mLteSignalStrength > 63) rssiIconLevel = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;
    else if (mLteSignalStrength >= 12) rssiIconLevel = ISignalStrength::SIGNAL_STRENGTH_GREAT;
    else if (mLteSignalStrength >= 8) rssiIconLevel = ISignalStrength::SIGNAL_STRENGTH_GOOD;
    else if (mLteSignalStrength >= 5) rssiIconLevel = ISignalStrength::SIGNAL_STRENGTH_MODERATE;
    else if (mLteSignalStrength >= 0) rssiIconLevel = ISignalStrength::SIGNAL_STRENGTH_POOR;
    if (DBG) Slogger::I(TAG, "getLTELevel - rssi:%d rssiIconLevel:%d", mLteSignalStrength, rssiIconLevel);
    *lteLevel = rssiIconLevel;
    return NOERROR;
}

ECode CSignalStrength::GetLteAsuLevel(
    /* [out] */ Int32* lteAsuLevel)
{
    VALIDATE_NOT_NULL(lteAsuLevel);
    *lteAsuLevel = 99;
    Int32 lteDbm;
    GetLteDbm(&lteDbm);
    /*
     * 3GPP 27.007 (Ver 10.3.0) Sec 8.69
     * 0   -140 dBm or less
     * 1   -139 dBm
     * 2...96  -138... -44 dBm
     * 97  -43 dBm or greater
     * 255 not known or not detectable
     */
    /*
     * validateInput will always give a valid range between -140 t0 -44 as
     * per ril.h. so RSRP >= -43 & <-140 will fall under asu level 255
     * and not 97 or 0
     */
    if (lteDbm == ISignalStrength::INVALID) *lteAsuLevel = 255;
    else *lteAsuLevel = lteDbm + 140;
    if (DBG) Slogger::I(TAG, "Lte Asu level:%d" , *lteAsuLevel);
    return NOERROR;
}

ECode CSignalStrength::IsGsm(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mIsGsm;
    return NOERROR;
}

ECode CSignalStrength::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 primeNum = 31;
    *hashCode = ((mGsmSignalStrength * primeNum)
            + (mGsmBitErrorRate * primeNum)
            + (mCdmaDbm * primeNum) + (mCdmaEcio * primeNum)
            + (mEvdoDbm * primeNum) + (mEvdoEcio * primeNum) + (mEvdoSnr * primeNum)
            + (mLteSignalStrength * primeNum) + (mLteRsrp * primeNum)
            + (mLteRsrq * primeNum) + (mLteRssnr * primeNum) + (mLteCqi * primeNum)
            + (mIsGsm ? 1 : 0));
    return NOERROR;
}

ECode CSignalStrength::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<ISignalStrength> s;
    s = ISignalStrength::Probe(o);

    /*try {
        s = (SignalStrength) o;
    } catch (ClassCastException ex) {
        return false;
    }*/

    if (o == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 gsmSignalStrength, gsmBitErrorRate, cdmaDbm, cdmaEcio, evdoDbm, evdoEcio, evdoSnr, lteSignalStrength, lteRsrp, lteRsrq, lteRssnr, lteCqi;
    Boolean isGsm;

    s->GetGsmSignalStrength(&gsmSignalStrength);
    s->GetGsmBitErrorRate(&gsmBitErrorRate);
    s->GetCdmaDbm(&cdmaDbm);
    s->GetCdmaEcio(&cdmaEcio);
    s->GetEvdoDbm(&evdoDbm);
    s->GetEvdoEcio(&evdoEcio);
    s->GetEvdoSnr(&evdoSnr);
    s->GetLteSignalStrength(&lteSignalStrength);
    s->GetLteRsrp(&lteRsrp);
    s->GetLteRsrq(&lteRsrq);
    s->GetLteRssnr(&lteRssnr);
    s->GetLteCqi(&lteCqi);
    s->IsGsm(&isGsm);

    *res = (mGsmSignalStrength == gsmSignalStrength
            && mGsmBitErrorRate == gsmBitErrorRate
            && mCdmaDbm == cdmaDbm
            && mCdmaEcio == cdmaEcio
            && mEvdoDbm == evdoDbm
            && mEvdoEcio == evdoEcio
            && mEvdoSnr == evdoSnr
            && mLteSignalStrength == lteSignalStrength
            && mLteRsrp == lteRsrp
            && mLteRsrq == lteRsrq
            && mLteRssnr == lteRssnr
            && mLteCqi == lteCqi
            && isGsm == isGsm);

    return NOERROR;
}

ECode CSignalStrength::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("SignalStrength:");
    sb.Append(" ");sb.Append(mGsmSignalStrength);
    sb.Append(" ");sb.Append(mGsmBitErrorRate);
    sb.Append(" ");sb.Append(mCdmaDbm);
    sb.Append(" ");sb.Append(mCdmaEcio);
    sb.Append(" ");sb.Append(mEvdoDbm);
    sb.Append(" ");sb.Append(mEvdoEcio);
    sb.Append(" ");sb.Append(mEvdoSnr);
    sb.Append(" ");sb.Append(mLteSignalStrength);
    sb.Append(" ");sb.Append(mLteRsrp);
    sb.Append(" ");sb.Append(mLteRsrq);
    sb.Append(" ");sb.Append(mLteRssnr);
    sb.Append(" ");sb.Append(mLteCqi);
    sb.Append(" ");sb.Append((mIsGsm ? "gsm|lte" : "cdma"));
    *str = sb.ToString();
    return NOERROR;
}

ECode CSignalStrength::FillInNotifierBundle(
    /* [in] */ IBundle* m)
{
    m->PutInt32(String("GsmSignalStrength"), mGsmSignalStrength);
    m->PutInt32(String("GsmBitErrorRate"), mGsmBitErrorRate);
    m->PutInt32(String("CdmaDbm"), mCdmaDbm);
    m->PutInt32(String("CdmaEcio"), mCdmaEcio);
    m->PutInt32(String("EvdoDbm"), mEvdoDbm);
    m->PutInt32(String("EvdoEcio"), mEvdoEcio);
    m->PutInt32(String("EvdoSnr"), mEvdoSnr);
    m->PutInt32(String("LteSignalStrength"), mLteSignalStrength);
    m->PutInt32(String("LteRsrp"), mLteRsrp);
    m->PutInt32(String("LteRsrq"), mLteRsrq);
    m->PutInt32(String("LteRssnr"), mLteRssnr);
    m->PutInt32(String("LteCqi"), mLteCqi);
    m->PutBoolean(String("isGsm"), mIsGsm);
    return NOERROR;
}

ECode CSignalStrength::NewFromBundle(
    /* [in] */ IBundle* m,
    /* [out] */ ISignalStrength** ss)
{
    VALIDATE_NOT_NULL(ss);
    AutoPtr<CSignalStrength> ret;
    CSignalStrength::NewByFriend((CSignalStrength**)&ret);
    ret->SetFromNotifierBundle(m);
    *ss = ISignalStrength::Probe(ret);
    REFCOUNT_ADD(*ss);
    return NOERROR;
}

ECode CSignalStrength::SetFromNotifierBundle(
    /* [in] */ IBundle* m)
{
    m->GetInt32(String("GsmSignalStrength"), &mGsmSignalStrength);
    m->GetInt32(String("GsmBitErrorRate"), &mGsmBitErrorRate);
    m->GetInt32(String("CdmaDbm"), &mCdmaDbm);
    m->GetInt32(String("CdmaEcio"), &mCdmaEcio);
    m->GetInt32(String("EvdoDbm"), &mEvdoDbm);
    m->GetInt32(String("EvdoEcio"), &mEvdoEcio);
    m->GetInt32(String("EvdoSnr"), &mEvdoSnr);
    m->GetInt32(String("LteSignalStrength"), &mLteSignalStrength);
    m->GetInt32(String("LteRsrp"), &mLteRsrp);
    m->GetInt32(String("LteRsrq"), &mLteRsrq);
    m->GetInt32(String("LteRssnr"), &mLteRssnr);
    m->GetInt32(String("LteCqi"), &mLteCqi);
    m->GetBoolean(String("isGsm"), &mIsGsm);
    return NOERROR;
}

ECode CSignalStrength::CopyFrom(
    /* [in] */ ISignalStrength* s)
{
    Int32 gsmSignalStrength, gsmBitErrorRate, cdmaDbm, cdmaEcio, evdoDbm, evdoEcio, evdoSnr, lteSignalStrength, lteRsrp, lteRsrq, lteRssnr, lteCqi;
    Boolean isGsm;

    s->GetGsmSignalStrength(&gsmSignalStrength);
    s->GetGsmBitErrorRate(&gsmBitErrorRate);
    s->GetCdmaDbm(&cdmaDbm);
    s->GetCdmaEcio(&cdmaEcio);
    s->GetEvdoDbm(&evdoDbm);
    s->GetEvdoEcio(&evdoEcio);
    s->GetEvdoSnr(&evdoSnr);
    s->GetLteSignalStrength(&lteSignalStrength);
    s->GetLteRsrp(&lteRsrp);
    s->GetLteRsrq(&lteRsrq);
    s->GetLteRssnr(&lteRssnr);
    s->GetLteCqi(&lteCqi);
    s->IsGsm(&isGsm);

    mGsmSignalStrength = gsmSignalStrength;
    mGsmBitErrorRate = gsmBitErrorRate;
    mCdmaDbm = cdmaDbm;
    mCdmaEcio = cdmaEcio;
    mEvdoDbm = evdoDbm;
    mEvdoEcio = evdoEcio;
    mEvdoSnr = evdoSnr;
    mLteSignalStrength = lteSignalStrength;
    mLteRsrp = lteRsrp;
    mLteRsrq = lteRsrq;
    mLteRssnr = lteRssnr;
    mLteCqi = lteCqi;
    mIsGsm = isGsm;
    return NOERROR;
}

AutoPtr<ISignalStrength> CSignalStrength::MakeSignalStrengthFromRilParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<ISignalStrength> signalStrength;
    CSignalStrength::New((ISignalStrength**)&signalStrength);

    Int32 gsmSignalStrength;
    source->ReadInt32(&gsmSignalStrength);
    Int32 gsmBitErrorRate;
    source->ReadInt32(&gsmBitErrorRate);
    Int32 cdmaDbm;
    source->ReadInt32(&cdmaDbm);
    Int32 cdmaEcio;
    source->ReadInt32(&cdmaEcio);
    Int32 evdoDbm;
    source->ReadInt32(&evdoDbm);
    Int32 evdoEcio;
    source->ReadInt32(&evdoEcio);
    Int32 evdoSnr;
    source->ReadInt32(&evdoSnr);
    Int32 lteSignalStrength;
    source->ReadInt32(&lteSignalStrength);
    Int32 lteRsrp;
    source->ReadInt32(&lteRsrp);
    Int32 lteRsrq;
    source->ReadInt32(&lteRsrq);
    Int32 lteRssnr;
    source->ReadInt32(&lteRssnr);
    Int32 lteCqi;
    source->ReadInt32(&lteCqi);
    signalStrength->Initialize(
            gsmSignalStrength, gsmBitErrorRate, cdmaDbm, cdmaEcio, evdoDbm, evdoEcio,
            evdoSnr, lteSignalStrength, lteRsrp, lteRsrq, lteRssnr, lteCqi, TRUE);//TODO

    return signalStrength;
}


} // namespace Telephony
} // namespace Droid
} // namespace Elastos

