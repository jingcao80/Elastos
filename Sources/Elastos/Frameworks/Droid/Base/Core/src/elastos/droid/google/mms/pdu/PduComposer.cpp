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

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Text::ITextUtils;

using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::IArrays;
using Elastos::Utility::IHashMap;

public class PduComposer {
    /**
     * Address type.
     */
    static private final Int32 PDU_PHONE_NUMBER_ADDRESS_TYPE = 1;
    static private final Int32 PDU_EMAIL_ADDRESS_TYPE = 2;
    static private final Int32 PDU_IPV4_ADDRESS_TYPE = 3;
    static private final Int32 PDU_IPV6_ADDRESS_TYPE = 4;
    static private final Int32 PDU_UNKNOWN_ADDRESS_TYPE = 5;

    /**
     * Address regular expression string.
     */
    static const String REGEXP_PHONE_NUMBER_ADDRESS_TYPE = "\\+?[0-9|\\.|\\-]+";
    static const String REGEXP_EMAIL_ADDRESS_TYPE = "[a-zA-Z| ]*\\<{0,1}[a-zA-Z| ]+@{1}" +
            "[a-zA-Z| ]+\\.{1}[a-zA-Z| ]+\\>{0,1}";
    static const String REGEXP_IPV6_ADDRESS_TYPE =
        "[a-fA-F]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}" +
        "[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}\\:{1}" +
        "[a-fA-F0-9]{4}\\:{1}[a-fA-F0-9]{4}";
    static const String REGEXP_IPV4_ADDRESS_TYPE = "[0-9]{1,3}\\.{1}[0-9]{1,3}\\.{1}" +
            "[0-9]{1,3}\\.{1}[0-9]{1,3}";

    /**
     * The postfix strings of address.
     */
    static const String STRING_PHONE_NUMBER_ADDRESS_TYPE = "/TYPE=PLMN";
    static const String STRING_IPV4_ADDRESS_TYPE = "/TYPE=IPV4";
    static const String STRING_IPV6_ADDRESS_TYPE = "/TYPE=IPV6";

    /**
     * Error values.
     */
    static private final Int32 PDU_COMPOSE_SUCCESS = 0;
    static private final Int32 PDU_COMPOSE_CONTENT_ERROR = 1;
    static private final Int32 PDU_COMPOSE_FIELD_NOT_SET = 2;
    static private final Int32 PDU_COMPOSE_FIELD_NOT_SUPPORTED = 3;

    /**
     * WAP values defined in WSP spec.
     */
    static private final Int32 QUOTED_STRING_FLAG = 34;
    static private final Int32 END_STRING_FLAG = 0;
    static private final Int32 LENGTH_QUOTE = 31;
    static private final Int32 TEXT_MAX = 127;
    static private final Int32 SHORT_INTEGER_MAX = 127;
    static private final Int32 LONG_INTEGER_LENGTH_MAX = 8;

    /**
     * Block size when read data from InputStream.
     */
    static private final Int32 PDU_COMPOSER_BLOCK_SIZE = 1024;

    /**
     * The output message.
     */
    protected ByteArrayOutputStream mMessage = NULL;

    /**
     * The PDU.
     */
    private GenericPdu mPdu = NULL;

    /**
     * Current visiting position of the mMessage.
     */
    protected Int32 mPosition = 0;

    /**
     * Message compose buffer stack.
     */
    private BufferStack mStack = NULL;

    /**
     * Content resolver.
     */
    private final ContentResolver mResolver;

    /**
     * Header of this pdu.
     */
    private PduHeaders mPduHeader = NULL;

    /**
     * Map of all content type
     */
    private static HashMap<String, Integer> mContentTypeMap = NULL;

    static {
        mContentTypeMap = new HashMap<String, Integer>();

        Int32 i;
        For (i = 0; i < PduContentTypes.contentTypes.length; i++) {
            mContentTypeMap->Put(PduContentTypes.contentTypes[i], i);
        }
    }

    /**
     * Constructor.
     *
     * @param context the context
     * @param pdu the pdu to be composed
     */
    public PduComposer(Context context, GenericPdu pdu) {
        mPdu = pdu;
        mResolver = context->GetContentResolver();
        mPduHeader = pdu->GetPduHeaders();
        mStack = new BufferStack();
        mMessage = new ByteArrayOutputStream();
        mPosition = 0;
    }

    /**
     * Make the message. No need to check whether mandatory fields are set,
     * because the constructors of outgoing pdus are taking care of this.
     *
     * @return OutputStream of maked message. Return NULL if
     *         the PDU is invalid.
     */
    public Byte[] Make() {
        // Get Message-type.
        Int32 type = mPdu->GetMessageType();

        /* make the message */
        Switch (type) {
            case PduHeaders.MESSAGE_TYPE_SEND_REQ:
            case PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF:
                If (MakeSendRetrievePdu(type) != PDU_COMPOSE_SUCCESS) {
                    return NULL;
                }
                break;
            case PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND:
                If (MakeNotifyResp() != PDU_COMPOSE_SUCCESS) {
                    return NULL;
                }
                break;
            case PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND:
                If (MakeAckInd() != PDU_COMPOSE_SUCCESS) {
                    return NULL;
                }
                break;
            case PduHeaders.MESSAGE_TYPE_READ_REC_IND:
                If (MakeReadRecInd() != PDU_COMPOSE_SUCCESS) {
                    return NULL;
                }
                break;
            default:
                return NULL;
        }

        return mMessage->ToByteArray();
    }

    /**
     *  Copy buf to mMessage.
     */
    protected void Arraycopy(Byte[] buf, Int32 pos, Int32 length) {
        mMessage->Write(buf, pos, length);
        mPosition = mPosition + length;
    }

    /**
     * Append a Byte to mMessage.
     */
    protected void Append(Int32 value) {
        mMessage->Write(value);
        mPosition ++;
    }

    /**
     * Append short integer value to mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendShortInteger(Int32 value) {
        /*
         * From WAP-230-WSP-20010705-a:
         * Short-integer = OCTET
         * ; Integers in range 0-127 shall be encoded as a one octet value
         * ; with the most significant bit set to One (1xxx xxxx) and with
         * ; the value in the remaining least significant bits.
         * In our implementation, only low 7 bits are stored and otherwise
         * bits are ignored.
         */
        Append((value | 0x80) & 0xff);
    }

    /**
     * Append an octet number between 128 and 255 into mMessage.
     * NOTE:
     * A value between 0 and 127 should be appended by using appendShortInteger.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendOctet(Int32 number) {
        Append(number);
    }

    /**
     * Append a short length into mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendShortLength(Int32 value) {
        /*
         * From WAP-230-WSP-20010705-a:
         * Short-length = <Any octet 0-30>
         */
        Append(value);
    }

    /**
     * Append Int64 integer into mMessage. it's used for really Int64 integers.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendLongInteger(Int64 longInt) {
        /*
         * From WAP-230-WSP-20010705-a:
         * Long-integer = Short-length Multi-octet-integer
         * ; The Short-length indicates the length of the Multi-octet-integer
         * Multi-octet-integer = 1*30 OCTET
         * ; The content octets shall be an unsigned integer value with the
         * ; most significant octet encoded First (big-endian representation).
         * ; The minimum number of octets must be used to encode the value.
         */
        Int32 size;
        Int64 temp = longInt;

        // Count the length of the Int64 integer.
        For(size = 0; (temp != 0) && (size < LONG_INTEGER_LENGTH_MAX); size++) {
            temp = (temp >>> 8);
        }

        // Set Length.
        AppendShortLength(size);

        // Count and set the Int64 integer.
        Int32 i;
        Int32 shift = (size -1) * 8;

        For (i = 0; i < size; i++) {
            Append((Int32)((longInt >>> shift) & 0xff));
            shift = shift - 8;
        }
    }

    /**
     * Append text string into mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendTextString(Byte[] text) {
        /*
         * From WAP-230-WSP-20010705-a:
         * Text-string = [Quote] *TEXT End-of-string
         * ; If the first character in the TEXT is in the range of 128-255,
         * ; a Quote character must precede it. Otherwise the Quote character
         * ;must be omitted. The Quote is not part of the contents.
         */
        If (((text[0])&0xff) > TEXT_MAX) { // No need to check for <= 255
            Append(TEXT_MAX);
        }

        Arraycopy(text, 0, text.length);
        Append(0);
    }

    /**
     * Append text string into mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendTextString(String str) {
        /*
         * From WAP-230-WSP-20010705-a:
         * Text-string = [Quote] *TEXT End-of-string
         * ; If the first character in the TEXT is in the range of 128-255,
         * ; a Quote character must precede it. Otherwise the Quote character
         * ;must be omitted. The Quote is not part of the contents.
         */
        AppendTextString(str->GetBytes());
    }

    /**
     * Append encoded string value to mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendEncodedString(EncodedStringValue enStr) {
        /*
         * From OMA-TS-MMS-ENC-V1_3-20050927-C:
         * Encoded-string-value = Text-string | Value-length Char-set Text-string
         */
        Assert(enStr != NULL);

        Int32 charset = enStr->GetCharacterSet();
        Byte[] textString = enStr->GetTextString();
        If (NULL == textString) {
            return;
        }

        /*
         * In the implementation of EncodedStringValue, the charset field will
         * never be 0. It will always be composed as
         * Encoded-string-value = Value-length Char-set Text-string
         */
        mStack->Newbuf();
        PositionMarker start = mStack->Mark();

        AppendShortInteger(charset);
        AppendTextString(textString);

        Int32 len = start->GetLength();
        mStack->Pop();
        AppendValueLength(len);
        mStack->Copy();
    }

    /**
     * Append uintvar integer into mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendUintvarInteger(Int64 value) {
        /*
         * From WAP-230-WSP-20010705-a:
         * To encode a large unsigned integer, split it into 7-bit fragments
         * and place them in the payloads of multiple octets. The most significant
         * bits are placed in the first octets with the least significant bits
         * ending up in the last octet. All octets MUST set the Continue bit to 1
         * except the last octet, which MUST set the Continue bit to 0.
         */
        Int32 i;
        Int64 max = SHORT_INTEGER_MAX;

        For (i = 0; i < 5; i++) {
            If (value < max) {
                break;
            }

            max = (max << 7) | 0x7fl;
        }

        While(i > 0) {
            Int64 temp = value >>> (i * 7);
            temp = temp & 0x7f;

            Append((Int32)((temp | 0x80) & 0xff));

            i--;
        }

        Append((Int32)(value & 0x7f));
    }

    /**
     * Append date value into mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendDateValue(Int64 date) {
        /*
         * From OMA-TS-MMS-ENC-V1_3-20050927-C:
         * Date-value = Long-integer
         */
        AppendLongInteger(date);
    }

    /**
     * Append value length to mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendValueLength(Int64 value) {
        /*
         * From WAP-230-WSP-20010705-a:
         * Value-length = Short-length | (Length-quote Length)
         * ; Value length is used to indicate the length of the value to follow
         * Short-length = <Any octet 0-30>
         * Length-quote = <Octet 31>
         * Length = Uintvar-integer
         */
        If (value < LENGTH_QUOTE) {
            AppendShortLength((Int32) value);
            return;
        }

        Append(LENGTH_QUOTE);
        AppendUintvarInteger(value);
    }

    /**
     * Append quoted string to mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendQuotedString(Byte[] text) {
        /*
         * From WAP-230-WSP-20010705-a:
         * Quoted-string = <Octet 34> *TEXT End-of-string
         * ;The TEXT encodes an RFC2616 Quoted-string with the enclosing
         * ;quotation-marks <"> removed.
         */
        Append(QUOTED_STRING_FLAG);
        Arraycopy(text, 0, text.length);
        Append(END_STRING_FLAG);
    }

    /**
     * Append quoted string to mMessage.
     * This implementation doesn't check the validity of parameter, since it
     * assumes that the values are validated in the GenericPdu setter methods.
     */
    protected void AppendQuotedString(String str) {
        /*
         * From WAP-230-WSP-20010705-a:
         * Quoted-string = <Octet 34> *TEXT End-of-string
         * ;The TEXT encodes an RFC2616 Quoted-string with the enclosing
         * ;quotation-marks <"> removed.
         */
        AppendQuotedString(str->GetBytes());
    }

    private EncodedStringValue AppendAddressType(EncodedStringValue address) {
        EncodedStringValue temp = NULL;

        try {
            Int32 addressType = CheckAddressType(address->GetString());
            temp = EncodedStringValue->Copy(address);
            If (PDU_PHONE_NUMBER_ADDRESS_TYPE == addressType) {
                // Phone number.
                temp->AppendTextString(STRING_PHONE_NUMBER_ADDRESS_TYPE->GetBytes());
            } else If (PDU_IPV4_ADDRESS_TYPE == addressType) {
                // Ipv4 address.
                temp->AppendTextString(STRING_IPV4_ADDRESS_TYPE->GetBytes());
            } else If (PDU_IPV6_ADDRESS_TYPE == addressType) {
                // Ipv6 address.
                temp->AppendTextString(STRING_IPV6_ADDRESS_TYPE->GetBytes());
            }
        } Catch (NullPointerException e) {
            return NULL;
        }

        return temp;
    }

    /**
     * Append header to mMessage.
     */
    private Int32 AppendHeader(Int32 field) {
        Switch (field) {
            case PduHeaders.MMS_VERSION:
                AppendOctet(field);

                Int32 version = mPduHeader->GetOctet(field);
                If (0 == version) {
                    AppendShortInteger(PduHeaders.CURRENT_MMS_VERSION);
                } else {
                    AppendShortInteger(version);
                }

                break;

            case PduHeaders.MESSAGE_ID:
            case PduHeaders.TRANSACTION_ID:
                Byte[] textString = mPduHeader->GetTextString(field);
                If (NULL == textString) {
                    return PDU_COMPOSE_FIELD_NOT_SET;
                }

                AppendOctet(field);
                AppendTextString(textString);
                break;

            case PduHeaders.TO:
            case PduHeaders.BCC:
            case PduHeaders.CC:
                EncodedStringValue[] addr = mPduHeader->GetEncodedStringValues(field);

                If (NULL == addr) {
                    return PDU_COMPOSE_FIELD_NOT_SET;
                }

                EncodedStringValue temp;
                For (Int32 i = 0; i < addr.length; i++) {
                    temp = AppendAddressType(addr[i]);
                    If (temp == NULL) {
                        return PDU_COMPOSE_CONTENT_ERROR;
                    }

                    AppendOctet(field);
                    AppendEncodedString(temp);
                }
                break;

            case PduHeaders.FROM:
                // Value-Length (Address-present-token Encoded-string-value | Insert-address-token)
                AppendOctet(field);

                EncodedStringValue from = mPduHeader->GetEncodedStringValue(field);
                If ((from == NULL)
                        || TextUtils->IsEmpty(from->GetString())
                        || new String(from->GetTextString()).Equals(
                                PduHeaders.FROM_INSERT_ADDRESS_TOKEN_STR)) {
                    // Length of from = 1
                    Append(1);
                    // Insert-address-token = <Octet 129>
                    Append(PduHeaders.FROM_INSERT_ADDRESS_TOKEN);
                } else {
                    mStack->Newbuf();
                    PositionMarker fstart = mStack->Mark();

                    // Address-present-token = <Octet 128>
                    Append(PduHeaders.FROM_ADDRESS_PRESENT_TOKEN);

                    temp = AppendAddressType(from);
                    If (temp == NULL) {
                        return PDU_COMPOSE_CONTENT_ERROR;
                    }

                    AppendEncodedString(temp);

                    Int32 flen = fstart->GetLength();
                    mStack->Pop();
                    AppendValueLength(flen);
                    mStack->Copy();
                }
                break;

            case PduHeaders.READ_STATUS:
            case PduHeaders.STATUS:
            case PduHeaders.REPORT_ALLOWED:
            case PduHeaders.PRIORITY:
            case PduHeaders.DELIVERY_REPORT:
            case PduHeaders.READ_REPORT:
            case PduHeaders.RETRIEVE_STATUS:
                Int32 octet = mPduHeader->GetOctet(field);
                If (0 == octet) {
                    return PDU_COMPOSE_FIELD_NOT_SET;
                }

                AppendOctet(field);
                AppendOctet(octet);
                break;

            case PduHeaders.DATE:
                Int64 date = mPduHeader->GetLongInteger(field);
                If (-1 == date) {
                    return PDU_COMPOSE_FIELD_NOT_SET;
                }

                AppendOctet(field);
                AppendDateValue(date);
                break;

            case PduHeaders.SUBJECT:
            case PduHeaders.RETRIEVE_TEXT:
                EncodedStringValue enString =
                    mPduHeader->GetEncodedStringValue(field);
                If (NULL == enString) {
                    return PDU_COMPOSE_FIELD_NOT_SET;
                }

                AppendOctet(field);
                AppendEncodedString(enString);
                break;

            case PduHeaders.MESSAGE_CLASS:
                Byte[] messageClass = mPduHeader->GetTextString(field);
                If (NULL == messageClass) {
                    return PDU_COMPOSE_FIELD_NOT_SET;
                }

                AppendOctet(field);
                If (Arrays->Equals(messageClass,
                        PduHeaders.MESSAGE_CLASS_ADVERTISEMENT_STR->GetBytes())) {
                    AppendOctet(PduHeaders.MESSAGE_CLASS_ADVERTISEMENT);
                } else If (Arrays->Equals(messageClass,
                        PduHeaders.MESSAGE_CLASS_AUTO_STR->GetBytes())) {
                    AppendOctet(PduHeaders.MESSAGE_CLASS_AUTO);
                } else If (Arrays->Equals(messageClass,
                        PduHeaders.MESSAGE_CLASS_PERSONAL_STR->GetBytes())) {
                    AppendOctet(PduHeaders.MESSAGE_CLASS_PERSONAL);
                } else If (Arrays->Equals(messageClass,
                        PduHeaders.MESSAGE_CLASS_INFORMATIONAL_STR->GetBytes())) {
                    AppendOctet(PduHeaders.MESSAGE_CLASS_INFORMATIONAL);
                } else {
                    AppendTextString(messageClass);
                }
                break;

            case PduHeaders.EXPIRY:
                Int64 expiry = mPduHeader->GetLongInteger(field);
                If (-1 == expiry) {
                    return PDU_COMPOSE_FIELD_NOT_SET;
                }

                AppendOctet(field);

                mStack->Newbuf();
                PositionMarker expiryStart = mStack->Mark();

                Append(PduHeaders.VALUE_RELATIVE_TOKEN);
                AppendLongInteger(expiry);

                Int32 expiryLength = expiryStart->GetLength();
                mStack->Pop();
                AppendValueLength(expiryLength);
                mStack->Copy();
                break;

            default:
                return PDU_COMPOSE_FIELD_NOT_SUPPORTED;
        }

        return PDU_COMPOSE_SUCCESS;
    }

    /**
     * Make ReadRec.Ind.
     */
    private Int32 MakeReadRecInd() {
        If (mMessage == NULL) {
            mMessage = new ByteArrayOutputStream();
            mPosition = 0;
        }

        // X-Mms-Message-Type
        AppendOctet(PduHeaders.MESSAGE_TYPE);
        AppendOctet(PduHeaders.MESSAGE_TYPE_READ_REC_IND);

        // X-Mms-MMS-Version
        If (AppendHeader(PduHeaders.MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // Message-ID
        If (AppendHeader(PduHeaders.MESSAGE_ID) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // To
        If (AppendHeader(PduHeaders.TO) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // From
        If (AppendHeader(PduHeaders.FROM) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // Date Optional
        AppendHeader(PduHeaders.DATE);

        // X-Mms-Read-Status
        If (AppendHeader(PduHeaders.READ_STATUS) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // X-Mms-Applic-ID Optional(not support)
        // X-Mms-Reply-Applic-ID Optional(not support)
        // X-Mms-Aux-Applic-Info Optional(not support)

        return PDU_COMPOSE_SUCCESS;
    }

    /**
     * Make NotifyResp.Ind.
     */
    private Int32 MakeNotifyResp() {
        If (mMessage == NULL) {
            mMessage = new ByteArrayOutputStream();
            mPosition = 0;
        }

        //    X-Mms-Message-Type
        AppendOctet(PduHeaders.MESSAGE_TYPE);
        AppendOctet(PduHeaders.MESSAGE_TYPE_NOTIFYRESP_IND);

        // X-Mms-Transaction-ID
        If (AppendHeader(PduHeaders.TRANSACTION_ID) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // X-Mms-MMS-Version
        If (AppendHeader(PduHeaders.MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        //  X-Mms-Status
        If (AppendHeader(PduHeaders.STATUS) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // X-Mms-Report-Allowed Optional (not support)
        return PDU_COMPOSE_SUCCESS;
    }

    /**
     * Make Acknowledge.Ind.
     */
    private Int32 MakeAckInd() {
        If (mMessage == NULL) {
            mMessage = new ByteArrayOutputStream();
            mPosition = 0;
        }

        //    X-Mms-Message-Type
        AppendOctet(PduHeaders.MESSAGE_TYPE);
        AppendOctet(PduHeaders.MESSAGE_TYPE_ACKNOWLEDGE_IND);

        // X-Mms-Transaction-ID
        If (AppendHeader(PduHeaders.TRANSACTION_ID) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        //     X-Mms-MMS-Version
        If (AppendHeader(PduHeaders.MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // X-Mms-Report-Allowed Optional
        AppendHeader(PduHeaders.REPORT_ALLOWED);

        return PDU_COMPOSE_SUCCESS;
    }

    /**
     * Make Send.req.
     */
    private Int32 MakeSendRetrievePdu(Int32 type) {
        If (mMessage == NULL) {
            mMessage = new ByteArrayOutputStream();
            mPosition = 0;
        }

        // X-Mms-Message-Type
        AppendOctet(PduHeaders.MESSAGE_TYPE);
        AppendOctet(type);

        // X-Mms-Transaction-ID
        AppendOctet(PduHeaders.TRANSACTION_ID);

        Byte[] trid = mPduHeader->GetTextString(PduHeaders.TRANSACTION_ID);
        If (trid == NULL) {
            // Transaction-ID should be Set(by Transaction) before Make().
            throw new IllegalArgumentException("Transaction-ID is NULL.");
        }
        AppendTextString(trid);

        //  X-Mms-MMS-Version
        If (AppendHeader(PduHeaders.MMS_VERSION) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // Date Date-value Optional.
        AppendHeader(PduHeaders.DATE);

        // From
        If (AppendHeader(PduHeaders.FROM) != PDU_COMPOSE_SUCCESS) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        Boolean recipient = FALSE;

        // To
        If (AppendHeader(PduHeaders.TO) != PDU_COMPOSE_CONTENT_ERROR) {
            recipient = TRUE;
        }

        // Cc
        If (AppendHeader(PduHeaders.CC) != PDU_COMPOSE_CONTENT_ERROR) {
            recipient = TRUE;
        }

        // Bcc
        If (AppendHeader(PduHeaders.BCC) != PDU_COMPOSE_CONTENT_ERROR) {
            recipient = TRUE;
        }

        // Need at least one of "cc", "bcc" and "to".
        If (FALSE == recipient) {
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        // Subject Optional
        AppendHeader(PduHeaders.SUBJECT);

        // X-Mms-Message-Class Optional
        // Message-class-value = Class-identifier | Token-text
        AppendHeader(PduHeaders.MESSAGE_CLASS);

        // X-Mms-Expiry Optional
        AppendHeader(PduHeaders.EXPIRY);

        // X-Mms-Priority Optional
        AppendHeader(PduHeaders.PRIORITY);

        // X-Mms-Delivery-Report Optional
        AppendHeader(PduHeaders.DELIVERY_REPORT);

        // X-Mms-Read-Report Optional
        AppendHeader(PduHeaders.READ_REPORT);

        If (type == PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF) {
            // X-Mms-Retrieve-Status Optional
            AppendHeader(PduHeaders.RETRIEVE_STATUS);
            // X-Mms-Retrieve-Text Optional
            AppendHeader(PduHeaders.RETRIEVE_TEXT);
        }

        //    Content-Type
        AppendOctet(PduHeaders.CONTENT_TYPE);

        //  Message body
        return MakeMessageBody(type);
    }

    /**
     * Make message body.
     */
    private Int32 MakeMessageBody(Int32 type) {
        // 1. add body informations
        mStack->Newbuf();  // Switching buffer because we need to

        PositionMarker ctStart = mStack->Mark();

        // This contentTypeIdentifier should be used for type of attachment...
        String contentType = new String(mPduHeader->GetTextString(PduHeaders.CONTENT_TYPE));
        Integer contentTypeIdentifier = mContentTypeMap->Get(contentType);
        If (contentTypeIdentifier == NULL) {
            // content type is mandatory
            return PDU_COMPOSE_CONTENT_ERROR;
        }

        AppendShortInteger(contentTypeIdentifier->IntValue());

        // content-type parameter: start
        PduBody body;
        If (type == PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF) {
            body = ((RetrieveConf) mPdu).GetBody();
        } else {
            body = ((SendReq) mPdu).GetBody();
        }
        If (NULL == body || body->GetPartsNum() == 0) {
            // empty message
            AppendUintvarInteger(0);
            mStack->Pop();
            mStack->Copy();
            return PDU_COMPOSE_SUCCESS;
        }

        PduPart part;
        try {
            part = body->GetPart(0);

            Byte[] start = part->GetContentId();
            If (start != NULL) {
                AppendOctet(PduPart.P_DEP_START);
                If (('<' == start[0]) && ('>' == start[start.length - 1])) {
                    AppendTextString(start);
                } else {
                    AppendTextString("<" + new String(start) + ">");
                }
            }

            // content-type parameter: type
            AppendOctet(PduPart.P_CT_MR_TYPE);
            AppendTextString(part->GetContentType());
        }
        Catch (ArrayIndexOutOfBoundsException e){
            e->PrintStackTrace();
        }

        Int32 ctLength = ctStart->GetLength();
        mStack->Pop();
        AppendValueLength(ctLength);
        mStack->Copy();

        // 3. add content
        Int32 partNum = body->GetPartsNum();
        AppendUintvarInteger(partNum);
        For (Int32 i = 0; i < partNum; i++) {
            part = body->GetPart(i);
            mStack->Newbuf();  // Leaving space for header lengh and data length
            PositionMarker attachment = mStack->Mark();

            mStack->Newbuf();  // Leaving space for Content-Type length
            PositionMarker contentTypeBegin = mStack->Mark();

            Byte[] partContentType = part->GetContentType();

            If (partContentType == NULL) {
                // content type is mandatory
                return PDU_COMPOSE_CONTENT_ERROR;
            }

            // content-type value
            Integer partContentTypeIdentifier =
                mContentTypeMap->Get(new String(partContentType));
            If (partContentTypeIdentifier == NULL) {
                AppendTextString(partContentType);
            } else {
                AppendShortInteger(partContentTypeIdentifier->IntValue());
            }

            /* Content-type parameter : name.
             * The value of name, filename, content-location is the same.
             * Just one of them is enough for this PDU.
             */
            Byte[] name = part->GetName();

            If (NULL == name) {
                name = part->GetFilename();

                If (NULL == name) {
                    name = part->GetContentLocation();

                    If (NULL == name) {
                        name = part->GetContentId();

                        If (NULL == name) {
                            // At lease one of name, filename, Content-location, Content id
                            // should be available.
                            return PDU_COMPOSE_CONTENT_ERROR;
                        }
                    }
                }
            }
            AppendOctet(PduPart.P_DEP_NAME);
            AppendTextString(name);

            // content-type parameter : charset
            Int32 charset = part->GetCharset();
            If (charset != 0) {
                AppendOctet(PduPart.P_CHARSET);
                AppendShortInteger(charset);
            }

            Int32 contentTypeLength = contentTypeBegin->GetLength();
            mStack->Pop();
            AppendValueLength(contentTypeLength);
            mStack->Copy();

            // content id
            Byte[] contentId = part->GetContentId();

            If (NULL != contentId) {
                AppendOctet(PduPart.P_CONTENT_ID);
                If (('<' == contentId[0]) && ('>' == contentId[contentId.length - 1])) {
                    AppendQuotedString(contentId);
                } else {
                    AppendQuotedString("<" + new String(contentId) + ">");
                }
            }

            // content-location
            Byte[] contentLocation = part->GetContentLocation();
            If (NULL != contentLocation) {
            	AppendOctet(PduPart.P_CONTENT_LOCATION);
            	AppendTextString(contentLocation);
            }

            // content
            Int32 headerLength = attachment->GetLength();

            Int32 dataLength = 0; // Just for safety...
            Byte[] partData = part->GetData();

            If (partData != NULL) {
                Arraycopy(partData, 0, partData.length);
                dataLength = partData.length;
            } else {
                InputStream cr = NULL;
                try {
                    Byte[] buffer = new Byte[PDU_COMPOSER_BLOCK_SIZE];
                    cr = mResolver->OpenInputStream(part->GetDataUri());
                    Int32 len = 0;
                    While ((len = cr->Read(buffer)) != -1) {
                        mMessage->Write(buffer, 0, len);
                        mPosition += len;
                        dataLength += len;
                    }
                } Catch (FileNotFoundException e) {
                    return PDU_COMPOSE_CONTENT_ERROR;
                } Catch (IOException e) {
                    return PDU_COMPOSE_CONTENT_ERROR;
                } Catch (RuntimeException e) {
                    return PDU_COMPOSE_CONTENT_ERROR;
                } finally {
                    If (cr != NULL) {
                        try {
                            cr->Close();
                        } Catch (IOException e) {
                        }
                    }
                }
            }

            If (dataLength != (attachment->GetLength() - headerLength)) {
                throw new RuntimeException("BUG: Length sanity check failed");
            }

            mStack->Pop();
            AppendUintvarInteger(headerLength);
            AppendUintvarInteger(dataLength);
            mStack->Copy();
        }

        return PDU_COMPOSE_SUCCESS;
    }

    /**
     *  Record current message informations.
     */
    static private class LengthRecordNode {
        ByteArrayOutputStream currentMessage = NULL;
        public Int32 currentPosition = 0;

        public LengthRecordNode next = NULL;
    }

    /**
     * Mark current message position and stact size.
     */
    private class PositionMarker {
        private Int32 c_pos;   // Current position
        private Int32 currentStackSize;  // Current stack size

        Int32 GetLength() {
            // If these assert fails, likely that you are finding the
            // size of buffer that is deep in BufferStack you can only
            // find the length of the buffer that is on top
            If (currentStackSize != mStack.stackSize) {
                throw new RuntimeException("BUG: Invalid call to GetLength()");
            }

            return mPosition - c_pos;
        }
    }

    /**
     * This implementation can be OPTIMIZED to use only
     * 2 buffers. This optimization involves changing BufferStack
     * only... Its Usage (interface) will not change.
     */
    private class BufferStack {
        private LengthRecordNode stack = NULL;
        private LengthRecordNode toCopy = NULL;

        Int32 stackSize = 0;

        /**
         *  Create a new message buffer and push it into the stack.
         */
        void Newbuf() {
            // You can't create a new buff when toCopy != NULL
            // That is after calling Pop() and before calling Copy()
            // If you do, it is a bug
            If (toCopy != NULL) {
                throw new RuntimeException("BUG: Invalid Newbuf() before Copy()");
            }

            LengthRecordNode temp = new LengthRecordNode();

            temp.currentMessage = mMessage;
            temp.currentPosition = mPosition;

            temp.next = stack;
            stack = temp;

            stackSize = stackSize + 1;

            mMessage = new ByteArrayOutputStream();
            mPosition = 0;
        }

        /**
         *  Pop the message before and record current message in the stack.
         */
        void Pop() {
            ByteArrayOutputStream currentMessage = mMessage;
            Int32 currentPosition = mPosition;

            mMessage = stack.currentMessage;
            mPosition = stack.currentPosition;

            toCopy = stack;
            // Re using the top element of the stack to avoid memory allocation

            stack = stack.next;
            stackSize = stackSize - 1;

            toCopy.currentMessage = currentMessage;
            toCopy.currentPosition = currentPosition;
        }

        /**
         *  Append current message to the message before.
         */
        void Copy() {
            Arraycopy(toCopy.currentMessage->ToByteArray(), 0,
                    toCopy.currentPosition);

            toCopy = NULL;
        }

        /**
         *  Mark current message position
         */
        PositionMarker Mark() {
            PositionMarker m = new PositionMarker();

            m.c_pos = mPosition;
            m.currentStackSize = stackSize;

            return m;
        }
    }

    /**
     * Check address type.
     *
     * @param address address string without the postfix stinng type,
     *        such as "/TYPE=PLMN", "/TYPE=IPv6" and "/TYPE=IPv4"
     * @return PDU_PHONE_NUMBER_ADDRESS_TYPE if it is phone number,
     *         PDU_EMAIL_ADDRESS_TYPE if it is email address,
     *         PDU_IPV4_ADDRESS_TYPE if it is ipv4 address,
     *         PDU_IPV6_ADDRESS_TYPE if it is ipv6 address,
     *         PDU_UNKNOWN_ADDRESS_TYPE if it is unknown.
     */
    protected static Int32 CheckAddressType(String address) {
        /**
         * From OMA-TS-MMS-ENC-V1_3-20050927-C.pdf, section 8.
         * address = ( e-mail / device-address / alphanum-shortcode / num-shortcode)
         * e-mail = mailbox; to the definition of mailbox as described in
         * section 3.4 of [RFC2822], but excluding the
         * obsolete definitions as indicated by the "obs-" prefix.
         * device-address = ( global-phone-number "/TYPE=PLMN" )
         * / ( ipv4 "/TYPE=IPv4" ) / ( ipv6 "/TYPE=IPv6" )
         * / ( escaped-value "/TYPE=" address-type )
         *
         * global-phone-number = ["+"] 1*( DIGIT / written-sep )
         * written-sep =("-"/".")
         *
         * ipv4 = 1*3DIGIT 3( "." 1*3DIGIT ) ; IPv4 address value
         *
         * ipv6 = 4HEXDIG 7( ":" 4HEXDIG ) ; IPv6 address per RFC 2373
         */

        If (NULL == address) {
            return PDU_UNKNOWN_ADDRESS_TYPE;
        }

        If (address->Matches(REGEXP_IPV4_ADDRESS_TYPE)) {
            // Ipv4 address.
            return PDU_IPV4_ADDRESS_TYPE;
        }else If (address->Matches(REGEXP_PHONE_NUMBER_ADDRESS_TYPE)) {
            // Phone number.
            return PDU_PHONE_NUMBER_ADDRESS_TYPE;
        } else If (address->Matches(REGEXP_EMAIL_ADDRESS_TYPE)) {
            // Email address.
            return PDU_EMAIL_ADDRESS_TYPE;
        } else If (address->Matches(REGEXP_IPV6_ADDRESS_TYPE)) {
            // Ipv6 address.
            return PDU_IPV6_ADDRESS_TYPE;
        } else {
            // Unknown address.
            return PDU_UNKNOWN_ADDRESS_TYPE;
        }
    }
}
