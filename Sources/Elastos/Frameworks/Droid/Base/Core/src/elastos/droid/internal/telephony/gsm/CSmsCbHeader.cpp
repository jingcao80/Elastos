#include "elastos/droid/internal/telephony/gsm/CSmsCbHeader.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const Int32 CSmsCbHeader::PDU_HEADER_LENGTH;
const Int32 CSmsCbHeader::FORMAT_GSM;
const Int32 CSmsCbHeader::FORMAT_UMTS;
const Int32 CSmsCbHeader::FORMAT_ETWS_PRIMARY;
const Int32 CSmsCbHeader::MESSAGE_TYPE_CBS_MESSAGE;
const Int32 CSmsCbHeader::PDU_LENGTH_GSM;
const Int32 CSmsCbHeader::PDU_LENGTH_ETWS;

CAR_INTERFACE_IMPL(CSmsCbHeader, Object, ISmsCbHeader)

CAR_OBJECT_IMPL(CSmsCbHeader)

CSmsCbHeader::CSmsCbHeader()
    : mGeographicalScope(0)
    , mSerialNumber(0)
    , mMessageIdentifier(0)
    , mDataCodingScheme(0)
    , mPageIndex(0)
    , mNrOfPages(0)
    , mFormat(0)
{
}

ECode CSmsCbHeader::constructor()
{
    return NOERROR;
}

ECode CSmsCbHeader::constructor(
    /* [in] */ ArrayOf<Byte>* pdu)
{
    assert(0);
    // if (pdu == null || pdu.length < PDU_HEADER_LENGTH) {
    //     throw new IllegalArgumentException("Illegal PDU");
    // }

    // if (pdu.length <= PDU_LENGTH_GSM) {
    //     // can be ETWS or GSM format.
    //     // Per TS23.041 9.4.1.2 and 9.4.1.3.2, GSM and ETWS format both
    //     // contain serial number which contains GS, Message Code, and Update Number
    //     // per 9.4.1.2.1, and message identifier in same octets
    //     mGeographicalScope = (pdu[0] & 0xc0) >>> 6;
    //     mSerialNumber = ((pdu[0] & 0xff) << 8) | (pdu[1] & 0xff);
    //     mMessageIdentifier = ((pdu[2] & 0xff) << 8) | (pdu[3] & 0xff);
    //     if (isEtwsMessage() && pdu.length <= PDU_LENGTH_ETWS) {
    //         mFormat = FORMAT_ETWS_PRIMARY;
    //         mDataCodingScheme = -1;
    //         mPageIndex = -1;
    //         mNrOfPages = -1;
    //         boolean emergencyUserAlert = (pdu[4] & 0x1) != 0;
    //         boolean activatePopup = (pdu[5] & 0x80) != 0;
    //         int warningType = (pdu[4] & 0xfe) >>> 1;
    //         byte[] warningSecurityInfo;
    //         // copy the Warning-Security-Information, if present
    //         if (pdu.length > PDU_HEADER_LENGTH) {
    //             warningSecurityInfo = Arrays.copyOfRange(pdu, 6, pdu.length);
    //         } else {
    //             warningSecurityInfo = null;
    //         }
    //         mEtwsInfo = new SmsCbEtwsInfo(warningType, emergencyUserAlert, activatePopup,
    //                 warningSecurityInfo);
    //         mCmasInfo = null;
    //         return;     // skip the ETWS/CMAS initialization code for regular notifications
    //     } else {
    //         // GSM pdus are no more than 88 bytes
    //         mFormat = FORMAT_GSM;
    //         mDataCodingScheme = pdu[4] & 0xff;

    //         // Check for invalid page parameter
    //         int pageIndex = (pdu[5] & 0xf0) >>> 4;
    //         int nrOfPages = pdu[5] & 0x0f;

    //         if (pageIndex == 0 || nrOfPages == 0 || pageIndex > nrOfPages) {
    //             pageIndex = 1;
    //             nrOfPages = 1;
    //         }

    //         mPageIndex = pageIndex;
    //         mNrOfPages = nrOfPages;
    //     }
    // } else {
    //     // UMTS pdus are always at least 90 bytes since the payload includes
    //     // a number-of-pages octet and also one length octet per page
    //     mFormat = FORMAT_UMTS;

    //     int messageType = pdu[0];

    //     if (messageType != MESSAGE_TYPE_CBS_MESSAGE) {
    //         throw new IllegalArgumentException("Unsupported message type " + messageType);
    //     }

    //     mMessageIdentifier = ((pdu[1] & 0xff) << 8) | pdu[2] & 0xff;
    //     mGeographicalScope = (pdu[3] & 0xc0) >>> 6;
    //     mSerialNumber = ((pdu[3] & 0xff) << 8) | (pdu[4] & 0xff);
    //     mDataCodingScheme = pdu[5] & 0xff;

    //     // We will always consider a UMTS message as having one single page
    //     // since there's only one instance of the header, even though the
    //     // actual payload may contain several pages.
    //     mPageIndex = 1;
    //     mNrOfPages = 1;
    // }

    // if (isEtwsMessage()) {
    //     boolean emergencyUserAlert = isEtwsEmergencyUserAlert();
    //     boolean activatePopup = isEtwsPopupAlert();
    //     int warningType = getEtwsWarningType();
    //     mEtwsInfo = new SmsCbEtwsInfo(warningType, emergencyUserAlert, activatePopup, null);
    //     mCmasInfo = null;
    // } else if (isCmasMessage()) {
    //     int messageClass = getCmasMessageClass();
    //     int severity = getCmasSeverity();
    //     int urgency = getCmasUrgency();
    //     int certainty = getCmasCertainty();
    //     mEtwsInfo = null;
    //     mCmasInfo = new SmsCbCmasInfo(messageClass, SmsCbCmasInfo.CMAS_CATEGORY_UNKNOWN,
    //             SmsCbCmasInfo.CMAS_RESPONSE_TYPE_UNKNOWN, severity, urgency, certainty);
    // } else {
    //     mEtwsInfo = null;
    //     mCmasInfo = null;
    // }
    return NOERROR;
}

ECode CSmsCbHeader::GetGeographicalScope(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mGeographicalScope;
    return NOERROR;
}

ECode CSmsCbHeader::GetSerialNumber(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSerialNumber;
    return NOERROR;
}

ECode CSmsCbHeader::GetServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMessageIdentifier;
    return NOERROR;
}

ECode CSmsCbHeader::GetDataCodingScheme(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDataCodingScheme;
    return NOERROR;
}

ECode CSmsCbHeader::GetPageIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPageIndex;
    return NOERROR;
}

ECode CSmsCbHeader::GetNumberOfPages(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNrOfPages;
    return NOERROR;
}

ECode CSmsCbHeader::GetEtwsInfo(
    /* [out] */ ISmsCbEtwsInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEtwsInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSmsCbHeader::GetCmasInfo(
    /* [out] */ ISmsCbCmasInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCmasInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSmsCbHeader::IsEmergencyMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMessageIdentifier >= ISmsCbConstants::MESSAGE_ID_PWS_FIRST_IDENTIFIER
            && mMessageIdentifier <= ISmsCbConstants::MESSAGE_ID_PWS_LAST_IDENTIFIER;
    return NOERROR;
}

ECode CSmsCbHeader::IsEtwsPrimaryNotification(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFormat == FORMAT_ETWS_PRIMARY;
    return NOERROR;
}

ECode CSmsCbHeader::IsUmtsFormat(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFormat == FORMAT_UMTS;
    return NOERROR;
}

ECode CSmsCbHeader::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("SmsCbHeader{GS=") + mGeographicalScope + ", serialNumber=0x" +
            StringUtils::ToHexString(mSerialNumber) +
            ", messageIdentifier=0x" + StringUtils::ToHexString(mMessageIdentifier) +
            ", DCS=0x" + StringUtils::ToHexString(mDataCodingScheme) +
            ", page " + StringUtils::ToString(mPageIndex) +
            " of " + StringUtils::ToString(mNrOfPages) + '}';
    return NOERROR;
}

Boolean CSmsCbHeader::IsEtwsMessage()
{
    return (mMessageIdentifier & ISmsCbConstants::MESSAGE_ID_ETWS_TYPE_MASK)
            == ISmsCbConstants::MESSAGE_ID_ETWS_TYPE;
}

Boolean CSmsCbHeader::IsCmasMessage()
{
    return mMessageIdentifier >= ISmsCbConstants::MESSAGE_ID_CMAS_FIRST_IDENTIFIER
            && mMessageIdentifier <= ISmsCbConstants::MESSAGE_ID_CMAS_LAST_IDENTIFIER;
}

Boolean CSmsCbHeader::IsEtwsPopupAlert()
{
    return (mSerialNumber & ISmsCbConstants::SERIAL_NUMBER_ETWS_ACTIVATE_POPUP) != 0;
}

Boolean CSmsCbHeader::IsEtwsEmergencyUserAlert()
{
    return (mSerialNumber & ISmsCbConstants::SERIAL_NUMBER_ETWS_EMERGENCY_USER_ALERT) != 0;
}

Int32 CSmsCbHeader::GetEtwsWarningType()
{
    return mMessageIdentifier - ISmsCbConstants::MESSAGE_ID_ETWS_EARTHQUAKE_WARNING;
}

Int32 CSmsCbHeader::GetCmasMessageClass()
{
    switch (mMessageIdentifier) {
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_PRESIDENTIAL_LEVEL:
            return ISmsCbCmasInfo::CMAS_CLASS_PRESIDENTIAL_LEVEL_ALERT;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_LIKELY:
            return ISmsCbCmasInfo::CMAS_CLASS_EXTREME_THREAT;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_LIKELY:
            return ISmsCbCmasInfo::CMAS_CLASS_SEVERE_THREAT;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_CHILD_ABDUCTION_EMERGENCY:
            return ISmsCbCmasInfo::CMAS_CLASS_CHILD_ABDUCTION_EMERGENCY;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_REQUIRED_MONTHLY_TEST:
            return ISmsCbCmasInfo::CMAS_CLASS_REQUIRED_MONTHLY_TEST;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXERCISE:
            return ISmsCbCmasInfo::CMAS_CLASS_CMAS_EXERCISE;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_OPERATOR_DEFINED_USE:
            return ISmsCbCmasInfo::CMAS_CLASS_OPERATOR_DEFINED_USE;

        default:
            return ISmsCbCmasInfo::CMAS_CLASS_UNKNOWN;
    }
}

Int32 CSmsCbHeader::GetCmasSeverity()
{
    switch (mMessageIdentifier) {
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_LIKELY:
            return ISmsCbCmasInfo::CMAS_SEVERITY_EXTREME;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_LIKELY:
            return ISmsCbCmasInfo::CMAS_SEVERITY_SEVERE;

        default:
            return ISmsCbCmasInfo::CMAS_SEVERITY_UNKNOWN;
    }
}

Int32 CSmsCbHeader::GetCmasUrgency()
{
    switch (mMessageIdentifier) {
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_LIKELY:
            return ISmsCbCmasInfo::CMAS_URGENCY_IMMEDIATE;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_LIKELY:
            return ISmsCbCmasInfo::CMAS_URGENCY_EXPECTED;

        default:
            return ISmsCbCmasInfo::CMAS_URGENCY_UNKNOWN;
    }
}

Int32 CSmsCbHeader::GetCmasCertainty()
{
    switch (mMessageIdentifier) {
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_OBSERVED:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_OBSERVED:
            return ISmsCbCmasInfo::CMAS_CERTAINTY_OBSERVED;

        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_LIKELY:
        case ISmsCbConstants::MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_LIKELY:
            return ISmsCbCmasInfo::CMAS_CERTAINTY_LIKELY;

        default:
            return ISmsCbCmasInfo::CMAS_CERTAINTY_UNKNOWN;
    }
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos