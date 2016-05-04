/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony;

using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Utility::IHexDump;

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;

using Elastos::Utility::IArrayList;

/**
 * SMS user data header, as specified in TS 23.040 9.2.3.24.
 */
public class SmsHeader {

    // TODO(cleanup): this data structure is generally referred to as
    // the 'user data header' or UDH, and so the class name should
    // change to reflect this...

    /** SMS user data header information element identifiers.
     * (see TS 23.040 9.2.3.24)
     */
    public static const Int32 ELT_ID_CONCATENATED_8_BIT_REFERENCE       = 0x00;
    public static const Int32 ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION     = 0x01;
    public static const Int32 ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT  = 0x04;
    public static const Int32 ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT = 0x05;
    public static const Int32 ELT_ID_SMSC_CONTROL_PARAMS                = 0x06;
    public static const Int32 ELT_ID_UDH_SOURCE_INDICATION              = 0x07;
    public static const Int32 ELT_ID_CONCATENATED_16_BIT_REFERENCE      = 0x08;
    public static const Int32 ELT_ID_WIRELESS_CTRL_MSG_PROTOCOL         = 0x09;
    public static const Int32 ELT_ID_TEXT_FORMATTING                    = 0x0A;
    public static const Int32 ELT_ID_PREDEFINED_SOUND                   = 0x0B;
    public static const Int32 ELT_ID_USER_DEFINED_SOUND                 = 0x0C;
    public static const Int32 ELT_ID_PREDEFINED_ANIMATION               = 0x0D;
    public static const Int32 ELT_ID_LARGE_ANIMATION                    = 0x0E;
    public static const Int32 ELT_ID_SMALL_ANIMATION                    = 0x0F;
    public static const Int32 ELT_ID_LARGE_PICTURE                      = 0x10;
    public static const Int32 ELT_ID_SMALL_PICTURE                      = 0x11;
    public static const Int32 ELT_ID_VARIABLE_PICTURE                   = 0x12;
    public static const Int32 ELT_ID_USER_PROMPT_INDICATOR              = 0x13;
    public static const Int32 ELT_ID_EXTENDED_OBJECT                    = 0x14;
    public static const Int32 ELT_ID_REUSED_EXTENDED_OBJECT             = 0x15;
    public static const Int32 ELT_ID_COMPRESSION_CONTROL                = 0x16;
    public static const Int32 ELT_ID_OBJECT_DISTR_INDICATOR             = 0x17;
    public static const Int32 ELT_ID_STANDARD_WVG_OBJECT                = 0x18;
    public static const Int32 ELT_ID_CHARACTER_SIZE_WVG_OBJECT          = 0x19;
    public static const Int32 ELT_ID_EXTENDED_OBJECT_DATA_REQUEST_CMD   = 0x1A;
    public static const Int32 ELT_ID_RFC_822_EMAIL_HEADER               = 0x20;
    public static const Int32 ELT_ID_HYPERLINK_FORMAT_ELEMENT           = 0x21;
    public static const Int32 ELT_ID_REPLY_ADDRESS_ELEMENT              = 0x22;
    public static const Int32 ELT_ID_ENHANCED_VOICE_MAIL_INFORMATION    = 0x23;
    public static const Int32 ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHIFT     = 0x24;
    public static const Int32 ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHIFT    = 0x25;

    public static const Int32 PORT_WAP_PUSH = 2948;
    public static const Int32 PORT_WAP_WSP  = 9200;

    public static class PortAddrs {
        public Int32 destPort;
        public Int32 origPort;
        public Boolean areEightBits;
    }

    public static class ConcatRef {
        public Int32 refNumber;
        public Int32 seqNumber;
        public Int32 msgCount;
        public Boolean isEightBits;
    }

    public static class SpecialSmsMsg {
        public Int32 msgIndType;
        public Int32 msgCount;
    }

    /**
     * A header element that is not explicitly parsed, meaning not
     * PortAddrs or ConcatRef or SpecialSmsMsg.
     */
    public static class MiscElt {
        public Int32 id;
        public Byte[] data;
    }

    public PortAddrs portAddrs;
    public ConcatRef concatRef;
    public ArrayList<SpecialSmsMsg> specialSmsMsgList = new ArrayList<SpecialSmsMsg>();
    public ArrayList<MiscElt> miscEltList = new ArrayList<MiscElt>();

    /** 7 bit national language locking shift table, or 0 for GSM default 7 bit alphabet. */
    public Int32 languageTable;

    /** 7 bit national language single shift table, or 0 for GSM default 7 bit extension table. */
    public Int32 languageShiftTable;

    public SmsHeader() {}

    /**
     * Create structured SmsHeader object from serialized Byte array representation.
     * (see TS 23.040 9.2.3.24)
     * @param data is user data header bytes
     * @return SmsHeader object
     */
    public static SmsHeader FromByteArray(Byte[] data) {
        ByteArrayInputStream inStream = new ByteArrayInputStream(data);
        SmsHeader smsHeader = new SmsHeader();
        While (inStream->Available() > 0) {
            /**
             * NOTE: as defined in the spec, ConcatRef and PortAddr
             * fields should not reoccur, but if they do the last
             * occurrence is to be used.  Also, for ConcatRef
             * elements, if the count is zero, sequence is zero, or
             * sequence is larger than count, the entire element is to
             * be ignored.
             */
            Int32 id = inStream->Read();
            Int32 length = inStream->Read();
            ConcatRef concatRef;
            PortAddrs portAddrs;
            Switch (id) {
            case ELT_ID_CONCATENATED_8_BIT_REFERENCE:
                concatRef = new ConcatRef();
                concatRef.refNumber = inStream->Read();
                concatRef.msgCount = inStream->Read();
                concatRef.seqNumber = inStream->Read();
                concatRef.isEightBits = TRUE;
                If (concatRef.msgCount != 0 && concatRef.seqNumber != 0 &&
                        concatRef.seqNumber <= concatRef.msgCount) {
                    smsHeader.concatRef = concatRef;
                }
                break;
            case ELT_ID_CONCATENATED_16_BIT_REFERENCE:
                concatRef = new ConcatRef();
                concatRef.refNumber = (inStream->Read() << 8) | inStream->Read();
                concatRef.msgCount = inStream->Read();
                concatRef.seqNumber = inStream->Read();
                concatRef.isEightBits = FALSE;
                If (concatRef.msgCount != 0 && concatRef.seqNumber != 0 &&
                        concatRef.seqNumber <= concatRef.msgCount) {
                    smsHeader.concatRef = concatRef;
                }
                break;
            case ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT:
                portAddrs = new PortAddrs();
                portAddrs.destPort = inStream->Read();
                portAddrs.origPort = inStream->Read();
                portAddrs.areEightBits = TRUE;
                smsHeader.portAddrs = portAddrs;
                break;
            case ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT:
                portAddrs = new PortAddrs();
                portAddrs.destPort = (inStream->Read() << 8) | inStream->Read();
                portAddrs.origPort = (inStream->Read() << 8) | inStream->Read();
                portAddrs.areEightBits = FALSE;
                smsHeader.portAddrs = portAddrs;
                break;
            case ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHIFT:
                smsHeader.languageShiftTable = inStream->Read();
                break;
            case ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHIFT:
                smsHeader.languageTable = inStream->Read();
                break;
            case ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION:
                SpecialSmsMsg specialSmsMsg = new SpecialSmsMsg();
                specialSmsMsg.msgIndType = inStream->Read();
                specialSmsMsg.msgCount = inStream->Read();
                smsHeader.specialSmsMsgList->Add(specialSmsMsg);
                break;
            default:
                MiscElt miscElt = new MiscElt();
                miscElt.id = id;
                miscElt.data = new Byte[length];
                inStream->Read(miscElt.data, 0, length);
                smsHeader.miscEltList->Add(miscElt);
            }
        }
        return smsHeader;
    }

    /**
     * Create serialized Byte array representation from structured SmsHeader object.
     * (see TS 23.040 9.2.3.24)
     * @return Byte array representing the SmsHeader
     */
    public static Byte[] ToByteArray(SmsHeader smsHeader) {
        If ((smsHeader.portAddrs == NULL) &&
            (smsHeader.concatRef == NULL) &&
            (smsHeader.specialSmsMsgList->IsEmpty()) &&
            (smsHeader.miscEltList->IsEmpty()) &&
            (smsHeader.languageShiftTable == 0) &&
            (smsHeader.languageTable == 0)) {
            return NULL;
        }

        ByteArrayOutputStream outStream =
                new ByteArrayOutputStream(SmsConstants.MAX_USER_DATA_BYTES);
        ConcatRef concatRef = smsHeader.concatRef;
        If (concatRef != NULL) {
            If (concatRef.isEightBits) {
                outStream->Write(ELT_ID_CONCATENATED_8_BIT_REFERENCE);
                outStream->Write(3);
                outStream->Write(concatRef.refNumber);
            } else {
                outStream->Write(ELT_ID_CONCATENATED_16_BIT_REFERENCE);
                outStream->Write(4);
                outStream->Write(concatRef.refNumber >>> 8);
                outStream->Write(concatRef.refNumber & 0x00FF);
            }
            outStream->Write(concatRef.msgCount);
            outStream->Write(concatRef.seqNumber);
        }
        PortAddrs portAddrs = smsHeader.portAddrs;
        If (portAddrs != NULL) {
            If (portAddrs.areEightBits) {
                outStream->Write(ELT_ID_APPLICATION_PORT_ADDRESSING_8_BIT);
                outStream->Write(2);
                outStream->Write(portAddrs.destPort);
                outStream->Write(portAddrs.origPort);
            } else {
                outStream->Write(ELT_ID_APPLICATION_PORT_ADDRESSING_16_BIT);
                outStream->Write(4);
                outStream->Write(portAddrs.destPort >>> 8);
                outStream->Write(portAddrs.destPort & 0x00FF);
                outStream->Write(portAddrs.origPort >>> 8);
                outStream->Write(portAddrs.origPort & 0x00FF);
            }
        }
        If (smsHeader.languageShiftTable != 0) {
            outStream->Write(ELT_ID_NATIONAL_LANGUAGE_SINGLE_SHIFT);
            outStream->Write(1);
            outStream->Write(smsHeader.languageShiftTable);
        }
        If (smsHeader.languageTable != 0) {
            outStream->Write(ELT_ID_NATIONAL_LANGUAGE_LOCKING_SHIFT);
            outStream->Write(1);
            outStream->Write(smsHeader.languageTable);
        }
        For (SpecialSmsMsg specialSmsMsg : smsHeader.specialSmsMsgList) {
            outStream->Write(ELT_ID_SPECIAL_SMS_MESSAGE_INDICATION);
            outStream->Write(2);
            outStream->Write(specialSmsMsg.msgIndType & 0xFF);
            outStream->Write(specialSmsMsg.msgCount & 0xFF);
        }
        For (MiscElt miscElt : smsHeader.miscEltList) {
            outStream->Write(miscElt.id);
            outStream->Write(miscElt.data.length);
            outStream->Write(miscElt.data, 0, miscElt.data.length);
        }
        return outStream->ToByteArray();
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder builder = new StringBuilder();
        builder->Append("UserDataHeader ");
        builder->Append("{ ConcatRef ");
        If (concatRef == NULL) {
            builder->Append("unset");
        } else {
            builder->Append("{ refNumber=" + concatRef.refNumber);
            builder->Append(", msgCount=" + concatRef.msgCount);
            builder->Append(", seqNumber=" + concatRef.seqNumber);
            builder->Append(", isEightBits=" + concatRef.isEightBits);
            builder->Append(" }");
        }
        builder->Append(", PortAddrs ");
        If (portAddrs == NULL) {
            builder->Append("unset");
        } else {
            builder->Append("{ destPort=" + portAddrs.destPort);
            builder->Append(", origPort=" + portAddrs.origPort);
            builder->Append(", areEightBits=" + portAddrs.areEightBits);
            builder->Append(" }");
        }
        If (languageShiftTable != 0) {
            builder->Append(", languageShiftTable=" + languageShiftTable);
        }
        If (languageTable != 0) {
            builder->Append(", languageTable=" + languageTable);
        }
        For (SpecialSmsMsg specialSmsMsg : specialSmsMsgList) {
            builder->Append(", SpecialSmsMsg ");
            builder->Append("{ msgIndType=" + specialSmsMsg.msgIndType);
            builder->Append(", msgCount=" + specialSmsMsg.msgCount);
            builder->Append(" }");
        }
        For (MiscElt miscElt : miscEltList) {
            builder->Append(", MiscElt ");
            builder->Append("{ id=" + miscElt.id);
            builder->Append(", length=" + miscElt.data.length);
            builder->Append(", data=" + HexDump->ToHexString(miscElt.data));
            builder->Append(" }");
        }
        builder->Append(" }");
        return builder->ToString();
    }

}
