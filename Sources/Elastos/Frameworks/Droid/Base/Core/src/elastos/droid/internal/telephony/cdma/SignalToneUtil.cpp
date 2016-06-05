#include "Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"

#include "elastos/droid/internal/telephony/cdma/SignalToneUtil.h"

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

// AutoPtr<IHashMap> SignalToneUtil::mHm = InitHashMap();

Int32 SignalToneUtil::GetAudioToneFromSignalInfo(
    /* [in] */ Int32 signalType,
    /* [in] */ Int32 alertPitch,
    /* [in] */ Int32 signal)
{
    // ==================before translated======================
    // Integer result = mHm.get(signalParamHash(signalType, alertPitch, signal));
    // if (result == null) {
    //     return CDMA_INVALID_TONE;
    // }
    // return result;
    assert(0);
    return 0;
}

AutoPtr<IHashMap> SignalToneUtil::InitHashMap()
{
    AutoPtr<IHashMap> ret;
    CHashMap::New((IHashMap**)&ret);
    // ==================before translated======================
    //
    // /* SIGNAL_TYPE_ISDN */
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_ISDN_NORMAL), ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_NORMAL);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //                 IS95_CONST_IR_SIG_ISDN_INTGRP),
    //                 ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_INTERGROUP);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_ISDN_SP_PRI), ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_SP_PRI);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_ISDN_PAT_3), ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_PAT3);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_ISDN_PING), ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_PING_RING);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_ISDN_PAT_5), ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_PAT5);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_ISDN_PAT_6), ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_PAT6);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_ISDN_PAT_7), ToneGenerator.TONE_CDMA_CALL_SIGNAL_ISDN_PAT7);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_ISDN, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_ISDN_OFF), ToneGenerator.TONE_CDMA_SIGNAL_OFF);
    //
    // /* SIGNAL_TYPE_TONE */
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_DIAL), ToneGenerator.TONE_CDMA_DIAL_TONE_LITE);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_RING), ToneGenerator.TONE_CDMA_NETWORK_USA_RINGBACK);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_INT), ToneGenerator.TONE_SUP_INTERCEPT);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_ABB_INT), ToneGenerator.TONE_SUP_INTERCEPT_ABBREV);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_REORDER), ToneGenerator.TONE_CDMA_REORDER);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_ABB_RE), ToneGenerator.TONE_CDMA_ABBR_REORDER);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_BUSY), ToneGenerator.TONE_CDMA_NETWORK_BUSY);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_CONFIRM), ToneGenerator.TONE_SUP_CONFIRM);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_ANSWER), ToneGenerator.TONE_CDMA_ANSWER);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_CALL_W), ToneGenerator.TONE_CDMA_NETWORK_CALLWAITING);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_PIP), ToneGenerator.TONE_CDMA_PIP);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_TONE, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_TONE_NO_TONE), ToneGenerator.TONE_CDMA_SIGNAL_OFF);
    //
    // /* SIGNAL_TYPE_IS54B */
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_L), ToneGenerator.TONE_CDMA_HIGH_L);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_L), ToneGenerator.TONE_CDMA_MED_L);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_L), ToneGenerator.TONE_CDMA_LOW_L);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_SS), ToneGenerator.TONE_CDMA_HIGH_SS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_SS), ToneGenerator.TONE_CDMA_MED_SS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_SS), ToneGenerator.TONE_CDMA_LOW_SS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_SSL), ToneGenerator.TONE_CDMA_HIGH_SSL);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_SSL), ToneGenerator.TONE_CDMA_MED_SSL);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_SSL), ToneGenerator.TONE_CDMA_LOW_SSL);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_SS_2), ToneGenerator.TONE_CDMA_HIGH_SS_2);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_SS_2), ToneGenerator.TONE_CDMA_MED_SS_2);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_SS_2), ToneGenerator.TONE_CDMA_LOW_SS_2);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_SLS), ToneGenerator.TONE_CDMA_HIGH_SLS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_SLS), ToneGenerator.TONE_CDMA_MED_SLS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_SLS), ToneGenerator.TONE_CDMA_LOW_SLS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_S_X4), ToneGenerator.TONE_CDMA_HIGH_S_X4);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_S_X4), ToneGenerator.TONE_CDMA_MED_S_X4);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_S_X4), ToneGenerator.TONE_CDMA_LOW_S_X4);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_PBX_L), ToneGenerator.TONE_CDMA_HIGH_PBX_L);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_PBX_L), ToneGenerator.TONE_CDMA_MED_PBX_L);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_PBX_L), ToneGenerator.TONE_CDMA_LOW_PBX_L);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SS), ToneGenerator.TONE_CDMA_HIGH_PBX_SS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SS), ToneGenerator.TONE_CDMA_MED_PBX_SS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SS), ToneGenerator.TONE_CDMA_LOW_PBX_SS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SSL), ToneGenerator.TONE_CDMA_HIGH_PBX_SSL);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SSL), ToneGenerator.TONE_CDMA_MED_PBX_SSL);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SSL), ToneGenerator.TONE_CDMA_LOW_PBX_SSL);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SLS), ToneGenerator.TONE_CDMA_HIGH_PBX_SLS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SLS), ToneGenerator.TONE_CDMA_MED_PBX_SLS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_PBX_SLS), ToneGenerator.TONE_CDMA_LOW_PBX_SLS);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_HIGH,
    //         IS95_CONST_IR_SIG_IS54B_PBX_S_X4), ToneGenerator.TONE_CDMA_HIGH_PBX_S_X4);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_MED,
    //         IS95_CONST_IR_SIG_IS54B_PBX_S_X4), ToneGenerator.TONE_CDMA_MED_PBX_S_X4);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, IS95_CONST_IR_ALERT_LOW,
    //         IS95_CONST_IR_SIG_IS54B_PBX_S_X4), ToneGenerator.TONE_CDMA_LOW_PBX_S_X4);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_IS54B, TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN,
    //         IS95_CONST_IR_SIG_IS54B_NO_TONE), ToneGenerator.TONE_CDMA_SIGNAL_OFF);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_USR_DEFD_ALERT,
    //         TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN, IS95_CONST_IR_SIG_TONE_ABBR_ALRT),
    //         ToneGenerator.TONE_CDMA_ABBR_ALERT);
    //
    // ret.put(signalParamHash(IS95_CONST_IR_SIGNAL_USR_DEFD_ALERT,
    //         TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN, IS95_CONST_IR_SIG_TONE_NO_TONE),
    //         ToneGenerator.TONE_CDMA_ABBR_ALERT);
    assert(0);
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
    // ==================before translated======================
    // if ((signalType < 0) || (signalType > 256) || (alertPitch > 256) ||
    //         (alertPitch < 0) || (signal > 256) || (signal < 0)) {
    //     return new Integer(CDMA_INVALID_TONE);
    // }
    // // Based on 3GPP2 C.S0005-E, seciton 3.7.5.5 Signal,
    // // the alert pitch field is ignored by the mobile station unless
    // // SIGNAL_TYPE is '10',IS-54B Alerting.
    // // Set alert pitch to TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN
    // // so the alert pitch is not involved in hash calculation
    // // when signal type is not IS-54B.
    // if (signalType != IS95_CONST_IR_SIGNAL_IS54B) {
    //     alertPitch = TAPIAMSSCDMA_SIGNAL_PITCH_UNKNOWN;
    // }
    // return new Integer(signalType * 256 * 256 + alertPitch * 256 + signal);
    assert(0);
    AutoPtr<IInteger32> empty;
    return empty;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
