#include "elastos/droid/internal/telephony/gsm/CSmsCbHeader.h"
#include "elastos/droid/telephony/CSmsCbCmasInfo.h"
#include "elastos/droid/telephony/CSmsCbEtwsInfo.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Telephony::CSmsCbCmasInfo;
using Elastos::Droid::Telephony::CSmsCbEtwsInfo;
using Elastos::Droid::Telephony::ISmsCbCmasInfo;
using Elastos::Droid::Telephony::ISmsCbEtwsInfo;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

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
    if (pdu == NULL || pdu->GetLength() < PDU_HEADER_LENGTH) {
        // throw new IllegalArgumentException("Illegal PDU");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (pdu->GetLength() <= PDU_LENGTH_GSM) {
        // can be ETWS or GSM format.
        // Per TS23.041 9.4.1.2 and 9.4.1.3.2, GSM and ETWS format both
        // contain serial number which contains GS, Message Code, and Update Number
        // per 9.4.1.2.1, and message identifier in same octets
        mGeographicalScope = ((*pdu)[0] & 0xc0) >> 6;
        mSerialNumber = (((*pdu)[0] & 0xff) << 8) | ((*pdu)[1] & 0xff);
        mMessageIdentifier = (((*pdu)[2] & 0xff) << 8) | ((*pdu)[3] & 0xff);
        if (IsEtwsMessage() && pdu->GetLength() <= PDU_LENGTH_ETWS) {
            mFormat = FORMAT_ETWS_PRIMARY;
            mDataCodingScheme = -1;
            mPageIndex = -1;
            mNrOfPages = -1;
            Boolean emergencyUserAlert = ((*pdu)[4] & 0x1) != 0;
            Boolean activatePopup = ((*pdu)[5] & 0x80) != 0;
            Int32 warningType = ((*pdu)[4] & 0xfe) >> 1;
            AutoPtr<ArrayOf<Byte> > warningSecurityInfo;
            // copy the Warning-Security-Information, if present
            if (pdu->GetLength() > PDU_HEADER_LENGTH) {
                Arrays::CopyOfRange(pdu, 6, pdu->GetLength(), (ArrayOf<Byte>**)&warningSecurityInfo);
            }
            else {
                warningSecurityInfo = NULL;
            }
            CSmsCbEtwsInfo::New(warningType, emergencyUserAlert, activatePopup, warningSecurityInfo, (ISmsCbEtwsInfo**)&mEtwsInfo);
            mCmasInfo = NULL;
            return NOERROR;     // skip the ETWS/CMAS initialization code for regular notifications
        }
        else {
            // GSM pdus are no more than 88 bytes
            mFormat = FORMAT_GSM;
            mDataCodingScheme = (*pdu)[4] & 0xff;

            // Check for invalid page parameter
            Int32 pageIndex = ((*pdu)[5] & 0xf0) >> 4;
            Int32 nrOfPages = (*pdu)[5] & 0x0f;

            if (pageIndex == 0 || nrOfPages == 0 || pageIndex > nrOfPages) {
                pageIndex = 1;
                nrOfPages = 1;
            }

            mPageIndex = pageIndex;
            mNrOfPages = nrOfPages;
        }
    }
    else {
        // UMTS pdus are always at least 90 bytes since the payload includes
        // a number-of-pages octet and also one length octet per page
        mFormat = FORMAT_UMTS;

        Int32 messageType = (*pdu)[0];

        if (messageType != MESSAGE_TYPE_CBS_MESSAGE) {
            // throw new IllegalArgumentException("Unsupported message type " + messageType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        mMessageIdentifier = (((*pdu)[1] & 0xff) << 8) | ((*pdu)[2] & 0xff);
        mGeographicalScope = ((*pdu)[3] & 0xc0) >> 6;
        mSerialNumber = (((*pdu)[3] & 0xff) << 8) | ((*pdu)[4] & 0xff);
        mDataCodingScheme = (*pdu)[5] & 0xff;

        // We will always consider a UMTS message as having one single page
        // since there's only one instance of the header, even though the
        // actual payload may contain several pages.
        mPageIndex = 1;
        mNrOfPages = 1;
    }

    if (IsEtwsMessage()) {
        Boolean emergencyUserAlert = IsEtwsEmergencyUserAlert();
        Boolean activatePopup = IsEtwsPopupAlert();
        Int32 warningType = GetEtwsWarningType();
        CSmsCbEtwsInfo::New(warningType, emergencyUserAlert, activatePopup, NULL, (ISmsCbEtwsInfo**)&mEtwsInfo);
        mCmasInfo = NULL;
    }
    else if (IsCmasMessage()) {
        Int32 messageClass = GetCmasMessageClass();
        Int32 severity = GetCmasSeverity();
        Int32 urgency = GetCmasUrgency();
        Int32 certainty = GetCmasCertainty();
        mEtwsInfo = NULL;
        CSmsCbCmasInfo::New(messageClass, ISmsCbCmasInfo::CMAS_CATEGORY_UNKNOWN,
                ISmsCbCmasInfo::CMAS_RESPONSE_TYPE_UNKNOWN, severity, urgency, certainty, (ISmsCbCmasInfo**)&mCmasInfo);
    }
    else {
        mEtwsInfo = NULL;
        mCmasInfo = NULL;
    }
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
    StringBuilder sb("SmsCbHeader{GS=");
    sb += mGeographicalScope; sb += ", serialNumber=0x";
    sb += StringUtils::ToHexString(mSerialNumber);
    sb += ", messageIdentifier=0x"; sb += StringUtils::ToHexString(mMessageIdentifier);
    sb += ", DCS=0x"; sb += StringUtils::ToHexString(mDataCodingScheme);
    sb += ", page "; sb += StringUtils::ToString(mPageIndex);
    sb += " of "; sb += StringUtils::ToString(mNrOfPages);
    sb += "}";
    *result = sb.ToString();
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