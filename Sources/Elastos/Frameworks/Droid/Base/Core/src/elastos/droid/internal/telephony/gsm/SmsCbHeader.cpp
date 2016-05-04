/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.gsm;

using Elastos::Droid::Telephony::ISmsCbCmasInfo;
using Elastos::Droid::Telephony::ISmsCbEtwsInfo;

using Elastos::Utility::IArrays;

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
class SmsCbHeader {

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

    /**
     * Message type value as defined in 3gpp TS 25.324, section 11.1.
     */
    private static const Int32 MESSAGE_TYPE_CBS_MESSAGE = 1;

    /**
     * Length of GSM pdus
     */
    private static const Int32 PDU_LENGTH_GSM = 88;

    /**
     * Maximum length of ETWS primary message GSM pdus
     */
    private static const Int32 PDU_LENGTH_ETWS = 56;

    private final Int32 mGeographicalScope;

    /** The serial number combines geographical scope, message code, and update number. */
    private final Int32 mSerialNumber;

    /** The Message Identifier in 3GPP is the same as the Service Category in CDMA. */
    private final Int32 mMessageIdentifier;

    private final Int32 mDataCodingScheme;

    private final Int32 mPageIndex;

    private final Int32 mNrOfPages;

    private final Int32 mFormat;

    /** ETWS warning notification info. */
    private final SmsCbEtwsInfo mEtwsInfo;

    /** CMAS warning notification info. */
    private final SmsCbCmasInfo mCmasInfo;

    public SmsCbHeader(Byte[] pdu) throws IllegalArgumentException {
        If (pdu == NULL || pdu.length < PDU_HEADER_LENGTH) {
            throw new IllegalArgumentException("Illegal PDU");
        }

        If (pdu.length <= PDU_LENGTH_GSM) {
            // can be ETWS or GSM format.
            // Per TS23.041 9.4.1.2 and 9.4.1.3.2, GSM and ETWS format both
            // contain serial number which contains GS, Message Code, and Update Number
            // per 9.4.1.2.1, and message identifier in same octets
            mGeographicalScope = (pdu[0] & 0xc0) >>> 6;
            mSerialNumber = ((pdu[0] & 0xff) << 8) | (pdu[1] & 0xff);
            mMessageIdentifier = ((pdu[2] & 0xff) << 8) | (pdu[3] & 0xff);
            If (IsEtwsMessage() && pdu.length <= PDU_LENGTH_ETWS) {
                mFormat = FORMAT_ETWS_PRIMARY;
                mDataCodingScheme = -1;
                mPageIndex = -1;
                mNrOfPages = -1;
                Boolean emergencyUserAlert = (pdu[4] & 0x1) != 0;
                Boolean activatePopup = (pdu[5] & 0x80) != 0;
                Int32 warningType = (pdu[4] & 0xfe) >>> 1;
                Byte[] warningSecurityInfo;
                // copy the Warning-Security-Information, if present
                If (pdu.length > PDU_HEADER_LENGTH) {
                    warningSecurityInfo = Arrays->CopyOfRange(pdu, 6, pdu.length);
                } else {
                    warningSecurityInfo = NULL;
                }
                mEtwsInfo = new SmsCbEtwsInfo(warningType, emergencyUserAlert, activatePopup,
                        warningSecurityInfo);
                mCmasInfo = NULL;
                return;     // skip the ETWS/CMAS initialization code for regular notifications
            } else {
                // GSM pdus are no more than 88 bytes
                mFormat = FORMAT_GSM;
                mDataCodingScheme = pdu[4] & 0xff;

                // Check for invalid page parameter
                Int32 pageIndex = (pdu[5] & 0xf0) >>> 4;
                Int32 nrOfPages = pdu[5] & 0x0f;

                If (pageIndex == 0 || nrOfPages == 0 || pageIndex > nrOfPages) {
                    pageIndex = 1;
                    nrOfPages = 1;
                }

                mPageIndex = pageIndex;
                mNrOfPages = nrOfPages;
            }
        } else {
            // UMTS pdus are always at least 90 bytes since the payload includes
            // a number-of-pages octet and also one length octet per page
            mFormat = FORMAT_UMTS;

            Int32 messageType = pdu[0];

            If (messageType != MESSAGE_TYPE_CBS_MESSAGE) {
                throw new IllegalArgumentException("Unsupported message type " + messageType);
            }

            mMessageIdentifier = ((pdu[1] & 0xff) << 8) | pdu[2] & 0xff;
            mGeographicalScope = (pdu[3] & 0xc0) >>> 6;
            mSerialNumber = ((pdu[3] & 0xff) << 8) | (pdu[4] & 0xff);
            mDataCodingScheme = pdu[5] & 0xff;

            // We will always consider a UMTS message as having one single page
            // since there's only one instance of the header, even though the
            // actual payload may contain several pages.
            mPageIndex = 1;
            mNrOfPages = 1;
        }

        If (IsEtwsMessage()) {
            Boolean emergencyUserAlert = IsEtwsEmergencyUserAlert();
            Boolean activatePopup = IsEtwsPopupAlert();
            Int32 warningType = GetEtwsWarningType();
            mEtwsInfo = new SmsCbEtwsInfo(warningType, emergencyUserAlert, activatePopup, NULL);
            mCmasInfo = NULL;
        } else If (IsCmasMessage()) {
            Int32 messageClass = GetCmasMessageClass();
            Int32 severity = GetCmasSeverity();
            Int32 urgency = GetCmasUrgency();
            Int32 certainty = GetCmasCertainty();
            mEtwsInfo = NULL;
            mCmasInfo = new SmsCbCmasInfo(messageClass, SmsCbCmasInfo.CMAS_CATEGORY_UNKNOWN,
                    SmsCbCmasInfo.CMAS_RESPONSE_TYPE_UNKNOWN, severity, urgency, certainty);
        } else {
            mEtwsInfo = NULL;
            mCmasInfo = NULL;
        }
    }

    Int32 GetGeographicalScope() {
        return mGeographicalScope;
    }

    Int32 GetSerialNumber() {
        return mSerialNumber;
    }

    Int32 GetServiceCategory() {
        return mMessageIdentifier;
    }

    Int32 GetDataCodingScheme() {
        return mDataCodingScheme;
    }

    Int32 GetPageIndex() {
        return mPageIndex;
    }

    Int32 GetNumberOfPages() {
        return mNrOfPages;
    }

    SmsCbEtwsInfo GetEtwsInfo() {
        return mEtwsInfo;
    }

    SmsCbCmasInfo GetCmasInfo() {
        return mCmasInfo;
    }

    /**
     * Return whether this broadcast is an Emergency (PWS) message type.
     * @return TRUE if this message is emergency type; FALSE otherwise
     */
    Boolean IsEmergencyMessage() {
        return mMessageIdentifier >= SmsCbConstants.MESSAGE_ID_PWS_FIRST_IDENTIFIER
                && mMessageIdentifier <= SmsCbConstants.MESSAGE_ID_PWS_LAST_IDENTIFIER;
    }

    /**
     * Return whether this broadcast is an ETWS emergency message type.
     * @return TRUE if this message is ETWS emergency type; FALSE otherwise
     */
    private Boolean IsEtwsMessage() {
        Return (mMessageIdentifier & SmsCbConstants.MESSAGE_ID_ETWS_TYPE_MASK)
                == SmsCbConstants.MESSAGE_ID_ETWS_TYPE;
    }

    /**
     * Return whether this broadcast is an ETWS primary notification.
     * @return TRUE if this message is an ETWS primary notification; FALSE otherwise
     */
    Boolean IsEtwsPrimaryNotification() {
        return mFormat == FORMAT_ETWS_PRIMARY;
    }

    /**
     * Return whether this broadcast is in UMTS format.
     * @return TRUE if this message is in UMTS format; FALSE otherwise
     */
    Boolean IsUmtsFormat() {
        return mFormat == FORMAT_UMTS;
    }

    /**
     * Return whether this message is a CMAS emergency message type.
     * @return TRUE if this message is CMAS emergency type; FALSE otherwise
     */
    private Boolean IsCmasMessage() {
        return mMessageIdentifier >= SmsCbConstants.MESSAGE_ID_CMAS_FIRST_IDENTIFIER
                && mMessageIdentifier <= SmsCbConstants.MESSAGE_ID_CMAS_LAST_IDENTIFIER;
    }

    /**
     * Return whether the popup alert flag is set for an ETWS warning notification.
     * This method assumes that the message ID has already been checked for ETWS type.
     *
     * @return TRUE if the message code indicates a popup alert should be displayed
     */
    private Boolean IsEtwsPopupAlert() {
        Return (mSerialNumber & SmsCbConstants.SERIAL_NUMBER_ETWS_ACTIVATE_POPUP) != 0;
    }

    /**
     * Return whether the emergency user alert flag is set for an ETWS warning notification.
     * This method assumes that the message ID has already been checked for ETWS type.
     *
     * @return TRUE if the message code indicates an emergency user alert
     */
    private Boolean IsEtwsEmergencyUserAlert() {
        Return (mSerialNumber & SmsCbConstants.SERIAL_NUMBER_ETWS_EMERGENCY_USER_ALERT) != 0;
    }

    /**
     * Returns the warning type for an ETWS warning notification.
     * This method assumes that the message ID has already been checked for ETWS type.
     *
     * @return the ETWS warning type defined in 3GPP TS 23.041 section 9.3.24
     */
    private Int32 GetEtwsWarningType() {
        return mMessageIdentifier - SmsCbConstants.MESSAGE_ID_ETWS_EARTHQUAKE_WARNING;
    }

    /**
     * Returns the message class for a CMAS warning notification.
     * This method assumes that the message ID has already been checked for CMAS type.
     * @return the CMAS message class as defined in {@link SmsCbCmasInfo}
     */
    private Int32 GetCmasMessageClass() {
        Switch (mMessageIdentifier) {
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_PRESIDENTIAL_LEVEL:
                return SmsCbCmasInfo.CMAS_CLASS_PRESIDENTIAL_LEVEL_ALERT;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_LIKELY:
                return SmsCbCmasInfo.CMAS_CLASS_EXTREME_THREAT;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_LIKELY:
                return SmsCbCmasInfo.CMAS_CLASS_SEVERE_THREAT;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_CHILD_ABDUCTION_EMERGENCY:
                return SmsCbCmasInfo.CMAS_CLASS_CHILD_ABDUCTION_EMERGENCY;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_REQUIRED_MONTHLY_TEST:
                return SmsCbCmasInfo.CMAS_CLASS_REQUIRED_MONTHLY_TEST;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXERCISE:
                return SmsCbCmasInfo.CMAS_CLASS_CMAS_EXERCISE;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_OPERATOR_DEFINED_USE:
                return SmsCbCmasInfo.CMAS_CLASS_OPERATOR_DEFINED_USE;

            default:
                return SmsCbCmasInfo.CMAS_CLASS_UNKNOWN;
        }
    }

    /**
     * Returns the severity for a CMAS warning notification. This is only available for extreme
     * and severe alerts, not for other types such as Presidential Level and AMBER alerts.
     * This method assumes that the message ID has already been checked for CMAS type.
     * @return the CMAS severity as defined in {@link SmsCbCmasInfo}
     */
    private Int32 GetCmasSeverity() {
        Switch (mMessageIdentifier) {
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_LIKELY:
                return SmsCbCmasInfo.CMAS_SEVERITY_EXTREME;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_LIKELY:
                return SmsCbCmasInfo.CMAS_SEVERITY_SEVERE;

            default:
                return SmsCbCmasInfo.CMAS_SEVERITY_UNKNOWN;
        }
    }

    /**
     * Returns the urgency for a CMAS warning notification. This is only available for extreme
     * and severe alerts, not for other types such as Presidential Level and AMBER alerts.
     * This method assumes that the message ID has already been checked for CMAS type.
     * @return the CMAS urgency as defined in {@link SmsCbCmasInfo}
     */
    private Int32 GetCmasUrgency() {
        Switch (mMessageIdentifier) {
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_LIKELY:
                return SmsCbCmasInfo.CMAS_URGENCY_IMMEDIATE;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_LIKELY:
                return SmsCbCmasInfo.CMAS_URGENCY_EXPECTED;

            default:
                return SmsCbCmasInfo.CMAS_URGENCY_UNKNOWN;
        }
    }

    /**
     * Returns the certainty for a CMAS warning notification. This is only available for extreme
     * and severe alerts, not for other types such as Presidential Level and AMBER alerts.
     * This method assumes that the message ID has already been checked for CMAS type.
     * @return the CMAS certainty as defined in {@link SmsCbCmasInfo}
     */
    private Int32 GetCmasCertainty() {
        Switch (mMessageIdentifier) {
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_OBSERVED:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_OBSERVED:
                return SmsCbCmasInfo.CMAS_CERTAINTY_OBSERVED;

            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_IMMEDIATE_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_EXTREME_EXPECTED_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_IMMEDIATE_LIKELY:
            case SmsCbConstants.MESSAGE_ID_CMAS_ALERT_SEVERE_EXPECTED_LIKELY:
                return SmsCbCmasInfo.CMAS_CERTAINTY_LIKELY;

            default:
                return SmsCbCmasInfo.CMAS_CERTAINTY_UNKNOWN;
        }
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "SmsCbHeader{GS=" + mGeographicalScope + ", serialNumber=0x" +
                Integer->ToHexString(mSerialNumber) +
                ", messageIdentifier=0x" + Integer->ToHexString(mMessageIdentifier) +
                ", DCS=0x" + Integer->ToHexString(mDataCodingScheme) +
                ", page " + mPageIndex + " of " + mNrOfPages + '}';
    }
}
