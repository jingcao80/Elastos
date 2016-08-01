#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaInformationRecords.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//              CdmaInformationRecords::CdmaSignalInfoRec
//=====================================================================
CdmaInformationRecords::CdmaSignalInfoRec::CdmaSignalInfoRec()
    : isPresent(FALSE)
    , signalType(0)
    , alertPitch(0)
    , signal(0)
{
}

CdmaInformationRecords::CdmaSignalInfoRec::CdmaSignalInfoRec(
    /* [in] */ Int32 isPresent,
    /* [in] */ Int32 signalType,
    /* [in] */ Int32 alertPitch,
    /* [in] */ Int32 signal)
{
    this->isPresent = isPresent != 0;
    this->signalType = signalType;
    this->alertPitch = alertPitch;
    this->signal = signal;
}

ECode CdmaInformationRecords::CdmaSignalInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("CdmaSignalInfo: {") +
            " isPresent: " + StringUtils::BooleanToString(isPresent) +
            ", signalType: " + StringUtils::ToString(signalType) +
            ", alertPitch: " + StringUtils::ToString(alertPitch) +
            ", signal: " + StringUtils::ToString(signal) +
            " }";
    return NOERROR;
}

//=====================================================================
//              CdmaInformationRecords::CdmaDisplayInfoRec
//=====================================================================
CdmaInformationRecords::CdmaDisplayInfoRec::CdmaDisplayInfoRec(
    /* [in] */ Int32 id,
    /* [in] */ const String& alpha)
{
    this->id = id;
    this->alpha = alpha;
}

ECode CdmaInformationRecords::CdmaDisplayInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("CdmaDisplayInfoRec: {") +
            " id: " + CdmaInformationRecords::IdToString(id) +
            ", alpha: " + alpha +
            " }";
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
    this->number = number;
    this->numberType = (Byte)numberType;
    this->numberPlan = (Byte)numberPlan;
    this->pi = (Byte)pi;
    this->si = (Byte)si;
}

ECode CdmaInformationRecords::CdmaNumberInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("CdmaNumberInfoRec: {") +
            " id: " + CdmaInformationRecords::IdToString(id) +
            ", number: " + number +
            ", numberType: " + StringUtils::ToString(numberType) +
            ", numberPlan: " + StringUtils::ToString(numberPlan) +
            ", pi: " + StringUtils::ToString(pi) +
            ", si: " + StringUtils::ToString(si) +
            " }";
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
    : redirectingReason(reason)
{
    numberInfoRec = new CdmaNumberInfoRec(RIL_CDMA_REDIRECTING_NUMBER_INFO_REC,
                                          number, numberType, numberPlan, pi, si);
}

ECode CdmaInformationRecords::CdmaRedirectingNumberInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String str;
    numberInfoRec->ToString(&str);
    *result = String("CdmaNumberInfoRec: {") +
            " numberInfoRec: " + str +
            ", redirectingReason: " + StringUtils::ToString(redirectingReason) +
            " }";
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
    this->lineCtrlPolarityIncluded = (Byte)lineCtrlPolarityIncluded;
    this->lineCtrlToggle = (Byte)lineCtrlToggle;
    this->lineCtrlReverse = (Byte)lineCtrlReverse;
    this->lineCtrlPowerDenial = (Byte)lineCtrlPowerDenial;
}

ECode CdmaInformationRecords::CdmaLineControlInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("CdmaLineControlInfoRec: {") +
            " lineCtrlPolarityIncluded: " + StringUtils::ToString(lineCtrlPolarityIncluded) +
            " lineCtrlToggle: " + StringUtils::ToString(lineCtrlToggle) +
            " lineCtrlReverse: " + StringUtils::ToString(lineCtrlReverse) +
            " lineCtrlPowerDenial: " + StringUtils::ToString(lineCtrlPowerDenial) +
            " }";
    return NOERROR;
}

//=====================================================================
//              CdmaInformationRecords::CdmaT53ClirInfoRec
//=====================================================================
CdmaInformationRecords::CdmaT53ClirInfoRec::CdmaT53ClirInfoRec(
    /* [in] */ Int32 cause)
{
    this->cause = (Byte)cause;
}

ECode CdmaInformationRecords::CdmaT53ClirInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("CdmaT53ClirInfoRec: {") +
            " cause: " + StringUtils::ToString(cause) +
            " }";
    return NOERROR;
}

//=====================================================================
//          CdmaInformationRecords::CdmaT53AudioControlInfoRec
//=====================================================================
CdmaInformationRecords::CdmaT53AudioControlInfoRec::CdmaT53AudioControlInfoRec(
    /* [in] */ Int32 uplink,
    /* [in] */ Int32 downlink)
{
    this->uplink = (Byte) uplink;
    this->downlink = (Byte) downlink;
}

ECode CdmaInformationRecords::CdmaT53AudioControlInfoRec::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("CdmaT53AudioControlInfoRec: {") +
            " uplink: " + StringUtils::ToString(uplink) +
            " downlink: " + StringUtils::ToString(downlink) +
            " }";
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
    Int32 id;
    p->ReadInt32(&id);
    switch (id) {
        case RIL_CDMA_DISPLAY_INFO_REC:
        case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC: {
            String str;
            p->ReadString(&str);
            record  = new CdmaDisplayInfoRec(id, str);
            break;
        }
        case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
        case RIL_CDMA_CONNECTED_NUMBER_INFO_REC: {
            String str;
            p->ReadString(&str);
            Int32 val1, val2, val3, val4;
            p->ReadInt32(&val1);
            p->ReadInt32(&val2);
            p->ReadInt32(&val3);
            p->ReadInt32(&val4);
            record = new CdmaNumberInfoRec(id, str, val1, val2, val3, val4);
            break;
        }
        case RIL_CDMA_SIGNAL_INFO_REC: {
            Int32 val1, val2, val3, val4;
            p->ReadInt32(&val1);
            p->ReadInt32(&val2);
            p->ReadInt32(&val3);
            p->ReadInt32(&val4);
            record = new CdmaSignalInfoRec(val1, val2, val3, val4);
            break;
        }
        case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC: {
            String str;
            p->ReadString(&str);
            Int32 val1, val2, val3, val4 ,val5;
            p->ReadInt32(&val1);
            p->ReadInt32(&val2);
            p->ReadInt32(&val3);
            p->ReadInt32(&val4);
            p->ReadInt32(&val5);
            record = new CdmaRedirectingNumberInfoRec(str, val1, val2, val3, val4, val5);
            break;
        }
        case RIL_CDMA_LINE_CONTROL_INFO_REC: {
            Int32 val1, val2, val3, val4;
            p->ReadInt32(&val1);
            p->ReadInt32(&val2);
            p->ReadInt32(&val3);
            p->ReadInt32(&val4);
            record = new CdmaLineControlInfoRec(val1, val2, val3, val4);
            break;
        }
        case RIL_CDMA_T53_CLIR_INFO_REC: {
            Int32 val1;
            p->ReadInt32(&val1);
            record = new CdmaT53ClirInfoRec(val1);
            break;
        }
        case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC: {
            Int32 val1, val2;
            p->ReadInt32(&val1);
            p->ReadInt32(&val2);
            record = new CdmaT53AudioControlInfoRec(val1, val2);
            break;
        }
        case RIL_CDMA_T53_RELEASE_INFO_REC:
            // TODO: WHAT to do, for now fall through and throw exception
        default:
            // throw new RuntimeException("RIL_UNSOL_CDMA_INFO_REC: unsupported record. Got "
            //                             + CdmaInformationRecords.idToString(id) + " ");
            return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

String CdmaInformationRecords::IdToString(
    /* [in] */ Int32 id)
{
    switch(id) {
    case RIL_CDMA_DISPLAY_INFO_REC: return String("RIL_CDMA_DISPLAY_INFO_REC");
    case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC: return String("RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC");
    case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC: return String("RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC");
    case RIL_CDMA_CONNECTED_NUMBER_INFO_REC: return String("RIL_CDMA_CONNECTED_NUMBER_INFO_REC");
    case RIL_CDMA_SIGNAL_INFO_REC: return String("RIL_CDMA_SIGNAL_INFO_REC");
    case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC: return String("RIL_CDMA_REDIRECTING_NUMBER_INFO_REC");
    case RIL_CDMA_LINE_CONTROL_INFO_REC: return String("RIL_CDMA_LINE_CONTROL_INFO_REC");
    case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC: return String("RIL_CDMA_EXTENDED_DISPLAY_INFO_REC");
    case RIL_CDMA_T53_CLIR_INFO_REC: return String("RIL_CDMA_T53_CLIR_INFO_REC");
    case RIL_CDMA_T53_RELEASE_INFO_REC: return String("RIL_CDMA_T53_RELEASE_INFO_REC");
    case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC: return String("RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC");
    default: return String("<unknown record>");
    }
    return String("");
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
