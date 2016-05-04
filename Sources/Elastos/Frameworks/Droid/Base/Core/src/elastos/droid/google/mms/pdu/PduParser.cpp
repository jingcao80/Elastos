/*
 * Copyright (C) 2007-2008 Esmertec AG.
 * Copyright (C) 2007-2008 The Android Open Source Project
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

using com::Google::Android::Mms::IContentType;
using com::Google::Android::Mms::Pdu::IEncodedStringValue;

using Elastos::Droid::Utility::ILog;

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::Utility::IArrays;
using Elastos::Utility::IHashMap;

using Elastos::Droid::Content::Res::IResources;

public class PduParser {
    /**
     *  The next are WAP values defined in WSP specification.
     */
    private static const Int32 QUOTE = 127;
    private static const Int32 LENGTH_QUOTE = 31;
    private static const Int32 TEXT_MIN = 32;
    private static const Int32 TEXT_MAX = 127;
    private static const Int32 SHORT_INTEGER_MAX = 127;
    private static const Int32 SHORT_LENGTH_MAX = 30;
    private static const Int32 LONG_INTEGER_LENGTH_MAX = 8;
    private static const Int32 QUOTED_STRING_FLAG = 34;
    private static const Int32 END_STRING_FLAG = 0x00;
    //The next two are used by the interface "parseWapString" to
    //distinguish Text-String and Quoted-String.
    private static const Int32 TYPE_TEXT_STRING = 0;
    private static const Int32 TYPE_QUOTED_STRING = 1;
    private static const Int32 TYPE_TOKEN_STRING = 2;

    /**
     * Specify the part position.
     */
    private static const Int32 THE_FIRST_PART = 0;
    private static const Int32 THE_LAST_PART = 1;

    /**
     * The pdu data.
     */
    private ByteArrayInputStream mPduDataStream = NULL;

    /**
     * Store pdu headers
     */
    private PduHeaders mHeaders = NULL;

    /**
     * Store pdu parts.
     */
    private PduBody mBody = NULL;

    /**
     * Store the "type" parameter in "Content-Type" header field.
     */
    private static Byte[] mTypeParam = NULL;

    /**
     * Store the "start" parameter in "Content-Type" header field.
     */
    private static Byte[] mStartParam = NULL;

    /**
     * The log tag.
     */
    private static const String LOG_TAG = "PduParser";
    private static const Boolean DEBUG = FALSE;
    private static const Boolean LOCAL_LOGV = FALSE;

    /**
     * Constructor.
     *
     * @param pduDataStream pdu data to be parsed
     */
    public PduParser(Byte[] pduDataStream) {
        mPduDataStream = new ByteArrayInputStream(pduDataStream);
    }

    /**
     * Parse the pdu.
     *
     * @return the pdu structure if parsing successfully.
     *         NULL if parsing error happened or mandatory fields are not set.
     */
    public GenericPdu Parse(){
        If (mPduDataStream == NULL) {
            return NULL;
        }

        /* parse headers */
        mHeaders = ParseHeaders(mPduDataStream);
        If (NULL == mHeaders) {
            // Parse headers failed.
            return NULL;
        }

        /* get the message type */
        Int32 messageType = mHeaders->GetOctet(PduHeaders.MESSAGE_TYPE);

        /* check mandatory header fields */
        If (FALSE == CheckMandatoryHeader(mHeaders)) {
            Log("check mandatory headers failed!");
            return NULL;
        }

        If ((PduHeaders.MESSAGE_TYPE_SEND_REQ == messageType) ||
                (PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF == messageType)) {
            /* need to parse the parts */
            mBody = ParseParts(mPduDataStream);
            If (NULL == mBody) {
                // Parse parts failed.
                return NULL;
            }
        }

        Switch (messageType) {
            case PduHeaders.MESSAGE_TYPE_SEND_REQ:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_SEND_REQ");
                }
                SendReq sendReq = new SendReq(mHeaders, mBody);
                return sendReq;
            case PduHeaders.MESSAGE_TYPE_SEND_CONF:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_SEND_CONF");
                }
                SendConf sendConf = new SendConf(mHeaders);
                return sendConf;
            case PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_NOTIFICATION_IND");
                }
                NotificationInd notificationInd =
                    new NotificationInd(mHeaders);
                return notificationInd;
            case PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_NOTIFYRESP_IND");
                }
                NotifyRespInd notifyRespInd =
                    new NotifyRespInd(mHeaders);
                return notifyRespInd;
            case PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_RETRIEVE_CONF");
                }
                RetrieveConf retrieveConf =
                    new RetrieveConf(mHeaders, mBody);

                Byte[] contentType = retrieveConf->GetContentType();
                If (NULL == contentType) {
                    return NULL;
                }
                String ctTypeStr = new String(contentType);
                If (ctTypeStr->Equals(ContentType.MULTIPART_MIXED)
                        || ctTypeStr->Equals(ContentType.MULTIPART_RELATED)
                        || ctTypeStr->Equals(ContentType.MULTIPART_ALTERNATIVE)) {
                    // The MMS content type must be "application/vnd.wap.multipart.mixed"
                    // or "application/vnd.wap.multipart.related"
                    // or "application/vnd.wap.multipart.alternative"
                    return retrieveConf;
                } else If (ctTypeStr->Equals(ContentType.MULTIPART_ALTERNATIVE)) {
                    // "application/vnd.wap.multipart.alternative"
                    // should take only the first part.
                    PduPart firstPart = mBody->GetPart(0);
                    mBody->RemoveAll();
                    mBody->AddPart(0, firstPart);
                    return retrieveConf;
                }
                return NULL;
            case PduHeaders.MESSAGE_TYPE_DELIVERY_IND:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_DELIVERY_IND");
                }
                DeliveryInd deliveryInd =
                    new DeliveryInd(mHeaders);
                return deliveryInd;
            case PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_ACKNOWLEDGE_IND");
                }
                AcknowledgeInd acknowledgeInd =
                    new AcknowledgeInd(mHeaders);
                return acknowledgeInd;
            case PduHeaders.MESSAGE_TYPE_READ_ORIG_IND:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_READ_ORIG_IND");
                }
                ReadOrigInd readOrigInd =
                    new ReadOrigInd(mHeaders);
                return readOrigInd;
            case PduHeaders.MESSAGE_TYPE_READ_REC_IND:
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "parse: MESSAGE_TYPE_READ_REC_IND");
                }
                ReadRecInd readRecInd =
                    new ReadRecInd(mHeaders);
                return readRecInd;
            default:
                Log("Parser doesn't support this message type in this version!");
            return NULL;
        }
    }

    /**
     * Parse pdu headers.
     *
     * @param pduDataStream pdu data input stream
     * @return headers in PduHeaders structure, NULL when parse fail
     */
    protected PduHeaders ParseHeaders(ByteArrayInputStream pduDataStream){
        If (pduDataStream == NULL) {
            return NULL;
        }
        Boolean keepParsing = TRUE;
        PduHeaders headers = new PduHeaders();

        While (keepParsing && (pduDataStream->Available() > 0)) {
            pduDataStream->Mark(1);
            Int32 headerField = ExtractByteValue(pduDataStream);
            /* parse custom text header */
            If ((headerField >= TEXT_MIN) && (headerField <= TEXT_MAX)) {
                pduDataStream->Reset();
                Byte [] bVal = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "TextHeader: " + new String(bVal));
                }
                /* we should ignore it at the moment */
                continue;
            }
            Switch (headerField) {
                case PduHeaders.MESSAGE_TYPE:
                {
                    Int32 messageType = ExtractByteValue(pduDataStream);
                    If (LOCAL_LOGV) {
                        Logger::V(LOG_TAG, "parseHeaders: messageType: " + messageType);
                    }
                    Switch (messageType) {
                        // We don't support these kind of messages now.
                        case PduHeaders.MESSAGE_TYPE_FORWARD_REQ:
                        case PduHeaders.MESSAGE_TYPE_FORWARD_CONF:
                        case PduHeaders.MESSAGE_TYPE_MBOX_STORE_REQ:
                        case PduHeaders.MESSAGE_TYPE_MBOX_STORE_CONF:
                        case PduHeaders.MESSAGE_TYPE_MBOX_VIEW_REQ:
                        case PduHeaders.MESSAGE_TYPE_MBOX_VIEW_CONF:
                        case PduHeaders.MESSAGE_TYPE_MBOX_UPLOAD_REQ:
                        case PduHeaders.MESSAGE_TYPE_MBOX_UPLOAD_CONF:
                        case PduHeaders.MESSAGE_TYPE_MBOX_DELETE_REQ:
                        case PduHeaders.MESSAGE_TYPE_MBOX_DELETE_CONF:
                        case PduHeaders.MESSAGE_TYPE_MBOX_DESCR:
                        case PduHeaders.MESSAGE_TYPE_DELETE_REQ:
                        case PduHeaders.MESSAGE_TYPE_DELETE_CONF:
                        case PduHeaders.MESSAGE_TYPE_CANCEL_REQ:
                        case PduHeaders.MESSAGE_TYPE_CANCEL_CONF:
                            return NULL;
                    }
                    try {
                        headers->SetOctet(messageType, headerField);
                    } Catch(InvalidHeaderValueException e) {
                        Log("Set invalid Octet value: " + messageType +
                                " into the header filed: " + headerField);
                        return NULL;
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Octet header field!");
                        return NULL;
                    }
                    break;
                }
                /* Octect value */
                case PduHeaders.REPORT_ALLOWED:
                case PduHeaders.ADAPTATION_ALLOWED:
                case PduHeaders.DELIVERY_REPORT:
                case PduHeaders.DRM_CONTENT:
                case PduHeaders.DISTRIBUTION_INDICATOR:
                case PduHeaders.QUOTAS:
                case PduHeaders.READ_REPORT:
                case PduHeaders.STORE:
                case PduHeaders.STORED:
                case PduHeaders.TOTALS:
                case PduHeaders.SENDER_VISIBILITY:
                case PduHeaders.READ_STATUS:
                case PduHeaders.CANCEL_STATUS:
                case PduHeaders.PRIORITY:
                case PduHeaders.STATUS:
                case PduHeaders.REPLY_CHARGING:
                case PduHeaders.MM_STATE:
                case PduHeaders.RECOMMENDED_RETRIEVAL_MODE:
                case PduHeaders.CONTENT_CLASS:
                case PduHeaders.RETRIEVE_STATUS:
                case PduHeaders.STORE_STATUS:
                    /**
                     * The following field has a different value when
                     * used in the M-Mbox-Delete.conf and M-Delete.conf PDU.
                     * For now we ignore this fact, since we do not support these PDUs
                     */
                case PduHeaders.RESPONSE_STATUS:
                {
                    Int32 value = ExtractByteValue(pduDataStream);
                    If (LOCAL_LOGV) {
                        Logger::V(LOG_TAG, "parseHeaders: Byte: " + headerField + " value: " +
                                value);
                    }

                    try {
                        headers->SetOctet(value, headerField);
                    } Catch(InvalidHeaderValueException e) {
                        Log("Set invalid Octet value: " + value +
                                " into the header filed: " + headerField);
                        return NULL;
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Octet header field!");
                        return NULL;
                    }
                    break;
                }

                /* Long-Integer */
                case PduHeaders.DATE:
                case PduHeaders.REPLY_CHARGING_SIZE:
                case PduHeaders.MESSAGE_SIZE:
                {
                    try {
                        Int64 value = ParseLongInteger(pduDataStream);
                        If (LOCAL_LOGV) {
                            Logger::V(LOG_TAG, "parseHeaders: longint: " + headerField + " value: " +
                                    value);
                        }
                        headers->SetLongInteger(value, headerField);
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Long-Integer header field!");
                        return NULL;
                    }
                    break;
                }

                /* Integer-Value */
                case PduHeaders.MESSAGE_COUNT:
                case PduHeaders.START:
                case PduHeaders.LIMIT:
                {
                    try {
                        Int64 value = ParseIntegerValue(pduDataStream);
                        If (LOCAL_LOGV) {
                            Logger::V(LOG_TAG, "parseHeaders: Int32: " + headerField + " value: " +
                                    value);
                        }
                        headers->SetLongInteger(value, headerField);
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Long-Integer header field!");
                        return NULL;
                    }
                    break;
                }

                /* Text-String */
                case PduHeaders.TRANSACTION_ID:
                case PduHeaders.REPLY_CHARGING_ID:
                case PduHeaders.AUX_APPLIC_ID:
                case PduHeaders.APPLIC_ID:
                case PduHeaders.REPLY_APPLIC_ID:
                    /**
                     * The next three header fields are email addresses
                     * as defined in RFC2822,
                     * not including the characters "<" and ">"
                     */
                case PduHeaders.MESSAGE_ID:
                case PduHeaders.REPLACE_ID:
                case PduHeaders.CANCEL_ID:
                    /**
                     * The following field has a different value when
                     * used in the M-Mbox-Delete.conf and M-Delete.conf PDU.
                     * For now we ignore this fact, since we do not support these PDUs
                     */
                case PduHeaders.CONTENT_LOCATION:
                {
                    Byte[] value = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                    If (NULL != value) {
                        try {
                            If (LOCAL_LOGV) {
                                Logger::V(LOG_TAG, "parseHeaders: string: " + headerField + " value: " +
                                        new String(value));
                            }
                            headers->SetTextString(value, headerField);
                        } Catch(NullPointerException e) {
                            Log("NULL pointer error!");
                        } Catch(RuntimeException e) {
                            Log(headerField + "is not Text-String header field!");
                            return NULL;
                        }
                    }
                    break;
                }

                /* Encoded-string-value */
                case PduHeaders.SUBJECT:
                case PduHeaders.RECOMMENDED_RETRIEVAL_MODE_TEXT:
                case PduHeaders.RETRIEVE_TEXT:
                case PduHeaders.STATUS_TEXT:
                case PduHeaders.STORE_STATUS_TEXT:
                    /* the next one is not support
                     * M-Mbox-Delete.conf and M-Delete.conf now */
                case PduHeaders.RESPONSE_TEXT:
                {
                    EncodedStringValue value =
                        ParseEncodedStringValue(pduDataStream);
                    If (NULL != value) {
                        try {
                            If (LOCAL_LOGV) {
                                Logger::V(LOG_TAG, "parseHeaders: encoded string: " + headerField
                                        + " value: " + value->GetString());
                            }
                            headers->SetEncodedStringValue(value, headerField);
                        } Catch(NullPointerException e) {
                            Log("NULL pointer error!");
                        } Catch (RuntimeException e) {
                            Log(headerField + "is not Encoded-String-Value header field!");
                            return NULL;
                        }
                    }
                    break;
                }

                /* Addressing model */
                case PduHeaders.BCC:
                case PduHeaders.CC:
                case PduHeaders.TO:
                {
                    EncodedStringValue value =
                        ParseEncodedStringValue(pduDataStream);
                    If (NULL != value) {
                        Byte[] address = value->GetTextString();
                        If (NULL != address) {
                            String str = new String(address);
                            If (LOCAL_LOGV) {
                                Logger::V(LOG_TAG, "parseHeaders: (to/cc/bcc) address: " + headerField
                                        + " value: " + str);
                            }
                            Int32 endIndex = str->IndexOf("/");
                            If (endIndex > 0) {
                                str = str->Substring(0, endIndex);
                            }
                            try {
                                value->SetTextString(str->GetBytes());
                            } Catch(NullPointerException e) {
                                Log("NULL pointer error!");
                                return NULL;
                            }
                        }

                        try {
                            headers->AppendEncodedStringValue(value, headerField);
                        } Catch(NullPointerException e) {
                            Log("NULL pointer error!");
                        } Catch(RuntimeException e) {
                            Log(headerField + "is not Encoded-String-Value header field!");
                            return NULL;
                        }
                    }
                    break;
                }

                /* Value-length
                 * (Absolute-token Date-value | Relative-token Delta-seconds-value) */
                case PduHeaders.DELIVERY_TIME:
                case PduHeaders.EXPIRY:
                case PduHeaders.REPLY_CHARGING_DEADLINE:
                {
                    /* parse Value-length */
                    ParseValueLength(pduDataStream);

                    /* Absolute-token or Relative-token */
                    Int32 token = ExtractByteValue(pduDataStream);

                    /* Date-value or Delta-seconds-value */
                    Int64 timeValue;
                    try {
                        timeValue = ParseLongInteger(pduDataStream);
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Long-Integer header field!");
                        return NULL;
                    }
                    If (PduHeaders.VALUE_RELATIVE_TOKEN == token) {
                        /* need to convert the Delta-seconds-value
                         * into Date-value */
                        timeValue = System->CurrentTimeMillis()/1000 + timeValue;
                    }

                    try {
                        If (LOCAL_LOGV) {
                            Logger::V(LOG_TAG, "parseHeaders: time value: " + headerField
                                    + " value: " + timeValue);
                        }
                        headers->SetLongInteger(timeValue, headerField);
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Long-Integer header field!");
                        return NULL;
                    }
                    break;
                }

                case PduHeaders.FROM: {
                    /* From-value =
                     * Value-length
                     * (Address-present-token Encoded-string-value | Insert-address-token)
                     */
                    EncodedStringValue from = NULL;
                    ParseValueLength(pduDataStream); /* parse value-length */

                    /* Address-present-token or Insert-address-token */
                    Int32 fromToken = ExtractByteValue(pduDataStream);

                    /* Address-present-token or Insert-address-token */
                    If (PduHeaders.FROM_ADDRESS_PRESENT_TOKEN == fromToken) {
                        /* Encoded-string-value */
                        from = ParseEncodedStringValue(pduDataStream);
                        If (NULL != from) {
                            Byte[] address = from->GetTextString();
                            If (NULL != address) {
                                String str = new String(address);
                                Int32 endIndex = str->IndexOf("/");
                                If (endIndex > 0) {
                                    str = str->Substring(0, endIndex);
                                }
                                try {
                                    from->SetTextString(str->GetBytes());
                                } Catch(NullPointerException e) {
                                    Log("NULL pointer error!");
                                    return NULL;
                                }
                            }
                        }
                    } else {
                        try {
                            from = new EncodedStringValue(
                                    PduHeaders.FROM_INSERT_ADDRESS_TOKEN_STR->GetBytes());
                        } Catch(NullPointerException e) {
                            Log(headerField + "is not Encoded-String-Value header field!");
                            return NULL;
                        }
                    }

                    try {
                        If (LOCAL_LOGV) {
                            Logger::V(LOG_TAG, "parseHeaders: from address: " + headerField
                                    + " value: " + from->GetString());
                        }
                        headers->SetEncodedStringValue(from, PduHeaders.FROM);
                    } Catch(NullPointerException e) {
                        Log("NULL pointer error!");
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Encoded-String-Value header field!");
                        return NULL;
                    }
                    break;
                }

                case PduHeaders.MESSAGE_CLASS: {
                    /* Message-class-value = Class-identifier | Token-text */
                    pduDataStream->Mark(1);
                    Int32 messageClass = ExtractByteValue(pduDataStream);
                    If (LOCAL_LOGV) {
                        Logger::V(LOG_TAG, "parseHeaders: MESSAGE_CLASS: " + headerField
                                + " value: " + messageClass);
                    }

                    If (messageClass >= PduHeaders.MESSAGE_CLASS_PERSONAL) {
                        /* Class-identifier */
                        try {
                            If (PduHeaders.MESSAGE_CLASS_PERSONAL == messageClass) {
                                headers->SetTextString(
                                        PduHeaders.MESSAGE_CLASS_PERSONAL_STR->GetBytes(),
                                        PduHeaders.MESSAGE_CLASS);
                            } else If (PduHeaders.MESSAGE_CLASS_ADVERTISEMENT == messageClass) {
                                headers->SetTextString(
                                        PduHeaders.MESSAGE_CLASS_ADVERTISEMENT_STR->GetBytes(),
                                        PduHeaders.MESSAGE_CLASS);
                            } else If (PduHeaders.MESSAGE_CLASS_INFORMATIONAL == messageClass) {
                                headers->SetTextString(
                                        PduHeaders.MESSAGE_CLASS_INFORMATIONAL_STR->GetBytes(),
                                        PduHeaders.MESSAGE_CLASS);
                            } else If (PduHeaders.MESSAGE_CLASS_AUTO == messageClass) {
                                headers->SetTextString(
                                        PduHeaders.MESSAGE_CLASS_AUTO_STR->GetBytes(),
                                        PduHeaders.MESSAGE_CLASS);
                            }
                        } Catch(NullPointerException e) {
                            Log("NULL pointer error!");
                        } Catch(RuntimeException e) {
                            Log(headerField + "is not Text-String header field!");
                            return NULL;
                        }
                    } else {
                        /* Token-text */
                        pduDataStream->Reset();
                        Byte[] messageClassString = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                        If (NULL != messageClassString) {
                            try {
                                headers->SetTextString(messageClassString, PduHeaders.MESSAGE_CLASS);
                            } Catch(NullPointerException e) {
                                Log("NULL pointer error!");
                            } Catch(RuntimeException e) {
                                Log(headerField + "is not Text-String header field!");
                                return NULL;
                            }
                        }
                    }
                    break;
                }

                case PduHeaders.MMS_VERSION: {
                    Int32 version = ParseShortInteger(pduDataStream);

                    try {
                        If (LOCAL_LOGV) {
                            Logger::V(LOG_TAG, "parseHeaders: MMS_VERSION: " + headerField
                                    + " value: " + version);
                        }
                        headers->SetOctet(version, PduHeaders.MMS_VERSION);
                    } Catch(InvalidHeaderValueException e) {
                        Log("Set invalid Octet value: " + version +
                                " into the header filed: " + headerField);
                        return NULL;
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Octet header field!");
                        return NULL;
                    }
                    break;
                }

                case PduHeaders.PREVIOUSLY_SENT_BY: {
                    /* Previously-sent-by-value =
                     * Value-length Forwarded-count-value Encoded-string-value */
                    /* parse value-length */
                    ParseValueLength(pduDataStream);

                    /* parse Forwarded-count-value */
                    try {
                        ParseIntegerValue(pduDataStream);
                    } Catch(RuntimeException e) {
                        Log(headerField + " is not Integer-Value");
                        return NULL;
                    }

                    /* parse Encoded-string-value */
                    EncodedStringValue previouslySentBy =
                        ParseEncodedStringValue(pduDataStream);
                    If (NULL != previouslySentBy) {
                        try {
                            If (LOCAL_LOGV) {
                                Logger::V(LOG_TAG, "parseHeaders: PREVIOUSLY_SENT_BY: " + headerField
                                        + " value: " + previouslySentBy->GetString());
                            }
                            headers->SetEncodedStringValue(previouslySentBy,
                                    PduHeaders.PREVIOUSLY_SENT_BY);
                        } Catch(NullPointerException e) {
                            Log("NULL pointer error!");
                        } Catch(RuntimeException e) {
                            Log(headerField + "is not Encoded-String-Value header field!");
                            return NULL;
                        }
                    }
                    break;
                }

                case PduHeaders.PREVIOUSLY_SENT_DATE: {
                    /* Previously-sent-date-value =
                     * Value-length Forwarded-count-value Date-value */
                    /* parse value-length */
                    ParseValueLength(pduDataStream);

                    /* parse Forwarded-count-value */
                    try {
                        ParseIntegerValue(pduDataStream);
                    } Catch(RuntimeException e) {
                        Log(headerField + " is not Integer-Value");
                        return NULL;
                    }

                    /* Date-value */
                    try {
                        Int64 perviouslySentDate = ParseLongInteger(pduDataStream);
                        If (LOCAL_LOGV) {
                            Logger::V(LOG_TAG, "parseHeaders: PREVIOUSLY_SENT_DATE: " + headerField
                                    + " value: " + perviouslySentDate);
                        }
                        headers->SetLongInteger(perviouslySentDate,
                                PduHeaders.PREVIOUSLY_SENT_DATE);
                    } Catch(RuntimeException e) {
                        Log(headerField + "is not Long-Integer header field!");
                        return NULL;
                    }
                    break;
                }

                case PduHeaders.MM_FLAGS: {
                    /* MM-flags-value =
                     * Value-length
                     * ( Add-token | Remove-token | Filter-token )
                     * Encoded-string-value
                     */
                    If (LOCAL_LOGV) {
                        Logger::V(LOG_TAG, "parseHeaders: MM_FLAGS: " + headerField
                                + " NOT REALLY SUPPORTED");
                    }

                    /* parse Value-length */
                    ParseValueLength(pduDataStream);

                    /* Add-token | Remove-token | Filter-token */
                    ExtractByteValue(pduDataStream);

                    /* Encoded-string-value */
                    ParseEncodedStringValue(pduDataStream);

                    /* not store this header filed in "headers",
                     * because now PduHeaders doesn't support it */
                    break;
                }

                /* Value-length
                 * (Message-total-token | Size-total-token) Integer-Value */
                case PduHeaders.MBOX_TOTALS:
                case PduHeaders.MBOX_QUOTAS:
                {
                    If (LOCAL_LOGV) {
                        Logger::V(LOG_TAG, "parseHeaders: MBOX_TOTALS: " + headerField);
                    }
                    /* Value-length */
                    ParseValueLength(pduDataStream);

                    /* Message-total-token | Size-total-token */
                    ExtractByteValue(pduDataStream);

                    /*Integer-Value*/
                    try {
                        ParseIntegerValue(pduDataStream);
                    } Catch(RuntimeException e) {
                        Log(headerField + " is not Integer-Value");
                        return NULL;
                    }

                    /* not store these headers filed in "headers",
                    because now PduHeaders doesn't support them */
                    break;
                }

                case PduHeaders.ELEMENT_DESCRIPTOR: {
                    If (LOCAL_LOGV) {
                        Logger::V(LOG_TAG, "parseHeaders: ELEMENT_DESCRIPTOR: " + headerField);
                    }
                    ParseContentType(pduDataStream, NULL);

                    /* not store this header filed in "headers",
                    because now PduHeaders doesn't support it */
                    break;
                }

                case PduHeaders.CONTENT_TYPE: {
                    HashMap<Integer, Object> map =
                        new HashMap<Integer, Object>();
                    Byte[] contentType =
                        ParseContentType(pduDataStream, map);

                    If (NULL != contentType) {
                        try {
                            If (LOCAL_LOGV) {
                                Logger::V(LOG_TAG, "parseHeaders: CONTENT_TYPE: " + headerField +
                                        contentType->ToString());
                            }
                            headers->SetTextString(contentType, PduHeaders.CONTENT_TYPE);
                        } Catch(NullPointerException e) {
                            Log("NULL pointer error!");
                        } Catch(RuntimeException e) {
                            Log(headerField + "is not Text-String header field!");
                            return NULL;
                        }
                    }

                    /* get start parameter */
                    mStartParam = (Byte[]) map->Get(PduPart.P_START);

                    /* get charset parameter */
                    mTypeParam= (Byte[]) map->Get(PduPart.P_TYPE);

                    keepParsing = FALSE;
                    break;
                }

                case PduHeaders.CONTENT:
                case PduHeaders.ADDITIONAL_HEADERS:
                case PduHeaders.ATTRIBUTES:
                default: {
                    If (LOCAL_LOGV) {
                        Logger::V(LOG_TAG, "parseHeaders: Unknown header: " + headerField);
                    }
                    Log("Unknown header");
                }
            }
        }

        return headers;
    }

    /**
     * Parse pdu parts.
     *
     * @param pduDataStream pdu data input stream
     * @return parts in PduBody structure
     */
    protected static PduBody ParseParts(ByteArrayInputStream pduDataStream) {
        If (pduDataStream == NULL) {
            return NULL;
        }

        Int32 count = ParseUnsignedInt(pduDataStream); // get the number of parts
        PduBody body = new PduBody();

        For (Int32 i = 0 ; i < count ; i++) {
            Int32 headerLength = ParseUnsignedInt(pduDataStream);
            Int32 dataLength = ParseUnsignedInt(pduDataStream);
            PduPart part = new PduPart();
            Int32 startPos = pduDataStream->Available();
            If (startPos <= 0) {
                // Invalid part.
                return NULL;
            }

            /* parse part's content-type */
            HashMap<Integer, Object> map = new HashMap<Integer, Object>();
            Byte[] contentType = ParseContentType(pduDataStream, map);
            If (NULL != contentType) {
                part->SetContentType(contentType);
            } else {
                part->SetContentType((PduContentTypes.contentTypes[0]).GetBytes()); //"*/*"
            }

            /* get name parameter */
            Byte[] name = (Byte[]) map->Get(PduPart.P_NAME);
            If (NULL != name) {
                part->SetName(name);
            }

            /* get charset parameter */
            Integer charset = (Integer) map->Get(PduPart.P_CHARSET);
            If (NULL != charset) {
                part->SetCharset(charset);
            }

            /* parse part's headers */
            Int32 endPos = pduDataStream->Available();
            Int32 partHeaderLen = headerLength - (startPos - endPos);
            If (partHeaderLen > 0) {
                If (FALSE == ParsePartHeaders(pduDataStream, part, partHeaderLen)) {
                    // Parse part header faild.
                    return NULL;
                }
            } else If (partHeaderLen < 0) {
                // Invalid length of content-type.
                return NULL;
            }

            /* FIXME: check content-id, name, filename and content location,
             * if not set anyone of them, generate a default content-location
             */
            If ((NULL == part->GetContentLocation())
                    && (NULL == part->GetName())
                    && (NULL == part->GetFilename())
                    && (NULL == part->GetContentId())) {
                part->SetContentLocation(Long->ToOctalString(
                        System->CurrentTimeMillis()).GetBytes());
            }

            /* get part's data */
            If (dataLength > 0) {
                Byte[] partData = new Byte[dataLength];
                String partContentType = new String(part->GetContentType());
                pduDataStream->Read(partData, 0, dataLength);
                If (partContentType->EqualsIgnoreCase(ContentType.MULTIPART_ALTERNATIVE)) {
                    // parse "multipart/vnd.wap.multipart.alternative".
                    PduBody childBody = ParseParts(new ByteArrayInputStream(partData));
                    // take the first part of children.
                    part = childBody->GetPart(0);
                } else {
                    // Check Content-Transfer-Encoding.
                    Byte[] partDataEncoding = part->GetContentTransferEncoding();
                    If (NULL != partDataEncoding) {
                        String encoding = new String(partDataEncoding);
                        If (encoding->EqualsIgnoreCase(PduPart.P_BASE64)) {
                            // Decode "base64" into "binary".
                            partData = Base64->DecodeBase64(partData);
                        } else If (encoding->EqualsIgnoreCase(PduPart.P_QUOTED_PRINTABLE)) {
                            // Decode "quoted-printable" into "binary".
                            partData = QuotedPrintable->DecodeQuotedPrintable(partData);
                        } else {
                            // "binary" is the default encoding.
                        }
                    }
                    If (NULL == partData) {
                        Log("Decode part data error!");
                        return NULL;
                    }
                    part->SetData(partData);
                }
            }

            /* add this part to body */
            If (THE_FIRST_PART == CheckPartPosition(part)) {
                /* this is the first part */
                body->AddPart(0, part);
            } else {
                /* add the part to the end */
                body->AddPart(part);
            }
        }

        return body;
    }

    /**
     * Log status.
     *
     * @param text log information
     */
    private static void Log(String text) {
        If (LOCAL_LOGV) {
            Logger::V(LOG_TAG, text);
        }
    }

    /**
     * Parse unsigned integer.
     *
     * @param pduDataStream pdu data input stream
     * @return the integer, -1 when failed
     */
    protected static Int32 ParseUnsignedInt(ByteArrayInputStream pduDataStream) {
        /**
         * From wap-230-wsp-20010705-a.pdf
         * The maximum size of a uintvar is 32 bits.
         * So it will be encoded in no more than 5 octets.
         */
        Assert(NULL != pduDataStream);
        Int32 result = 0;
        Int32 temp = pduDataStream->Read();
        If (temp == -1) {
            return temp;
        }

        While((temp & 0x80) != 0) {
            result = result << 7;
            result |= temp & 0x7F;
            temp = pduDataStream->Read();
            If (temp == -1) {
                return temp;
            }
        }

        result = result << 7;
        result |= temp & 0x7F;

        return result;
    }

    /**
     * Parse value length.
     *
     * @param pduDataStream pdu data input stream
     * @return the integer
     */
    protected static Int32 ParseValueLength(ByteArrayInputStream pduDataStream) {
        /**
         * From wap-230-wsp-20010705-a.pdf
         * Value-length = Short-length | (Length-quote Length)
         * Short-length = <Any octet 0-30>
         * Length-quote = <Octet 31>
         * Length = Uintvar-integer
         * Uintvar-integer = 1*5 OCTET
         */
        Assert(NULL != pduDataStream);
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        Int32 first = temp & 0xFF;

        If (first <= SHORT_LENGTH_MAX) {
            return first;
        } else If (first == LENGTH_QUOTE) {
            return ParseUnsignedInt(pduDataStream);
        }

        throw new RuntimeException ("Value length > LENGTH_QUOTE!");
    }

    /**
     * Parse encoded string value.
     *
     * @param pduDataStream pdu data input stream
     * @return the EncodedStringValue
     */
    protected static EncodedStringValue ParseEncodedStringValue(ByteArrayInputStream pduDataStream){
        /**
         * From OMA-TS-MMS-ENC-V1_3-20050927-C.pdf
         * Encoded-string-value = Text-string | Value-length Char-set Text-string
         */
        Assert(NULL != pduDataStream);
        pduDataStream->Mark(1);
        EncodedStringValue returnValue = NULL;
        Int32 charset = 0;
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        Int32 first = temp & 0xFF;
        If (first == 0) {
            return new EncodedStringValue("");
        }

        pduDataStream->Reset();
        If (first < TEXT_MIN) {
            ParseValueLength(pduDataStream);

            charset = ParseShortInteger(pduDataStream); //get the "Charset"
        }

        Byte[] textString = ParseWapString(pduDataStream, TYPE_TEXT_STRING);

        try {
            If (0 != charset) {
                returnValue = new EncodedStringValue(charset, textString);
            } else {
                returnValue = new EncodedStringValue(textString);
            }
        } Catch(Exception e) {
            return NULL;
        }

        return returnValue;
    }

    /**
     * Parse Text-String or Quoted-String.
     *
     * @param pduDataStream pdu data input stream
     * @param stringType TYPE_TEXT_STRING or TYPE_QUOTED_STRING
     * @return the string without End-of-string in Byte array
     */
    protected static Byte[] ParseWapString(ByteArrayInputStream pduDataStream,
            Int32 stringType) {
        Assert(NULL != pduDataStream);
        /**
         * From wap-230-wsp-20010705-a.pdf
         * Text-string = [Quote] *TEXT End-of-string
         * If the first character in the TEXT is in the range of 128-255,
         * a Quote character must precede it.
         * Otherwise the Quote character must be omitted.
         * The Quote is not part of the contents.
         * Quote = <Octet 127>
         * End-of-string = <Octet 0>
         *
         * Quoted-string = <Octet 34> *TEXT End-of-string
         *
         * Token-text = Token End-of-string
         */

        // Mark supposed beginning of Text-string
        // We will have to mark again if first Char32 is QUOTE or QUOTED_STRING_FLAG
        pduDataStream->Mark(1);

        // Check first Char32
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        If ((TYPE_QUOTED_STRING == stringType) &&
                (QUOTED_STRING_FLAG == temp)) {
            // Mark again if QUOTED_STRING_FLAG and ignore it
            pduDataStream->Mark(1);
        } else If ((TYPE_TEXT_STRING == stringType) &&
                (QUOTE == temp)) {
            // Mark again if QUOTE and ignore it
            pduDataStream->Mark(1);
        } else {
            // Otherwise go back to origin
            pduDataStream->Reset();
        }

        // We are now definitely at the beginning of string
        /**
         * Return *TOKEN or *TEXT (Text-String without QUOTE,
         * Quoted-String without QUOTED_STRING_FLAG and without End-of-string)
         */
        return GetWapString(pduDataStream, stringType);
    }

    /**
     * Check TOKEN data defined in RFC2616.
     * @param ch checking data
     * @return TRUE when ch is TOKEN, FALSE when ch is not TOKEN
     */
    protected static Boolean IsTokenCharacter(Int32 ch) {
        /**
         * Token      = 1*<any CHAR except CTLs or separators>
         * separators = "("(40) | ")"(41) | "<"(60) | ">"(62) | "@"(64)
         *            | ","(44) | ";"(59) | ":"(58) | "\"(92) | <">(34)
         *            | "/"(47) | "["(91) | "]"(93) | "?"(63) | "="(61)
         *            | "{"(123) | "}"(125) | SP(32) | HT(9)
         * CHAR       = <any US-ASCII Character (octets 0 - 127)>
         * CTL        = <any US-ASCII control character
         *            (octets 0 - 31) and DEL (127)>
         * SP         = <US-ASCII SP, Space (32)>
         * HT         = <US-ASCII HT, horizontal-Tab (9)>
         */
        If((ch < 33) || (ch > 126)) {
            return FALSE;
        }

        Switch(ch) {
            case '"': /* '"' */
            case '(': /* '(' */
            case ')': /* ')' */
            case ',': /* ',' */
            case '/': /* '/' */
            case ':': /* ':' */
            case ';': /* ';' */
            case '<': /* '<' */
            case '=': /* '=' */
            case '>': /* '>' */
            case '?': /* '?' */
            case '@': /* '@' */
            case '[': /* '[' */
            case '\\': /* '\' */
            case ']': /* ']' */
            case '{': /* '{' */
            case '}': /* '}' */
                return FALSE;
        }

        return TRUE;
    }

    /**
     * Check TEXT data defined in RFC2616.
     * @param ch checking data
     * @return TRUE when ch is TEXT, FALSE when ch is not TEXT
     */
    protected static Boolean IsText(Int32 ch) {
        /**
         * TEXT = <any OCTET except CTLs,
         *      but including LWS>
         * CTL  = <any US-ASCII control character
         *      (octets 0 - 31) and DEL (127)>
         * LWS  = [CRLF] 1*( SP | HT )
         * CRLF = CR LF
         * CR   = <US-ASCII CR, carriage Return (13)>
         * LF   = <US-ASCII LF, Linefeed (10)>
         */
        If(((ch >= 32) && (ch <= 126)) || ((ch >= 128) && (ch <= 255))) {
            return TRUE;
        }

        Switch(ch) {
            case '\t': /* '\t' */
            case '\n': /* '\n' */
            case '\r': /* '\r' */
                return TRUE;
        }

        return FALSE;
    }

    protected static Byte[] GetWapString(ByteArrayInputStream pduDataStream,
            Int32 stringType) {
        Assert(NULL != pduDataStream);
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        While((-1 != temp) && ('\0' != temp)) {
            // check each of the character
            If (stringType == TYPE_TOKEN_STRING) {
                If (IsTokenCharacter(temp)) {
                    out->Write(temp);
                }
            } else {
                If (IsText(temp)) {
                    out->Write(temp);
                }
            }

            temp = pduDataStream->Read();
            Assert(-1 != temp);
        }

        If (out->Size() > 0) {
            return out->ToByteArray();
        }

        return NULL;
    }

    /**
     * Extract a Byte value from the input stream.
     *
     * @param pduDataStream pdu data input stream
     * @return the Byte
     */
    protected static Int32 ExtractByteValue(ByteArrayInputStream pduDataStream) {
        Assert(NULL != pduDataStream);
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        return temp & 0xFF;
    }

    /**
     * Parse Short-Integer.
     *
     * @param pduDataStream pdu data input stream
     * @return the Byte
     */
    protected static Int32 ParseShortInteger(ByteArrayInputStream pduDataStream) {
        /**
         * From wap-230-wsp-20010705-a.pdf
         * Short-integer = OCTET
         * Integers in range 0-127 shall be encoded as a one
         * octet value with the most significant bit set to One (1xxx xxxx)
         * and with the value in the remaining least significant bits.
         */
        Assert(NULL != pduDataStream);
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        return temp & 0x7F;
    }

    /**
     * Parse Long-Integer.
     *
     * @param pduDataStream pdu data input stream
     * @return Int64 integer
     */
    protected static Int64 ParseLongInteger(ByteArrayInputStream pduDataStream) {
        /**
         * From wap-230-wsp-20010705-a.pdf
         * Long-integer = Short-length Multi-octet-integer
         * The Short-length indicates the length of the Multi-octet-integer
         * Multi-octet-integer = 1*30 OCTET
         * The content octets shall be an unsigned integer value
         * with the most significant octet encoded First (big-endian representation).
         * The minimum number of octets must be used to encode the value.
         * Short-length = <Any octet 0-30>
         */
        Assert(NULL != pduDataStream);
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        Int32 count = temp & 0xFF;

        If (count > LONG_INTEGER_LENGTH_MAX) {
            throw new RuntimeException("Octet count greater than 8 and I can't represent that!");
        }

        Int64 result = 0;

        For (Int32 i = 0 ; i < count ; i++) {
            temp = pduDataStream->Read();
            Assert(-1 != temp);
            result <<= 8;
            result += (temp & 0xFF);
        }

        return result;
    }

    /**
     * Parse Integer-Value.
     *
     * @param pduDataStream pdu data input stream
     * @return Int64 integer
     */
    protected static Int64 ParseIntegerValue(ByteArrayInputStream pduDataStream) {
        /**
         * From wap-230-wsp-20010705-a.pdf
         * Integer-Value = Short-integer | Long-integer
         */
        Assert(NULL != pduDataStream);
        pduDataStream->Mark(1);
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        pduDataStream->Reset();
        If (temp > SHORT_INTEGER_MAX) {
            return ParseShortInteger(pduDataStream);
        } else {
            return ParseLongInteger(pduDataStream);
        }
    }

    /**
     * To skip length of the wap value.
     *
     * @param pduDataStream pdu data input stream
     * @param length area size
     * @return the values in this area
     */
    protected static Int32 SkipWapValue(ByteArrayInputStream pduDataStream, Int32 length) {
        Assert(NULL != pduDataStream);
        Byte[] area = new Byte[length];
        Int32 readLen = pduDataStream->Read(area, 0, length);
        If (readLen < length) { //The actually read length is lower than the length
            return -1;
        } else {
            return readLen;
        }
    }

    /**
     * Parse content type parameters. For now we just support
     * four parameters used in mms: "type", "start", "name", "charset".
     *
     * @param pduDataStream pdu data input stream
     * @param map to store parameters of Content-Type field
     * @param length length of all the parameters
     */
    protected static void ParseContentTypeParams(ByteArrayInputStream pduDataStream,
            HashMap<Integer, Object> map, Integer length) {
        /**
         * From wap-230-wsp-20010705-a.pdf
         * Parameter = Typed-parameter | Untyped-parameter
         * Typed-parameter = Well-known-parameter-token Typed-value
         * the actual expected type of the value is implied by the well-known parameter
         * Well-known-parameter-token = Integer-value
         * the code values used for parameters are specified in the Assigned Numbers appendix
         * Typed-value = Compact-value | Text-value
         * In addition to the expected type, there may be no value.
         * If the value cannot be encoded using the expected type, it shall be encoded as text.
         * Compact-value = Integer-value |
         * Date-value | Delta-seconds-value | Q-value | Version-value |
         * Uri-value
         * Untyped-parameter = Token-text Untyped-value
         * the type of the value is unknown, but it shall be encoded as an integer,
         * if that is possible.
         * Untyped-value = Integer-value | Text-value
         */
        Assert(NULL != pduDataStream);
        Assert(length > 0);

        Int32 startPos = pduDataStream->Available();
        Int32 tempPos = 0;
        Int32 lastLen = length;
        While(0 < lastLen) {
            Int32 param = pduDataStream->Read();
            Assert(-1 != param);
            lastLen--;

            Switch (param) {
                /**
                 * From rfc2387, chapter 3.1
                 * The type parameter must be specified and its value is the MIME media
                 * type of the "root" body part. It permits a MIME user agent to
                 * determine the content-type without reference to the enclosed body
                 * part. If the value of the type parameter and the root body part's
                 * content-type differ then the User Agent's behavior is undefined.
                 *
                 * From wap-230-wsp-20010705-a.pdf
                 * type = Constrained-encoding
                 * Constrained-encoding = Extension-Media | Short-integer
                 * Extension-media = *TEXT End-of-string
                 */
                case PduPart.P_TYPE:
                case PduPart.P_CT_MR_TYPE:
                    pduDataStream->Mark(1);
                    Int32 first = ExtractByteValue(pduDataStream);
                    pduDataStream->Reset();
                    If (first > TEXT_MAX) {
                        // Short-Integer (well-known type)
                        Int32 index = ParseShortInteger(pduDataStream);

                        If (index < PduContentTypes.contentTypes.length) {
                            Byte[] type = (PduContentTypes.contentTypes[index]).GetBytes();
                            map->Put(PduPart.P_TYPE, type);
                        } else {
                            //not support this type, ignore it.
                        }
                    } else {
                        // Text-String (extension-media)
                        Byte[] type = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                        If ((NULL != type) && (NULL != map)) {
                            map->Put(PduPart.P_TYPE, type);
                        }
                    }

                    tempPos = pduDataStream->Available();
                    lastLen = length - (startPos - tempPos);
                    break;

                    /**
                     * From oma-ts-mms-conf-v1_3.pdf, chapter 10.2.3.
                     * Start Parameter Referring to Presentation
                     *
                     * From rfc2387, chapter 3.2
                     * The start parameter, if given, is the content-ID of the compound
                     * object's "root". If not present the "root" is the first body part in
                     * the Multipart/Related entity. The "root" is the element the
                     * applications processes first.
                     *
                     * From wap-230-wsp-20010705-a.pdf
                     * start = Text-String
                     */
                case PduPart.P_START:
                case PduPart.P_DEP_START:
                    Byte[] start = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                    If ((NULL != start) && (NULL != map)) {
                        map->Put(PduPart.P_START, start);
                    }

                    tempPos = pduDataStream->Available();
                    lastLen = length - (startPos - tempPos);
                    break;

                    /**
                     * From oma-ts-mms-conf-v1_3.pdf
                     * In creation, the character set SHALL be either us-ascii
                     * (IANA MIBenum 3) or utf-8 (IANA MIBenum 106)[Unicode].
                     * In retrieval, both us-ascii and utf-8 SHALL be supported.
                     *
                     * From wap-230-wsp-20010705-a.pdf
                     * charset = Well-known-charset|Text-String
                     * Well-known-charset = Any-charset | Integer-value
                     * Both are encoded using values from Character Set
                     * Assignments table in Assigned Numbers
                     * Any-charset = <Octet 128>
                     * Equivalent to the special RFC2616 charset value "*"
                     */
                case PduPart.P_CHARSET:
                    pduDataStream->Mark(1);
                    Int32 firstValue = ExtractByteValue(pduDataStream);
                    pduDataStream->Reset();
                    //Check first Char32
                    If (((firstValue > TEXT_MIN) && (firstValue < TEXT_MAX)) ||
                            (END_STRING_FLAG == firstValue)) {
                        //Text-String (extension-charset)
                        Byte[] charsetStr = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                        try {
                            Int32 charsetInt = CharacterSets->GetMibEnumValue(
                                    new String(charsetStr));
                            map->Put(PduPart.P_CHARSET, charsetInt);
                        } Catch (UnsupportedEncodingException e) {
                            // Not a well-known charset, use "*".
                            Logger::E(LOG_TAG, Arrays->ToString(charsetStr), e);
                            map->Put(PduPart.P_CHARSET, CharacterSets.ANY_CHARSET);
                        }
                    } else {
                        //Well-known-charset
                        Int32 charset = (Int32) ParseIntegerValue(pduDataStream);
                        If (map != NULL) {
                            map->Put(PduPart.P_CHARSET, charset);
                        }
                    }

                    tempPos = pduDataStream->Available();
                    lastLen = length - (startPos - tempPos);
                    break;

                    /**
                     * From oma-ts-mms-conf-v1_3.pdf
                     * A name for multipart object SHALL be encoded using name-parameter
                     * for Content-Type header in WSP multipart headers.
                     *
                     * From wap-230-wsp-20010705-a.pdf
                     * name = Text-String
                     */
                case PduPart.P_DEP_NAME:
                case PduPart.P_NAME:
                    Byte[] name = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                    If ((NULL != name) && (NULL != map)) {
                        map->Put(PduPart.P_NAME, name);
                    }

                    tempPos = pduDataStream->Available();
                    lastLen = length - (startPos - tempPos);
                    break;
                default:
                    If (LOCAL_LOGV) {
                        Logger::V(LOG_TAG, "Not supported Content-Type parameter");
                    }
                If (-1 == SkipWapValue(pduDataStream, lastLen)) {
                    Logger::E(LOG_TAG, "Corrupt Content-Type");
                } else {
                    lastLen = 0;
                }
                break;
            }
        }

        If (0 != lastLen) {
            Logger::E(LOG_TAG, "Corrupt Content-Type");
        }
    }

    /**
     * Parse content type.
     *
     * @param pduDataStream pdu data input stream
     * @param map to store parameters in Content-Type header field
     * @return Content-Type value
     */
    protected static Byte[] ParseContentType(ByteArrayInputStream pduDataStream,
            HashMap<Integer, Object> map) {
        /**
         * From wap-230-wsp-20010705-a.pdf
         * Content-type-value = Constrained-media | Content-general-form
         * Content-general-form = Value-length Media-type
         * Media-type = (Well-known-media | Extension-Media) *(Parameter)
         */
        Assert(NULL != pduDataStream);

        Byte[] contentType = NULL;
        pduDataStream->Mark(1);
        Int32 temp = pduDataStream->Read();
        Assert(-1 != temp);
        pduDataStream->Reset();

        Int32 cur = (temp & 0xFF);

        If (cur < TEXT_MIN) {
            Int32 length = ParseValueLength(pduDataStream);
            Int32 startPos = pduDataStream->Available();
            pduDataStream->Mark(1);
            temp = pduDataStream->Read();
            Assert(-1 != temp);
            pduDataStream->Reset();
            Int32 first = (temp & 0xFF);

            If ((first >= TEXT_MIN) && (first <= TEXT_MAX)) {
                contentType = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
            } else If (first > TEXT_MAX) {
                Int32 index = ParseShortInteger(pduDataStream);

                If (index < PduContentTypes.contentTypes.length) { //well-known type
                    contentType = (PduContentTypes.contentTypes[index]).GetBytes();
                } else {
                    pduDataStream->Reset();
                    contentType = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                }
            } else {
                Logger::E(LOG_TAG, "Corrupt content-type");
                Return (PduContentTypes.contentTypes[0]).GetBytes(); //"*/*"
            }

            Int32 endPos = pduDataStream->Available();
            Int32 parameterLen = length - (startPos - endPos);
            If (parameterLen > 0) {//have parameters
                ParseContentTypeParams(pduDataStream, map, parameterLen);
            }

            If (parameterLen < 0) {
                Logger::E(LOG_TAG, "Corrupt MMS message");
                Return (PduContentTypes.contentTypes[0]).GetBytes(); //"*/*"
            }
        } else If (cur <= TEXT_MAX) {
            contentType = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
        } else {
            contentType =
                (PduContentTypes.contentTypes[ParseShortInteger(pduDataStream)]).GetBytes();
        }

        return contentType;
    }

    /**
     * Parse part's headers.
     *
     * @param pduDataStream pdu data input stream
     * @param part to store the header informations of the part
     * @param length length of the headers
     * @return TRUE if parse successfully, FALSE otherwise
     */
    protected static Boolean ParsePartHeaders(ByteArrayInputStream pduDataStream,
            PduPart part, Int32 length) {
        Assert(NULL != pduDataStream);
        Assert(NULL != part);
        Assert(length > 0);

        /**
         * From oma-ts-mms-conf-v1_3.pdf, chapter 10.2.
         * A name for multipart object SHALL be encoded using name-parameter
         * for Content-Type header in WSP multipart headers.
         * In decoding, name-parameter of Content-Type SHALL be used if available.
         * If name-parameter of Content-Type is not available,
         * filename parameter of Content-Disposition header SHALL be used if available.
         * If neither name-parameter of Content-Type header nor filename parameter
         * of Content-Disposition header is available,
         * Content-Location header SHALL be used if available.
         *
         * Within SMIL part the reference to the media object parts SHALL use
         * either Content-ID or Content-Location mechanism [RFC2557]
         * and the corresponding WSP part headers in media object parts
         * contain the corresponding definitions.
         */
        Int32 startPos = pduDataStream->Available();
        Int32 tempPos = 0;
        Int32 lastLen = length;
        While(0 < lastLen) {
            Int32 header = pduDataStream->Read();
            Assert(-1 != header);
            lastLen--;

            If (header > TEXT_MAX) {
                // Number assigned headers.
                Switch (header) {
                    case PduPart.P_CONTENT_LOCATION:
                        /**
                         * From wap-230-wsp-20010705-a.pdf, chapter 8.4.2.21
                         * Content-location-value = Uri-value
                         */
                        Byte[] contentLocation = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                        If (NULL != contentLocation) {
                            part->SetContentLocation(contentLocation);
                        }

                        tempPos = pduDataStream->Available();
                        lastLen = length - (startPos - tempPos);
                        break;
                    case PduPart.P_CONTENT_ID:
                        /**
                         * From wap-230-wsp-20010705-a.pdf, chapter 8.4.2.21
                         * Content-ID-value = Quoted-string
                         */
                        Byte[] contentId = ParseWapString(pduDataStream, TYPE_QUOTED_STRING);
                        If (NULL != contentId) {
                            part->SetContentId(contentId);
                        }

                        tempPos = pduDataStream->Available();
                        lastLen = length - (startPos - tempPos);
                        break;
                    case PduPart.P_DEP_CONTENT_DISPOSITION:
                    case PduPart.P_CONTENT_DISPOSITION:
                        /**
                         * From wap-230-wsp-20010705-a.pdf, chapter 8.4.2.21
                         * Content-disposition-value = Value-length Disposition *(Parameter)
                         * Disposition = Form-data | Attachment | Inline | Token-text
                         * Form-data = <Octet 128>
                         * Attachment = <Octet 129>
                         * Inline = <Octet 130>
                         */

                        /*
                         * some carrier mmsc servers do not support content_disposition
                         * field correctly
                         */
                        Boolean contentDisposition = Resources->GetSystem()->GetBoolean(com
                                .android.internal.R.bool.config_mms_content_disposition_support);

                        If (contentDisposition) {
                            Int32 len = ParseValueLength(pduDataStream);
                            pduDataStream->Mark(1);
                            Int32 thisStartPos = pduDataStream->Available();
                            Int32 thisEndPos = 0;
                            Int32 value = pduDataStream->Read();

                            If (value == PduPart.P_DISPOSITION_FROM_DATA ) {
                                part->SetContentDisposition(PduPart.DISPOSITION_FROM_DATA);
                            } else If (value == PduPart.P_DISPOSITION_ATTACHMENT) {
                                part->SetContentDisposition(PduPart.DISPOSITION_ATTACHMENT);
                            } else If (value == PduPart.P_DISPOSITION_INLINE) {
                                part->SetContentDisposition(PduPart.DISPOSITION_INLINE);
                            } else {
                                pduDataStream->Reset();
                                /* Token-text */
                                part->SetContentDisposition(ParseWapString(pduDataStream
                                        , TYPE_TEXT_STRING));
                            }

                            /* get filename parameter and skip other parameters */
                            thisEndPos = pduDataStream->Available();
                            If (thisStartPos - thisEndPos < len) {
                                value = pduDataStream->Read();
                                If (value == PduPart.P_FILENAME) { //filename is text-string
                                    part->SetFilename(ParseWapString(pduDataStream
                                            , TYPE_TEXT_STRING));
                                }

                                /* skip other parameters */
                                thisEndPos = pduDataStream->Available();
                                If (thisStartPos - thisEndPos < len) {
                                    Int32 last = len - (thisStartPos - thisEndPos);
                                    Byte[] temp = new Byte[last];
                                    pduDataStream->Read(temp, 0, last);
                                }
                            }

                            tempPos = pduDataStream->Available();
                            lastLen = length - (startPos - tempPos);
                        }
                        break;
                    default:
                        If (LOCAL_LOGV) {
                            Logger::V(LOG_TAG, "Not supported Part headers: " + header);
                        }
                    If (-1 == SkipWapValue(pduDataStream, lastLen)) {
                        Logger::E(LOG_TAG, "Corrupt Part headers");
                        return FALSE;
                    }
                    lastLen = 0;
                    break;
                }
            } else If ((header >= TEXT_MIN) && (header <= TEXT_MAX)) {
                // Not assigned header.
                Byte[] tempHeader = ParseWapString(pduDataStream, TYPE_TEXT_STRING);
                Byte[] tempValue = ParseWapString(pduDataStream, TYPE_TEXT_STRING);

                // Check the header whether it is "Content-Transfer-Encoding".
                If (TRUE ==
                    PduPart.CONTENT_TRANSFER_ENCODING->EqualsIgnoreCase(new String(tempHeader))) {
                    part->SetContentTransferEncoding(tempValue);
                }

                tempPos = pduDataStream->Available();
                lastLen = length - (startPos - tempPos);
            } else {
                If (LOCAL_LOGV) {
                    Logger::V(LOG_TAG, "Not supported Part headers: " + header);
                }
                // Skip all headers of this part.
                If (-1 == SkipWapValue(pduDataStream, lastLen)) {
                    Logger::E(LOG_TAG, "Corrupt Part headers");
                    return FALSE;
                }
                lastLen = 0;
            }
        }

        If (0 != lastLen) {
            Logger::E(LOG_TAG, "Corrupt Part headers");
            return FALSE;
        }

        return TRUE;
    }

    /**
     * Check the position of a specified part.
     *
     * @param part the part to be checked
     * @return part position, THE_FIRST_PART when it's the
     * first one, THE_LAST_PART when it's the last one.
     */
    private static Int32 CheckPartPosition(PduPart part) {
        Assert(NULL != part);
        If ((NULL == mTypeParam) &&
                (NULL == mStartParam)) {
            return THE_LAST_PART;
        }

        /* check part's content-id */
        If (NULL != mStartParam) {
            Byte[] contentId = part->GetContentId();
            If (NULL != contentId) {
                If (TRUE == Arrays->Equals(mStartParam, contentId)) {
                    return THE_FIRST_PART;
                }
            }
        }

        /* check part's content-type */
        If (NULL != mTypeParam) {
            Byte[] contentType = part->GetContentType();
            If (NULL != contentType) {
                If (TRUE == Arrays->Equals(mTypeParam, contentType)) {
                    return THE_FIRST_PART;
                }
            }
        }

        return THE_LAST_PART;
    }

    /**
     * Check mandatory headers of a pdu.
     *
     * @param headers pdu headers
     * @return TRUE if the pdu has all of the mandatory headers, FALSE otherwise.
     */
    protected static Boolean CheckMandatoryHeader(PduHeaders headers) {
        If (NULL == headers) {
            return FALSE;
        }

        /* get message type */
        Int32 messageType = headers->GetOctet(PduHeaders.MESSAGE_TYPE);

        /* check Mms-Version field */
        Int32 mmsVersion = headers->GetOctet(PduHeaders.MMS_VERSION);
        If (0 == mmsVersion) {
            // Every message should have Mms-Version field.
            return FALSE;
        }

        /* check mandatory header fields */
        Switch (messageType) {
            case PduHeaders.MESSAGE_TYPE_SEND_REQ:
                // Content-Type field.
                Byte[] srContentType = headers->GetTextString(PduHeaders.CONTENT_TYPE);
                If (NULL == srContentType) {
                    return FALSE;
                }

                // From field.
                EncodedStringValue srFrom = headers->GetEncodedStringValue(PduHeaders.FROM);
                If (NULL == srFrom) {
                    return FALSE;
                }

                // Transaction-Id field.
                Byte[] srTransactionId = headers->GetTextString(PduHeaders.TRANSACTION_ID);
                If (NULL == srTransactionId) {
                    return FALSE;
                }

                break;
            case PduHeaders.MESSAGE_TYPE_SEND_CONF:
                // Response-Status field.
                Int32 scResponseStatus = headers->GetOctet(PduHeaders.RESPONSE_STATUS);
                If (0 == scResponseStatus) {
                    return FALSE;
                }

                // Transaction-Id field.
                Byte[] scTransactionId = headers->GetTextString(PduHeaders.TRANSACTION_ID);
                If (NULL == scTransactionId) {
                    return FALSE;
                }

                break;
            case PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND:
                // Content-Location field.
                Byte[] niContentLocation = headers->GetTextString(PduHeaders.CONTENT_LOCATION);
                If (NULL == niContentLocation) {
                    return FALSE;
                }

                // Expiry field.
                Int64 niExpiry = headers->GetLongInteger(PduHeaders.EXPIRY);
                If (-1 == niExpiry) {
                    return FALSE;
                }

                // Message-Class field.
                Byte[] niMessageClass = headers->GetTextString(PduHeaders.MESSAGE_CLASS);
                If (NULL == niMessageClass) {
                    return FALSE;
                }

                // Message-Size field.
                Int64 niMessageSize = headers->GetLongInteger(PduHeaders.MESSAGE_SIZE);
                If (-1 == niMessageSize) {
                    return FALSE;
                }

                // Transaction-Id field.
                Byte[] niTransactionId = headers->GetTextString(PduHeaders.TRANSACTION_ID);
                If (NULL == niTransactionId) {
                    return FALSE;
                }

                break;
            case PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND:
                // Status field.
                Int32 nriStatus = headers->GetOctet(PduHeaders.STATUS);
                If (0 == nriStatus) {
                    return FALSE;
                }

                // Transaction-Id field.
                Byte[] nriTransactionId = headers->GetTextString(PduHeaders.TRANSACTION_ID);
                If (NULL == nriTransactionId) {
                    return FALSE;
                }

                break;
            case PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF:
                // Content-Type field.
                Byte[] rcContentType = headers->GetTextString(PduHeaders.CONTENT_TYPE);
                If (NULL == rcContentType) {
                    return FALSE;
                }

                // Date field.
                Int64 rcDate = headers->GetLongInteger(PduHeaders.DATE);
                If (-1 == rcDate) {
                    return FALSE;
                }

                break;
            case PduHeaders.MESSAGE_TYPE_DELIVERY_IND:
                // Date field.
                Int64 diDate = headers->GetLongInteger(PduHeaders.DATE);
                If (-1 == diDate) {
                    return FALSE;
                }

                // Message-Id field.
                Byte[] diMessageId = headers->GetTextString(PduHeaders.MESSAGE_ID);
                If (NULL == diMessageId) {
                    return FALSE;
                }

                // Status field.
                Int32 diStatus = headers->GetOctet(PduHeaders.STATUS);
                If (0 == diStatus) {
                    return FALSE;
                }

                // To field.
                EncodedStringValue[] diTo = headers->GetEncodedStringValues(PduHeaders.TO);
                If (NULL == diTo) {
                    return FALSE;
                }

                break;
            case PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND:
                // Transaction-Id field.
                Byte[] aiTransactionId = headers->GetTextString(PduHeaders.TRANSACTION_ID);
                If (NULL == aiTransactionId) {
                    return FALSE;
                }

                break;
            case PduHeaders.MESSAGE_TYPE_READ_ORIG_IND:
                // Date field.
                Int64 roDate = headers->GetLongInteger(PduHeaders.DATE);
                If (-1 == roDate) {
                    return FALSE;
                }

                // From field.
                EncodedStringValue roFrom = headers->GetEncodedStringValue(PduHeaders.FROM);
                If (NULL == roFrom) {
                    return FALSE;
                }

                // Message-Id field.
                Byte[] roMessageId = headers->GetTextString(PduHeaders.MESSAGE_ID);
                If (NULL == roMessageId) {
                    return FALSE;
                }

                // Read-Status field.
                Int32 roReadStatus = headers->GetOctet(PduHeaders.READ_STATUS);
                If (0 == roReadStatus) {
                    return FALSE;
                }

                // To field.
                EncodedStringValue[] roTo = headers->GetEncodedStringValues(PduHeaders.TO);
                If (NULL == roTo) {
                    return FALSE;
                }

                break;
            case PduHeaders.MESSAGE_TYPE_READ_REC_IND:
                // From field.
                EncodedStringValue rrFrom = headers->GetEncodedStringValue(PduHeaders.FROM);
                If (NULL == rrFrom) {
                    return FALSE;
                }

                // Message-Id field.
                Byte[] rrMessageId = headers->GetTextString(PduHeaders.MESSAGE_ID);
                If (NULL == rrMessageId) {
                    return FALSE;
                }

                // Read-Status field.
                Int32 rrReadStatus = headers->GetOctet(PduHeaders.READ_STATUS);
                If (0 == rrReadStatus) {
                    return FALSE;
                }

                // To field.
                EncodedStringValue[] rrTo = headers->GetEncodedStringValues(PduHeaders.TO);
                If (NULL == rrTo) {
                    return FALSE;
                }

                break;
            default:
                // Parser doesn't support this message type in this version.
                return FALSE;
        }

        return TRUE;
    }
}
