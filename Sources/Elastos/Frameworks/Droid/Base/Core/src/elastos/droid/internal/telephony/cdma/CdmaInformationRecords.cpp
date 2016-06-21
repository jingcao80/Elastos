#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaInformationRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//              CdmaInformationRecords::CdmaSignalInfoRec
//=====================================================================
CdmaInformationRecords::CdmaSignalInfoRec::CdmaSignalInfoRec()
{
}

CdmaInformationRecords::CdmaSignalInfoRec::CdmaSignalInfoRec(
    /* [in] */ Int32 isPresent,
    /* [in] */ Int32 signalType,
    /* [in] */ Int32 alertPitch,
    /* [in] */ Int32 signal)
{
    // ==================before translated======================
    // this.isPresent = isPresent != 0;
    // this.signalType = signalType;
    // this.alertPitch = alertPitch;
    // this.signal = signal;
}

ECode CdmaInformationRecords::CdmaSignalInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "CdmaSignalInfo: {" +
    //         " isPresent: " + isPresent +
    //         ", signalType: " + signalType +
    //         ", alertPitch: " + alertPitch +
    //         ", signal: " + signal +
    //         " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//              CdmaInformationRecords::CdmaDisplayInfoRec
//=====================================================================
CdmaInformationRecords::CdmaDisplayInfoRec::CdmaDisplayInfoRec(
    /* [in] */ Int32 id,
    /* [in] */ const String& alpha)
{
    // ==================before translated======================
    // this.id = id;
    // this.alpha = alpha;
}

ECode CdmaInformationRecords::CdmaDisplayInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "CdmaDisplayInfoRec: {" +
    //         " id: " + CdmaInformationRecords.idToString(id) +
    //         ", alpha: " + alpha +
    //         " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//              CdmaInformationRecords::CdmaNumberInfoRec
//=====================================================================
CdmaInformationRecords::CdmaNumberInfoRec::CdmaNumberInfoRec(
    /* [in] */ Int32 id,
    /* [in] */ const String& number,
    /* [in] */ Int32 numberType,
    /* [in] */ Int32 numberPlan,
    /* [in] */ Int32 pi,
    /* [in] */ Int32 si)
{
    // ==================before translated======================
    // this.number = number;
    // this.numberType = (byte)numberType;
    // this.numberPlan = (byte)numberPlan;
    // this.pi = (byte)pi;
    // this.si = (byte)si;
}

ECode CdmaInformationRecords::CdmaNumberInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "CdmaNumberInfoRec: {" +
    //         " id: " + CdmaInformationRecords.idToString(id) +
    //         ", number: " + number +
    //         ", numberType: " + numberType +
    //         ", numberPlan: " + numberPlan +
    //         ", pi: " + pi +
    //         ", si: " + si +
    //         " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//         CdmaInformationRecords::CdmaRedirectingNumberInfoRec
//=====================================================================
const Int32 CdmaInformationRecords::CdmaRedirectingNumberInfoRec::REASON_UNKNOWN;
const Int32 CdmaInformationRecords::CdmaRedirectingNumberInfoRec::REASON_CALL_FORWARDING_BUSY;
const Int32 CdmaInformationRecords::CdmaRedirectingNumberInfoRec::REASON_CALL_FORWARDING_NO_REPLY;
const Int32 CdmaInformationRecords::CdmaRedirectingNumberInfoRec::REASON_CALLED_DTE_OUT_OF_ORDER;
const Int32 CdmaInformationRecords::CdmaRedirectingNumberInfoRec::REASON_CALL_FORWARDING_BY_THE_CALLED_DTE;
const Int32 CdmaInformationRecords::CdmaRedirectingNumberInfoRec::REASON_CALL_FORWARDING_UNCONDITIONAL;

CdmaInformationRecords::CdmaRedirectingNumberInfoRec::CdmaRedirectingNumberInfoRec(
    /* [in] */ const String& number,
    /* [in] */ Int32 numberType,
    /* [in] */ Int32 numberPlan,
    /* [in] */ Int32 pi,
    /* [in] */ Int32 si,
    /* [in] */ Int32 reason)
{
    // ==================before translated======================
    // numberInfoRec = new CdmaNumberInfoRec(RIL_CDMA_REDIRECTING_NUMBER_INFO_REC,
    //                                       number, numberType, numberPlan, pi, si);
    // redirectingReason = reason;
}

ECode CdmaInformationRecords::CdmaRedirectingNumberInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "CdmaNumberInfoRec: {" +
    //         " numberInfoRec: " + numberInfoRec +
    //         ", redirectingReason: " + redirectingReason +
    //         " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//            CdmaInformationRecords::CdmaLineControlInfoRec
//=====================================================================
CdmaInformationRecords::CdmaLineControlInfoRec::CdmaLineControlInfoRec(
    /* [in] */ Int32 lineCtrlPolarityIncluded,
    /* [in] */ Int32 lineCtrlToggle,
    /* [in] */ Int32 lineCtrlReverse,
    /* [in] */ Int32 lineCtrlPowerDenial)
{
    // ==================before translated======================
    // this.lineCtrlPolarityIncluded = (byte)lineCtrlPolarityIncluded;
    // this.lineCtrlToggle = (byte)lineCtrlToggle;
    // this.lineCtrlReverse = (byte)lineCtrlReverse;
    // this.lineCtrlPowerDenial = (byte)lineCtrlPowerDenial;
}

ECode CdmaInformationRecords::CdmaLineControlInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "CdmaLineControlInfoRec: {" +
    //         " lineCtrlPolarityIncluded: " + lineCtrlPolarityIncluded +
    //         " lineCtrlToggle: " + lineCtrlToggle +
    //         " lineCtrlReverse: " + lineCtrlReverse +
    //         " lineCtrlPowerDenial: " + lineCtrlPowerDenial +
    //         " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//              CdmaInformationRecords::CdmaT53ClirInfoRec
//=====================================================================
CdmaInformationRecords::CdmaT53ClirInfoRec::CdmaT53ClirInfoRec(
    /* [in] */ Int32 cause)
{
    // ==================before translated======================
    // this.cause = (byte)cause;
}

ECode CdmaInformationRecords::CdmaT53ClirInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "CdmaT53ClirInfoRec: {" +
    //         " cause: " + cause +
    //         " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//          CdmaInformationRecords::CdmaT53AudioControlInfoRec
//=====================================================================
CdmaInformationRecords::CdmaT53AudioControlInfoRec::CdmaT53AudioControlInfoRec(
    /* [in] */ Int32 uplink,
    /* [in] */ Int32 downlink)
{
    // ==================before translated======================
    // this.uplink = (byte) uplink;
    // this.downlink = (byte) downlink;
}

ECode CdmaInformationRecords::CdmaT53AudioControlInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "CdmaT53AudioControlInfoRec: {" +
    //         " uplink: " + uplink +
    //         " downlink: " + downlink +
    //         " }";
    assert(0);
    return NOERROR;
}

//=====================================================================
//                        CdmaInformationRecords
//=====================================================================
CAR_INTERFACE_IMPL(CdmaInformationRecords, Object, ICdmaInformationRecords);

CdmaInformationRecords::CdmaInformationRecords()
{
}

ECode CdmaInformationRecords::constructor(
    /* [in] */ IParcel* p)
{
    // ==================before translated======================
    // int id = p.readInt();
    // switch (id) {
    //     case RIL_CDMA_DISPLAY_INFO_REC:
    //     case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC:
    //         record  = new CdmaDisplayInfoRec(id, p.readString());
    //         break;
    //
    //     case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
    //     case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
    //     case RIL_CDMA_CONNECTED_NUMBER_INFO_REC:
    //         record = new CdmaNumberInfoRec(id, p.readString(), p.readInt(), p.readInt(),
    //                 p.readInt(), p.readInt());
    //         break;
    //
    //     case RIL_CDMA_SIGNAL_INFO_REC:
    //         record = new CdmaSignalInfoRec(p.readInt(), p.readInt(), p.readInt(), p.readInt());
    //         break;
    //
    //     case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
    //         record = new CdmaRedirectingNumberInfoRec(p.readString(), p.readInt(), p.readInt(),
    //                 p.readInt(), p.readInt(), p.readInt());
    //         break;
    //
    //     case RIL_CDMA_LINE_CONTROL_INFO_REC:
    //         record = new CdmaLineControlInfoRec(p.readInt(), p.readInt(), p.readInt(),
    //                 p.readInt());
    //         break;
    //
    //     case RIL_CDMA_T53_CLIR_INFO_REC:
    //         record = new CdmaT53ClirInfoRec(p.readInt());
    //         break;
    //
    //     case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC:
    //         record = new CdmaT53AudioControlInfoRec(p.readInt(), p.readInt());
    //         break;
    //
    //     case RIL_CDMA_T53_RELEASE_INFO_REC:
    //         // TODO: WHAT to do, for now fall through and throw exception
    //     default:
    //         throw new RuntimeException("RIL_UNSOL_CDMA_INFO_REC: unsupported record. Got "
    //                                     + CdmaInformationRecords.idToString(id) + " ");
    //
    // }
    return NOERROR;
}

String CdmaInformationRecords::IdToString(
    /* [in] */ Int32 id)
{
    // ==================before translated======================
    // switch(id) {
    // case RIL_CDMA_DISPLAY_INFO_REC: return "RIL_CDMA_DISPLAY_INFO_REC";
    // case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC: return "RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC";
    // case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC: return "RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC";
    // case RIL_CDMA_CONNECTED_NUMBER_INFO_REC: return "RIL_CDMA_CONNECTED_NUMBER_INFO_REC";
    // case RIL_CDMA_SIGNAL_INFO_REC: return "RIL_CDMA_SIGNAL_INFO_REC";
    // case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC: return "RIL_CDMA_REDIRECTING_NUMBER_INFO_REC";
    // case RIL_CDMA_LINE_CONTROL_INFO_REC: return "RIL_CDMA_LINE_CONTROL_INFO_REC";
    // case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC: return "RIL_CDMA_EXTENDED_DISPLAY_INFO_REC";
    // case RIL_CDMA_T53_CLIR_INFO_REC: return "RIL_CDMA_T53_CLIR_INFO_REC";
    // case RIL_CDMA_T53_RELEASE_INFO_REC: return "RIL_CDMA_T53_RELEASE_INFO_REC";
    // case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC: return "RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC";
    // default: return "<unknown record>";
    // }
    assert(0);
    return String("");
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
