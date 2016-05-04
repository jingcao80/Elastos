/*
 * Copyright (C) 2007 Esmertec AG.
 * Copyright (C) 2007 The Android Open Source Project
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

package com.google.android.mms.pdu;


using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

public class PduHeaders {
    /**
     * All pdu header fields.
     */
    public static const Int32 BCC                             = 0x81;
    public static const Int32 CC                              = 0x82;
    public static const Int32 CONTENT_LOCATION                = 0x83;
    public static const Int32 CONTENT_TYPE                    = 0x84;
    public static const Int32 DATE                            = 0x85;
    public static const Int32 DELIVERY_REPORT                 = 0x86;
    public static const Int32 DELIVERY_TIME                   = 0x87;
    public static const Int32 EXPIRY                          = 0x88;
    public static const Int32 FROM                            = 0x89;
    public static const Int32 MESSAGE_CLASS                   = 0x8A;
    public static const Int32 MESSAGE_ID                      = 0x8B;
    public static const Int32 MESSAGE_TYPE                    = 0x8C;
    public static const Int32 MMS_VERSION                     = 0x8D;
    public static const Int32 MESSAGE_SIZE                    = 0x8E;
    public static const Int32 PRIORITY                        = 0x8F;

    public static const Int32 READ_REPLY                      = 0x90;
    public static const Int32 READ_REPORT                     = 0x90;
    public static const Int32 REPORT_ALLOWED                  = 0x91;
    public static const Int32 RESPONSE_STATUS                 = 0x92;
    public static const Int32 RESPONSE_TEXT                   = 0x93;
    public static const Int32 SENDER_VISIBILITY               = 0x94;
    public static const Int32 STATUS                          = 0x95;
    public static const Int32 SUBJECT                         = 0x96;
    public static const Int32 TO                              = 0x97;
    public static const Int32 TRANSACTION_ID                  = 0x98;
    public static const Int32 RETRIEVE_STATUS                 = 0x99;
    public static const Int32 RETRIEVE_TEXT                   = 0x9A;
    public static const Int32 READ_STATUS                     = 0x9B;
    public static const Int32 REPLY_CHARGING                  = 0x9C;
    public static const Int32 REPLY_CHARGING_DEADLINE         = 0x9D;
    public static const Int32 REPLY_CHARGING_ID               = 0x9E;
    public static const Int32 REPLY_CHARGING_SIZE             = 0x9F;

    public static const Int32 PREVIOUSLY_SENT_BY              = 0xA0;
    public static const Int32 PREVIOUSLY_SENT_DATE            = 0xA1;
    public static const Int32 STORE                           = 0xA2;
    public static const Int32 MM_STATE                        = 0xA3;
    public static const Int32 MM_FLAGS                        = 0xA4;
    public static const Int32 STORE_STATUS                    = 0xA5;
    public static const Int32 STORE_STATUS_TEXT               = 0xA6;
    public static const Int32 STORED                          = 0xA7;
    public static const Int32 ATTRIBUTES                      = 0xA8;
    public static const Int32 TOTALS                          = 0xA9;
    public static const Int32 MBOX_TOTALS                     = 0xAA;
    public static const Int32 QUOTAS                          = 0xAB;
    public static const Int32 MBOX_QUOTAS                     = 0xAC;
    public static const Int32 MESSAGE_COUNT                   = 0xAD;
    public static const Int32 CONTENT                         = 0xAE;
    public static const Int32 START                           = 0xAF;

    public static const Int32 ADDITIONAL_HEADERS              = 0xB0;
    public static const Int32 DISTRIBUTION_INDICATOR          = 0xB1;
    public static const Int32 ELEMENT_DESCRIPTOR              = 0xB2;
    public static const Int32 LIMIT                           = 0xB3;
    public static const Int32 RECOMMENDED_RETRIEVAL_MODE      = 0xB4;
    public static const Int32 RECOMMENDED_RETRIEVAL_MODE_TEXT = 0xB5;
    public static const Int32 STATUS_TEXT                     = 0xB6;
    public static const Int32 APPLIC_ID                       = 0xB7;
    public static const Int32 REPLY_APPLIC_ID                 = 0xB8;
    public static const Int32 AUX_APPLIC_ID                   = 0xB9;
    public static const Int32 CONTENT_CLASS                   = 0xBA;
    public static const Int32 DRM_CONTENT                     = 0xBB;
    public static const Int32 ADAPTATION_ALLOWED              = 0xBC;
    public static const Int32 REPLACE_ID                      = 0xBD;
    public static const Int32 CANCEL_ID                       = 0xBE;
    public static const Int32 CANCEL_STATUS                   = 0xBF;

    /**
     * X-Mms-Message-Type field types.
     */
    public static const Int32 MESSAGE_TYPE_SEND_REQ           = 0x80;
    public static const Int32 MESSAGE_TYPE_SEND_CONF          = 0x81;
    public static const Int32 MESSAGE_TYPE_NOTIFICATION_IND   = 0x82;
    public static const Int32 MESSAGE_TYPE_NOTIFYRESP_IND     = 0x83;
    public static const Int32 MESSAGE_TYPE_RETRIEVE_CONF      = 0x84;
    public static const Int32 MESSAGE_TYPE_ACKNOWLEDGE_IND    = 0x85;
    public static const Int32 MESSAGE_TYPE_DELIVERY_IND       = 0x86;
    public static const Int32 MESSAGE_TYPE_READ_REC_IND       = 0x87;
    public static const Int32 MESSAGE_TYPE_READ_ORIG_IND      = 0x88;
    public static const Int32 MESSAGE_TYPE_FORWARD_REQ        = 0x89;
    public static const Int32 MESSAGE_TYPE_FORWARD_CONF       = 0x8A;
    public static const Int32 MESSAGE_TYPE_MBOX_STORE_REQ     = 0x8B;
    public static const Int32 MESSAGE_TYPE_MBOX_STORE_CONF    = 0x8C;
    public static const Int32 MESSAGE_TYPE_MBOX_VIEW_REQ      = 0x8D;
    public static const Int32 MESSAGE_TYPE_MBOX_VIEW_CONF     = 0x8E;
    public static const Int32 MESSAGE_TYPE_MBOX_UPLOAD_REQ    = 0x8F;
    public static const Int32 MESSAGE_TYPE_MBOX_UPLOAD_CONF   = 0x90;
    public static const Int32 MESSAGE_TYPE_MBOX_DELETE_REQ    = 0x91;
    public static const Int32 MESSAGE_TYPE_MBOX_DELETE_CONF   = 0x92;
    public static const Int32 MESSAGE_TYPE_MBOX_DESCR         = 0x93;
    public static const Int32 MESSAGE_TYPE_DELETE_REQ         = 0x94;
    public static const Int32 MESSAGE_TYPE_DELETE_CONF        = 0x95;
    public static const Int32 MESSAGE_TYPE_CANCEL_REQ         = 0x96;
    public static const Int32 MESSAGE_TYPE_CANCEL_CONF        = 0x97;

    /**
     *  X-Mms-Delivery-Report |
     *  X-Mms-Read-Report |
     *  X-Mms-Report-Allowed |
     *  X-Mms-Sender-Visibility |
     *  X-Mms-Store |
     *  X-Mms-Stored |
     *  X-Mms-Totals |
     *  X-Mms-Quotas |
     *  X-Mms-Distribution-Indicator |
     *  X-Mms-DRM-Content |
     *  X-Mms-Adaptation-Allowed |
     *  field types.
     */
    public static const Int32 VALUE_YES                       = 0x80;
    public static const Int32 VALUE_NO                        = 0x81;

    /**
     *  Delivery-Time |
     *  Expiry and Reply-Charging-Deadline |
     *  field type components.
     */
    public static const Int32 VALUE_ABSOLUTE_TOKEN            = 0x80;
    public static const Int32 VALUE_RELATIVE_TOKEN            = 0x81;

    /**
     * X-Mms-MMS-Version field types.
     */
    public static const Int32 MMS_VERSION_1_3                 = ((1 << 4) | 3);
    public static const Int32 MMS_VERSION_1_2                 = ((1 << 4) | 2);
    public static const Int32 MMS_VERSION_1_1                 = ((1 << 4) | 1);
    public static const Int32 MMS_VERSION_1_0                 = ((1 << 4) | 0);

    // Current version is 1.2.
    public static const Int32 CURRENT_MMS_VERSION             = MMS_VERSION_1_2;

    /**
     *  From field type components.
     */
    public static const Int32 FROM_ADDRESS_PRESENT_TOKEN      = 0x80;
    public static const Int32 FROM_INSERT_ADDRESS_TOKEN       = 0x81;

    public static const String FROM_ADDRESS_PRESENT_TOKEN_STR = "address-present-token";
    public static const String FROM_INSERT_ADDRESS_TOKEN_STR = "insert-address-token";

    /**
     *  X-Mms-Status Field.
     */
    public static const Int32 STATUS_EXPIRED                  = 0x80;
    public static const Int32 STATUS_RETRIEVED                = 0x81;
    public static const Int32 STATUS_REJECTED                 = 0x82;
    public static const Int32 STATUS_DEFERRED                 = 0x83;
    public static const Int32 STATUS_UNRECOGNIZED             = 0x84;
    public static const Int32 STATUS_INDETERMINATE            = 0x85;
    public static const Int32 STATUS_FORWARDED                = 0x86;
    public static const Int32 STATUS_UNREACHABLE              = 0x87;

    /**
     *  MM-Flags field type components.
     */
    public static const Int32 MM_FLAGS_ADD_TOKEN              = 0x80;
    public static const Int32 MM_FLAGS_REMOVE_TOKEN           = 0x81;
    public static const Int32 MM_FLAGS_FILTER_TOKEN           = 0x82;

    /**
     *  X-Mms-Message-Class field types.
     */
    public static const Int32 MESSAGE_CLASS_PERSONAL          = 0x80;
    public static const Int32 MESSAGE_CLASS_ADVERTISEMENT     = 0x81;
    public static const Int32 MESSAGE_CLASS_INFORMATIONAL     = 0x82;
    public static const Int32 MESSAGE_CLASS_AUTO              = 0x83;

    public static const String MESSAGE_CLASS_PERSONAL_STR = "personal";
    public static const String MESSAGE_CLASS_ADVERTISEMENT_STR = "advertisement";
    public static const String MESSAGE_CLASS_INFORMATIONAL_STR = "informational";
    public static const String MESSAGE_CLASS_AUTO_STR = "auto";

    /**
     *  X-Mms-Priority field types.
     */
    public static const Int32 PRIORITY_LOW                    = 0x80;
    public static const Int32 PRIORITY_NORMAL                 = 0x81;
    public static const Int32 PRIORITY_HIGH                   = 0x82;

    /**
     *  X-Mms-Response-Status field types.
     */
    public static const Int32 RESPONSE_STATUS_OK                   = 0x80;
    public static const Int32 RESPONSE_STATUS_ERROR_UNSPECIFIED    = 0x81;
    public static const Int32 RESPONSE_STATUS_ERROR_SERVICE_DENIED = 0x82;

    public static const Int32 RESPONSE_STATUS_ERROR_MESSAGE_FORMAT_CORRUPT     = 0x83;
    public static const Int32 RESPONSE_STATUS_ERROR_SENDING_ADDRESS_UNRESOLVED = 0x84;

    public static const Int32 RESPONSE_STATUS_ERROR_MESSAGE_NOT_FOUND    = 0x85;
    public static const Int32 RESPONSE_STATUS_ERROR_NETWORK_PROBLEM      = 0x86;
    public static const Int32 RESPONSE_STATUS_ERROR_CONTENT_NOT_ACCEPTED = 0x87;
    public static const Int32 RESPONSE_STATUS_ERROR_UNSUPPORTED_MESSAGE  = 0x88;
    public static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE    = 0xC0;

    public static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_SENDNG_ADDRESS_UNRESOLVED = 0xC1;
    public static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_MESSAGE_NOT_FOUND         = 0xC2;
    public static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM           = 0xC3;
    public static const Int32 RESPONSE_STATUS_ERROR_TRANSIENT_PARTIAL_SUCCESS           = 0xC4;

    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_FAILURE                             = 0xE0;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_SERVICE_DENIED                      = 0xE1;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_MESSAGE_FORMAT_CORRUPT              = 0xE2;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_SENDING_ADDRESS_UNRESOLVED          = 0xE3;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND                   = 0xE4;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_CONTENT_NOT_ACCEPTED                = 0xE5;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_LIMITATIONS_NOT_MET  = 0xE6;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_REQUEST_NOT_ACCEPTED = 0xE6;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_FORWARDING_DENIED    = 0xE8;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_REPLY_CHARGING_NOT_SUPPORTED        = 0xE9;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_ADDRESS_HIDING_NOT_SUPPORTED        = 0xEA;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_LACK_OF_PREPAID                     = 0xEB;
    public static const Int32 RESPONSE_STATUS_ERROR_PERMANENT_END                                 = 0xFF;

    /**
     *  X-Mms-Retrieve-Status field types.
     */
    public static const Int32 RETRIEVE_STATUS_OK                                  = 0x80;
    public static const Int32 RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE             = 0xC0;
    public static const Int32 RETRIEVE_STATUS_ERROR_TRANSIENT_MESSAGE_NOT_FOUND   = 0xC1;
    public static const Int32 RETRIEVE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM     = 0xC2;
    public static const Int32 RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE             = 0xE0;
    public static const Int32 RETRIEVE_STATUS_ERROR_PERMANENT_SERVICE_DENIED      = 0xE1;
    public static const Int32 RETRIEVE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND   = 0xE2;
    public static const Int32 RETRIEVE_STATUS_ERROR_PERMANENT_CONTENT_UNSUPPORTED = 0xE3;
    public static const Int32 RETRIEVE_STATUS_ERROR_END                           = 0xFF;

    /**
     *  X-Mms-Sender-Visibility field types.
     */
    public static const Int32 SENDER_VISIBILITY_HIDE          = 0x80;
    public static const Int32 SENDER_VISIBILITY_SHOW          = 0x81;

    /**
     *  X-Mms-Read-Status field types.
     */
    public static const Int32 READ_STATUS_READ                        = 0x80;
    public static const Int32 READ_STATUS__DELETED_WITHOUT_BEING_READ = 0x81;

    /**
     *  X-Mms-Cancel-Status field types.
     */
    public static const Int32 CANCEL_STATUS_REQUEST_SUCCESSFULLY_RECEIVED = 0x80;
    public static const Int32 CANCEL_STATUS_REQUEST_CORRUPTED             = 0x81;

    /**
     *  X-Mms-Reply-Charging field types.
     */
    public static const Int32 REPLY_CHARGING_REQUESTED           = 0x80;
    public static const Int32 REPLY_CHARGING_REQUESTED_TEXT_ONLY = 0x81;
    public static const Int32 REPLY_CHARGING_ACCEPTED            = 0x82;
    public static const Int32 REPLY_CHARGING_ACCEPTED_TEXT_ONLY  = 0x83;

    /**
     *  X-Mms-MM-State field types.
     */
    public static const Int32 MM_STATE_DRAFT                  = 0x80;
    public static const Int32 MM_STATE_SENT                   = 0x81;
    public static const Int32 MM_STATE_NEW                    = 0x82;
    public static const Int32 MM_STATE_RETRIEVED              = 0x83;
    public static const Int32 MM_STATE_FORWARDED              = 0x84;

    /**
     * X-Mms-Recommended-Retrieval-Mode field types.
     */
    public static const Int32 RECOMMENDED_RETRIEVAL_MODE_MANUAL = 0x80;

    /**
     *  X-Mms-Content-Class field types.
     */
    public static const Int32 CONTENT_CLASS_TEXT              = 0x80;
    public static const Int32 CONTENT_CLASS_IMAGE_BASIC       = 0x81;
    public static const Int32 CONTENT_CLASS_IMAGE_RICH        = 0x82;
    public static const Int32 CONTENT_CLASS_VIDEO_BASIC       = 0x83;
    public static const Int32 CONTENT_CLASS_VIDEO_RICH        = 0x84;
    public static const Int32 CONTENT_CLASS_MEGAPIXEL         = 0x85;
    public static const Int32 CONTENT_CLASS_CONTENT_BASIC     = 0x86;
    public static const Int32 CONTENT_CLASS_CONTENT_RICH      = 0x87;

    /**
     *  X-Mms-Store-Status field types.
     */
    public static const Int32 STORE_STATUS_SUCCESS                                = 0x80;
    public static const Int32 STORE_STATUS_ERROR_TRANSIENT_FAILURE                = 0xC0;
    public static const Int32 STORE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM        = 0xC1;
    public static const Int32 STORE_STATUS_ERROR_PERMANENT_FAILURE                = 0xE0;
    public static const Int32 STORE_STATUS_ERROR_PERMANENT_SERVICE_DENIED         = 0xE1;
    public static const Int32 STORE_STATUS_ERROR_PERMANENT_MESSAGE_FORMAT_CORRUPT = 0xE2;
    public static const Int32 STORE_STATUS_ERROR_PERMANENT_MESSAGE_NOT_FOUND      = 0xE3;
    public static const Int32 STORE_STATUS_ERROR_PERMANENT_MMBOX_FULL             = 0xE4;
    public static const Int32 STORE_STATUS_ERROR_END                              = 0xFF;

    /**
     * The map contains the value of all headers.
     */
    private HashMap<Integer, Object> mHeaderMap = NULL;

    /**
     * Constructor of PduHeaders.
     */
    public PduHeaders() {
        mHeaderMap = new HashMap<Integer, Object>();
    }

    /**
     * Get octet value by header field.
     *
     * @param field the field
     * @return the octet value of the pdu header
     *          with specified header field. Return 0 if
     *          the value is not set.
     */
    protected Int32 GetOctet(Int32 field) {
        Integer octet = (Integer) mHeaderMap->Get(field);
        If (NULL == octet) {
            return 0;
        }

        return octet;
    }

    /**
     * Set octet value to pdu header by header field.
     *
     * @param value the value
     * @param field the field
     * @throws InvalidHeaderValueException if the value is invalid.
     */
    protected void SetOctet(Int32 value, Int32 field)
            throws InvalidHeaderValueException{
        /**
         * Check whether this field can be set for specific
         * header and check validity of the field.
         */
        Switch (field) {
            case REPORT_ALLOWED:
            case ADAPTATION_ALLOWED:
            case DELIVERY_REPORT:
            case DRM_CONTENT:
            case DISTRIBUTION_INDICATOR:
            case QUOTAS:
            case READ_REPORT:
            case STORE:
            case STORED:
            case TOTALS:
            case SENDER_VISIBILITY:
                If ((VALUE_YES != value) && (VALUE_NO != value)) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            case READ_STATUS:
                If ((READ_STATUS_READ != value) &&
                        (READ_STATUS__DELETED_WITHOUT_BEING_READ != value)) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            case CANCEL_STATUS:
                If ((CANCEL_STATUS_REQUEST_SUCCESSFULLY_RECEIVED != value) &&
                        (CANCEL_STATUS_REQUEST_CORRUPTED != value)) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            case PRIORITY:
                If ((value < PRIORITY_LOW) || (value > PRIORITY_HIGH)) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            case STATUS:
                break;
            case REPLY_CHARGING:
                If ((value < REPLY_CHARGING_REQUESTED)
                        || (value > REPLY_CHARGING_ACCEPTED_TEXT_ONLY)) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            case MM_STATE:
                If ((value < MM_STATE_DRAFT) || (value > MM_STATE_FORWARDED)) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            case RECOMMENDED_RETRIEVAL_MODE:
                If (RECOMMENDED_RETRIEVAL_MODE_MANUAL != value) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            case CONTENT_CLASS:
                If ((value < CONTENT_CLASS_TEXT)
                        || (value > CONTENT_CLASS_CONTENT_RICH)) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            case RETRIEVE_STATUS:
                // According to oma-ts-mms-enc-v1_3, section 7.3.50, we modify the invalid value.
                If ((value > RETRIEVE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM) &&
                        (value < RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE)) {
                    value = RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE;
                } else If ((value > RETRIEVE_STATUS_ERROR_PERMANENT_CONTENT_UNSUPPORTED) &&
                        (value <= RETRIEVE_STATUS_ERROR_END)) {
                    value = RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE;
                } else If ((value < RETRIEVE_STATUS_OK) ||
                        ((value > RETRIEVE_STATUS_OK) &&
                                (value < RETRIEVE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
                                (value > RETRIEVE_STATUS_ERROR_END)) {
                    value = RETRIEVE_STATUS_ERROR_PERMANENT_FAILURE;
                }
                break;
            case STORE_STATUS:
                // According to oma-ts-mms-enc-v1_3, section 7.3.58, we modify the invalid value.
                If ((value > STORE_STATUS_ERROR_TRANSIENT_NETWORK_PROBLEM) &&
                        (value < STORE_STATUS_ERROR_PERMANENT_FAILURE)) {
                    value = STORE_STATUS_ERROR_TRANSIENT_FAILURE;
                } else If ((value > STORE_STATUS_ERROR_PERMANENT_MMBOX_FULL) &&
                        (value <= STORE_STATUS_ERROR_END)) {
                    value = STORE_STATUS_ERROR_PERMANENT_FAILURE;
                } else If ((value < STORE_STATUS_SUCCESS) ||
                        ((value > STORE_STATUS_SUCCESS) &&
                                (value < STORE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
                                (value > STORE_STATUS_ERROR_END)) {
                    value = STORE_STATUS_ERROR_PERMANENT_FAILURE;
                }
                break;
            case RESPONSE_STATUS:
                // According to oma-ts-mms-enc-v1_3, section 7.3.48, we modify the invalid value.
                If ((value > RESPONSE_STATUS_ERROR_TRANSIENT_PARTIAL_SUCCESS) &&
                        (value < RESPONSE_STATUS_ERROR_PERMANENT_FAILURE)) {
                    value = RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE;
                } else If (((value > RESPONSE_STATUS_ERROR_PERMANENT_LACK_OF_PREPAID) &&
                        (value <= RESPONSE_STATUS_ERROR_PERMANENT_END)) ||
                        (value < RESPONSE_STATUS_OK) ||
                        ((value > RESPONSE_STATUS_ERROR_UNSUPPORTED_MESSAGE) &&
                                (value < RESPONSE_STATUS_ERROR_TRANSIENT_FAILURE)) ||
                                (value > RESPONSE_STATUS_ERROR_PERMANENT_END)) {
                    value = RESPONSE_STATUS_ERROR_PERMANENT_FAILURE;
                }
                break;
            case MMS_VERSION:
                If ((value < MMS_VERSION_1_0)|| (value > MMS_VERSION_1_3)) {
                    value = CURRENT_MMS_VERSION; // Current version is the default value.
                }
                break;
            case MESSAGE_TYPE:
                If ((value < MESSAGE_TYPE_SEND_REQ) || (value > MESSAGE_TYPE_CANCEL_CONF)) {
                    // Invalid value.
                    throw new InvalidHeaderValueException("Invalid Octet value!");
                }
                break;
            default:
                // This header value should not be Octect.
                throw new RuntimeException("Invalid header field!");
        }
        mHeaderMap->Put(field, value);
    }

    /**
     * Get TextString value by header field.
     *
     * @param field the field
     * @return the TextString value of the pdu header
     *          with specified header field
     */
    protected Byte[] GetTextString(Int32 field) {
        Return (Byte[]) mHeaderMap->Get(field);
    }

    /**
     * Set TextString value to pdu header by header field.
     *
     * @param value the value
     * @param field the field
     * @return the TextString value of the pdu header
     *          with specified header field
     * @throws NullPointerException if the value is NULL.
     */
    protected void SetTextString(Byte[] value, Int32 field) {
        /**
         * Check whether this field can be set for specific
         * header and check validity of the field.
         */
        If (NULL == value) {
            throw new NullPointerException();
        }

        Switch (field) {
            case TRANSACTION_ID:
            case REPLY_CHARGING_ID:
            case AUX_APPLIC_ID:
            case APPLIC_ID:
            case REPLY_APPLIC_ID:
            case MESSAGE_ID:
            case REPLACE_ID:
            case CANCEL_ID:
            case CONTENT_LOCATION:
            case MESSAGE_CLASS:
            case CONTENT_TYPE:
                break;
            default:
                // This header value should not be Text-String.
                throw new RuntimeException("Invalid header field!");
        }
        mHeaderMap->Put(field, value);
    }

    /**
     * Get EncodedStringValue value by header field.
     *
     * @param field the field
     * @return the EncodedStringValue value of the pdu header
     *          with specified header field
     */
    protected EncodedStringValue GetEncodedStringValue(Int32 field) {
        Return (EncodedStringValue) mHeaderMap->Get(field);
    }

    /**
     * Get TO, CC or BCC header value.
     *
     * @param field the field
     * @return the EncodeStringValue array of the pdu header
     *          with specified header field
     */
    protected EncodedStringValue[] GetEncodedStringValues(Int32 field) {
        ArrayList<EncodedStringValue> list =
                (ArrayList<EncodedStringValue>) mHeaderMap->Get(field);
        If (NULL == list) {
            return NULL;
        }
        EncodedStringValue[] values = new EncodedStringValue[list->Size()];
        return list->ToArray(values);
    }

    /**
     * Set EncodedStringValue value to pdu header by header field.
     *
     * @param value the value
     * @param field the field
     * @return the EncodedStringValue value of the pdu header
     *          with specified header field
     * @throws NullPointerException if the value is NULL.
     */
    protected void SetEncodedStringValue(EncodedStringValue value, Int32 field) {
        /**
         * Check whether this field can be set for specific
         * header and check validity of the field.
         */
        If (NULL == value) {
            throw new NullPointerException();
        }

        Switch (field) {
            case SUBJECT:
            case RECOMMENDED_RETRIEVAL_MODE_TEXT:
            case RETRIEVE_TEXT:
            case STATUS_TEXT:
            case STORE_STATUS_TEXT:
            case RESPONSE_TEXT:
            case FROM:
            case PREVIOUSLY_SENT_BY:
            case MM_FLAGS:
                break;
            default:
                // This header value should not be Encoded-String-Value.
                throw new RuntimeException("Invalid header field!");
        }

        mHeaderMap->Put(field, value);
    }

    /**
     * Set TO, CC or BCC header value.
     *
     * @param value the value
     * @param field the field
     * @return the EncodedStringValue value array of the pdu header
     *          with specified header field
     * @throws NullPointerException if the value is NULL.
     */
    protected void SetEncodedStringValues(EncodedStringValue[] value, Int32 field) {
        /**
         * Check whether this field can be set for specific
         * header and check validity of the field.
         */
        If (NULL == value) {
            throw new NullPointerException();
        }

        Switch (field) {
            case BCC:
            case CC:
            case TO:
                break;
            default:
                // This header value should not be Encoded-String-Value.
                throw new RuntimeException("Invalid header field!");
        }

        ArrayList<EncodedStringValue> list = new ArrayList<EncodedStringValue>();
        For (Int32 i = 0; i < value.length; i++) {
            list->Add(value[i]);
        }
        mHeaderMap->Put(field, list);
    }

    /**
     * Append one EncodedStringValue to another.
     *
     * @param value the EncodedStringValue to append
     * @param field the field
     * @throws NullPointerException if the value is NULL.
     */
    protected void AppendEncodedStringValue(EncodedStringValue value,
                                    Int32 field) {
        If (NULL == value) {
            throw new NullPointerException();
        }

        Switch (field) {
            case BCC:
            case CC:
            case TO:
                break;
            default:
                throw new RuntimeException("Invalid header field!");
        }

        ArrayList<EncodedStringValue> list =
            (ArrayList<EncodedStringValue>) mHeaderMap->Get(field);
        If (NULL == list) {
            list  = new ArrayList<EncodedStringValue>();
        }
        list->Add(value);
        mHeaderMap->Put(field, list);
    }

    /**
     * Get LongInteger value by header field.
     *
     * @param field the field
     * @return the LongInteger value of the pdu header
     *          with specified header field. if return -1, the
     *          field is not existed in pdu header.
     */
    protected Int64 GetLongInteger(Int32 field) {
        Long longInteger = (Long) mHeaderMap->Get(field);
        If (NULL == longInteger) {
            return -1;
        }

        return longInteger->LongValue();
    }

    /**
     * Set LongInteger value to pdu header by header field.
     *
     * @param value the value
     * @param field the field
     */
    protected void SetLongInteger(Int64 value, Int32 field) {
        /**
         * Check whether this field can be set for specific
         * header and check validity of the field.
         */
        Switch (field) {
            case DATE:
            case REPLY_CHARGING_SIZE:
            case MESSAGE_SIZE:
            case MESSAGE_COUNT:
            case START:
            case LIMIT:
            case DELIVERY_TIME:
            case EXPIRY:
            case REPLY_CHARGING_DEADLINE:
            case PREVIOUSLY_SENT_DATE:
                break;
            default:
                // This header value should not be LongInteger.
                throw new RuntimeException("Invalid header field!");
        }
        mHeaderMap->Put(field, value);
    }
}
