#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/internal/telephony/cdma/SignalToneUtil.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Media::IToneGenerator;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                            SignalToneUtil
//=====================================================================
CAR_INTERFACE_IMPL(SignalToneUtil, Object, ISignalToneUtil);

AutoPtr<IHashMap> SignalToneUtil::mHm = InitHashMap();

Int32 SignalToneUtil::GetAudioToneFromSignalInfo(
    /* [in] */ Int32 signalType,
    /* [in] */ Int32 alertPitch,
    /* [in] */ Int32 signal)
{
    AutoPtr<IInteger32> i32 = SignalParamHash(signalType, alertPitch, signal);
    AutoPtr<IInterface> obj;
     mHm->Get(i32, (IInterface**)&obj);
    AutoPtr<IInteger32> result = IInteger32::Probe(obj);
    if (result == NULL) {
        return CDMA_INVALID_TONE;
    }
    return CoreUtils::Unbox(result);
}

AutoPtr<IHashMap> SignalToneUtil::InitHashMap()
{
    AutoPtr<IHashMap> ret;
    CHashMap::New((IHashMap**)&ret);

    /* SIGNAL_TYPE_ISDN */
    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_ISDN_NORMAL), CoreUtils::Convert(IToneGenerator::TONE_CDMA_CALL_SIGNAL_ISDN_NORMAL));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
                    IS95_CONST_IR_SIG_ISDN_INTGRP),
                    CoreUtils::Convert(IToneGenerator::TONE_CDMA_CALL_SIGNAL_ISDN_INTERGROUP));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_ISDN_SP_PRI), CoreUtils::Convert(IToneGenerator::TONE_CDMA_CALL_SIGNAL_ISDN_SP_PRI));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_ISDN_PAT_3), CoreUtils::Convert(IToneGenerator::TONE_CDMA_CALL_SIGNAL_ISDN_PAT3));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_ISDN_PING), CoreUtils::Convert(IToneGenerator::TONE_CDMA_CALL_SIGNAL_ISDN_PING_RING));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_ISDN_PAT_5), CoreUtils::Convert(IToneGenerator::TONE_CDMA_CALL_SIGNAL_ISDN_PAT5));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_ISDN_PAT_6), CoreUtils::Convert(IToneGenerator::TONE_CDMA_CALL_SIGNAL_ISDN_PAT6));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_ISDN_PAT_7), CoreUtils::Convert(IToneGenerator::TONE_CDMA_CALL_SIGNAL_ISDN_PAT7));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_ISDN_OFF), CoreUtils::Convert(IToneGenerator::TONE_CDMA_SIGNAL_OFF));

    /* SIGNAL_TYPE_TONE */

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_DIAL), CoreUtils::Convert(IToneGenerator::TONE_CDMA_DIAL_TONE_LITE));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_RING), CoreUtils::Convert(IToneGenerator::TONE_CDMA_NETWORK_USA_RINGBACK));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_INT), CoreUtils::Convert(IToneGenerator::TONE_SUP_INTERCEPT));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_ABB_INT), CoreUtils::Convert(IToneGenerator::TONE_SUP_INTERCEPT_ABBREV));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_REORDER), CoreUtils::Convert(IToneGenerator::TONE_CDMA_REORDER));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_ABB_RE), CoreUtils::Convert(IToneGenerator::TONE_CDMA_ABBR_REORDER));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_BUSY), CoreUtils::Convert(IToneGenerator::TONE_CDMA_NETWORK_BUSY));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_CONFIRM), CoreUtils::Convert(IToneGenerator::TONE_SUP_CONFIRM));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_ANSWER), CoreUtils::Convert(IToneGenerator::TONE_CDMA_ANSWER));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_CALL_W), CoreUtils::Convert(IToneGenerator::TONE_CDMA_NETWORK_CALLWAITING));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_PIP), CoreUtils::Convert(IToneGenerator::TONE_CDMA_PIP));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_TONE_NO_TONE), CoreUtils::Convert(IToneGenerator::TONE_CDMA_SIGNAL_OFF));

    /* SIGNAL_TYPE_IS54B */
    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_L), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_L));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_L), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_L));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_L), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_L));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_SS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_SS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_SS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_SS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_SS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_SS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_SSL), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_SSL));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_SSL), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_SSL));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_SSL), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_SSL));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_SS_2), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_SS_2));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_SS_2), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_SS_2));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_SS_2), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_SS_2));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_SLS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_SLS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_SLS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_SLS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_SLS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_SLS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_S_X4), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_S_X4));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_S_X4), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_S_X4));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_S_X4), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_S_X4));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_PBX_L), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_PBX_L));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_PBX_L), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_PBX_L));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_PBX_L), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_PBX_L));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_PBX_SS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_PBX_SS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_PBX_SS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_PBX_SS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_PBX_SS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_PBX_SS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_PBX_SSL), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_PBX_SSL));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_PBX_SSL), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_PBX_SSL));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_PBX_SSL), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_PBX_SSL));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_PBX_SLS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_PBX_SLS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_PBX_SLS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_PBX_SLS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_PBX_SLS), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_PBX_SLS));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
            IS95_CONST_IR_SIG_IS54B_PBX_S_X4), CoreUtils::Convert(IToneGenerator::TONE_CDMA_HIGH_PBX_S_X4));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
            IS95_CONST_IR_SIG_IS54B_PBX_S_X4), CoreUtils::Convert(IToneGenerator::TONE_CDMA_MED_PBX_S_X4));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
            IS95_CONST_IR_SIG_IS54B_PBX_S_X4), CoreUtils::Convert(IToneGenerator::TONE_CDMA_LOW_PBX_S_X4));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_IS54B, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
            IS95_CONST_IR_SIG_IS54B_NO_TONE), CoreUtils::Convert(IToneGenerator::TONE_CDMA_SIGNAL_OFF));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_USR_DEFD_ALERT,
            TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN, IS95_CONST_IR_SIG_TONE_ABBR_ALRT),
            CoreUtils::Convert(IToneGenerator::TONE_CDMA_ABBR_ALERT));

    ret->Put(SignalParamHash(IS95_CONST_IR_SIGNAL_USR_DEFD_ALERT,
            TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN, IS95_CONST_IR_SIG_TONE_NO_TONE),
            CoreUtils::Convert(IToneGenerator::TONE_CDMA_ABBR_ALERT));

    return ret;
}

SignalToneUtil::SignalToneUtil()
{
}

AutoPtr<IInteger32> SignalToneUtil::SignalParamHash(
    /* [in] */ Int32 signalType,
    /* [in] */ Int32 alertPitch,
    /* [in] */ Int32 signal)
{
    if ((signalType < 0) || (signalType > 256) || (alertPitch > 256) ||
            (alertPitch < 0) || (signal > 256) || (signal < 0)) {
        return CoreUtils::Convert(CDMA_INVALID_TONE);
    }
    // Based on 3GPP2 C.S0005-E, seciton 3.7.5.5 Signal,
    // the alert pitch field is ignored by the mobile station unless
    // SIGNAL_TYPE is '10',IS-54B Alerting.
    // Set alert pitch to TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN
    // so the alert pitch is not involved in hash calculation
    // when signal type is not IS-54B.
    if (signalType != IS95_CONST_IR_SIGNAL_IS54B) {
        alertPitch = TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN;
    }
    return CoreUtils::Convert(signalType * 256 * 256 + alertPitch * 256 + signal);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
