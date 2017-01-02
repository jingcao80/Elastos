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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSCBHEADER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSCBHEADER_H__

#include "Elastos.Droid.Telephony.h"
#include "_Elastos_Droid_Internal_Telephony_Gsm_CSmsCbHeader.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telephony::ISmsCbEtwsInfo;
using Elastos::Droid::Telephony::ISmsCbCmasInfo;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * Parses a 3GPP TS 23.041 cell broadcast message header. This class is public for use by
 * CellBroadcastReceiver test cases, but should not be used by applications.
 *
 * All relevant header information is now sent as a Parcelable
 * {@link android.telephony.SmsCbMessage} object in the "message" extra of the
 * {@link android.provider.Telephony.Sms.Intents#SMS_CB_RECEIVED_ACTION} or
 * {@link android.provider.Telephony.Sms.Intents#SMS_EMERGENCY_CB_RECEIVED_ACTION} intent.
 * The raw PDU is no longer sent to SMS CB applications.
 */
CarClass(CSmsCbHeader)
    , public Object
    , public ISmsCbHeader
{
public:
    CSmsCbHeader();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pdu);

    CARAPI GetGeographicalScope(
        /* [out] */ Int32* result);

    CARAPI GetSerialNumber(
        /* [out] */ Int32* result);

    CARAPI GetServiceCategory(
        /* [out] */ Int32* result);

    CARAPI GetDataCodingScheme(
        /* [out] */ Int32* result);

    CARAPI GetPageIndex(
        /* [out] */ Int32* result);

    CARAPI GetNumberOfPages(
        /* [out] */ Int32* result);

    CARAPI GetEtwsInfo(
        /* [out] */ ISmsCbEtwsInfo** result);

    CARAPI GetCmasInfo(
        /* [out] */ ISmsCbCmasInfo** result);

    /**
     * Return whether this broadcast is an emergency (PWS) message type.
     * @return true if this message is emergency type; false otherwise
     */
    CARAPI IsEmergencyMessage(
        /* [out] */ Boolean* result);

    /**
     * Return whether this broadcast is an ETWS primary notification.
     * @return true if this message is an ETWS primary notification; false otherwise
     */
    CARAPI IsEtwsPrimaryNotification(
        /* [out] */ Boolean* result);

    /**
     * Return whether this broadcast is in UMTS format.
     * @return true if this message is in UMTS format; false otherwise
     */
    CARAPI IsUmtsFormat(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    /**
     * Return whether this broadcast is an ETWS emergency message type.
     * @return true if this message is ETWS emergency type; false otherwise
     */
    CARAPI_(Boolean) IsEtwsMessage();

    /**
     * Return whether this message is a CMAS emergency message type.
     * @return true if this message is CMAS emergency type; false otherwise
     */
    CARAPI_(Boolean) IsCmasMessage();

    /**
     * Return whether the popup alert flag is set for an ETWS warning notification.
     * This method assumes that the message ID has already been checked for ETWS type.
     *
     * @return true if the message code indicates a popup alert should be displayed
     */
    CARAPI_(Boolean) IsEtwsPopupAlert();

    /**
     * Return whether the emergency user alert flag is set for an ETWS warning notification.
     * This method assumes that the message ID has already been checked for ETWS type.
     *
     * @return true if the message code indicates an emergency user alert
     */
    CARAPI_(Boolean) IsEtwsEmergencyUserAlert();

    /**
     * Returns the warning type for an ETWS warning notification.
     * This method assumes that the message ID has already been checked for ETWS type.
     *
     * @return the ETWS warning type defined in 3GPP TS 23.041 section 9.3.24
     */
    CARAPI_(Int32) GetEtwsWarningType();

    /**
     * Returns the message class for a CMAS warning notification.
     * This method assumes that the message ID has already been checked for CMAS type.
     * @return the CMAS message class as defined in {@link SmsCbCmasInfo}
     */
    CARAPI_(Int32) GetCmasMessageClass();

    /**
     * Returns the severity for a CMAS warning notification. This is only available for extreme
     * and severe alerts, not for other types such as Presidential Level and AMBER alerts.
     * This method assumes that the message ID has already been checked for CMAS type.
     * @return the CMAS severity as defined in {@link SmsCbCmasInfo}
     */
    CARAPI_(Int32) GetCmasSeverity();

    /**
     * Returns the urgency for a CMAS warning notification. This is only available for extreme
     * and severe alerts, not for other types such as Presidential Level and AMBER alerts.
     * This method assumes that the message ID has already been checked for CMAS type.
     * @return the CMAS urgency as defined in {@link SmsCbCmasInfo}
     */
    CARAPI_(Int32) GetCmasUrgency();

    /**
     * Returns the certainty for a CMAS warning notification. This is only available for extreme
     * and severe alerts, not for other types such as Presidential Level and AMBER alerts.
     * This method assumes that the message ID has already been checked for CMAS type.
     * @return the CMAS certainty as defined in {@link SmsCbCmasInfo}
     */
    CARAPI_(Int32) GetCmasCertainty();

public:
    /**
     * Length of SMS-CB header
     */
    static const Int32 PDU_HEADER_LENGTH = 6;

    /**
     * GSM pdu format, as defined in 3gpp TS 23.041, section 9.4.1
     */
    static const Int32 FORMAT_GSM = 1;

    /**
     * UMTS pdu format, as defined in 3gpp TS 23.041, section 9.4.2
     */
    static const Int32 FORMAT_UMTS = 2;

    /**
     * GSM pdu format, as defined in 3gpp TS 23.041, section 9.4.1.3
     */
    static const Int32 FORMAT_ETWS_PRIMARY = 3;

private:
    /**
     * Message type value as defined in 3gpp TS 25.324, section 11.1.
     */
    static const Int32 MESSAGE_TYPE_CBS_MESSAGE = 1;

    /**
     * Length of GSM pdus
     */
    static const Int32 PDU_LENGTH_GSM = 88;

    /**
     * Maximum length of ETWS primary message GSM pdus
     */
    static const Int32 PDU_LENGTH_ETWS = 56;

    Int32 mGeographicalScope;

    /** The serial number combines geographical scope, message code, and update number. */
    Int32 mSerialNumber;

    /** The Message Identifier in 3GPP is the same as the Service Category in CDMA. */
    Int32 mMessageIdentifier;

    Int32 mDataCodingScheme;

    Int32 mPageIndex;

    Int32 mNrOfPages;

    Int32 mFormat;

    /** ETWS warning notification info. */
    AutoPtr<ISmsCbEtwsInfo> mEtwsInfo;

    /** CMAS warning notification info. */
    AutoPtr<ISmsCbCmasInfo> mCmasInfo;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSMSCBHEADER_H__
