/*
 * Copyright (C) 2012 The Android Open Source Project
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

using Elastos::Droid::Telephony::ISmsCbLocation;
using Elastos::Droid::Telephony::ISmsCbMessage;
using Elastos::Droid::Utility::IPair;

using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Droid::Internal::Telephony::ISmsConstants;


/**
 * Parses a GSM or UMTS format SMS-CB message into an {@link SmsCbMessage} object. The class is
 * public because {@link #CreateSmsCbMessage(SmsCbLocation, Byte[][])} is used by some test cases.
 */
public class GsmSmsCbMessage {

    /**
     * Languages in the 0000xxxx DCS group as defined in 3GPP TS 23.038, section 5.
     */
    private static const String[] LANGUAGE_CODES_GROUP_0 = {
            "de", "en", "it", "fr", "es", "nl", "sv", "da", "pt", "fi", "no", "el", "tr", "hu",
            "pl", NULL
    };

    /**
     * Languages in the 0010xxxx DCS group as defined in 3GPP TS 23.038, section 5.
     */
    private static const String[] LANGUAGE_CODES_GROUP_2 = {
            "cs", "he", "ar", "ru", "is", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
            NULL, NULL
    };

    private static const Char32 CARRIAGE_RETURN = 0x0d;

    private static const Int32 PDU_BODY_PAGE_LENGTH = 82;

    /** Utility class with only static methods. */
    private GsmSmsCbMessage() { }

    /**
     * Create a new SmsCbMessage object from a header object plus one or more received PDUs.
     *
     * @param pdus PDU bytes
     */
    static SmsCbMessage CreateSmsCbMessage(SmsCbHeader header, SmsCbLocation location,
            Byte[][] pdus) throws IllegalArgumentException {
        If (header->IsEtwsPrimaryNotification()) {
            return new SmsCbMessage(SmsCbMessage.MESSAGE_FORMAT_3GPP,
                    header->GetGeographicalScope(), header->GetSerialNumber(),
                    location, header->GetServiceCategory(),
                    NULL, "ETWS", SmsCbMessage.MESSAGE_PRIORITY_EMERGENCY,
                    header->GetEtwsInfo(), header->GetCmasInfo());
        } else {
            String language = NULL;
            StringBuilder sb = new StringBuilder();
            For (Byte[] pdu : pdus) {
                Pair<String, String> p = ParseBody(header, pdu);
                language = p.first;
                sb->Append(p.second);
            }
            Int32 priority = header->IsEmergencyMessage() ? SmsCbMessage.MESSAGE_PRIORITY_EMERGENCY
                    : SmsCbMessage.MESSAGE_PRIORITY_NORMAL;

            return new SmsCbMessage(SmsCbMessage.MESSAGE_FORMAT_3GPP,
                    header->GetGeographicalScope(), header->GetSerialNumber(), location,
                    header->GetServiceCategory(), language, sb->ToString(), priority,
                    header->GetEtwsInfo(), header->GetCmasInfo());
        }
    }

    /**
     * Create a new SmsCbMessage object from one or more received PDUs. This is used by some
     * CellBroadcastReceiver test cases, because SmsCbHeader is now package local.
     *
     * @param location the Location (geographical scope) for the message
     * @param pdus PDU bytes
     */
    public static SmsCbMessage CreateSmsCbMessage(SmsCbLocation location, Byte[][] pdus)
            throws IllegalArgumentException {
        SmsCbHeader header = new SmsCbHeader(pdus[0]);
        return CreateSmsCbMessage(header, location, pdus);
    }

    /**
     * Parse and unpack the body text according to the encoding in the DCS.
     * After completing successfully this method will have assigned the body
     * text into mBody, and optionally the language code into mLanguage
     *
     * @param header the message header to use
     * @param pdu the PDU to decode
     * @return a Pair of Strings containing the language and body of the message
     */
    private static Pair<String, String> ParseBody(SmsCbHeader header, Byte[] pdu) {
        Int32 encoding;
        String language = NULL;
        Boolean hasLanguageIndicator = FALSE;
        Int32 dataCodingScheme = header->GetDataCodingScheme();

        // Extract encoding and language from DCS, as defined in 3gpp TS 23.038,
        // section 5.
        Switch ((dataCodingScheme & 0xf0) >> 4) {
            case 0x00:
                encoding = SmsConstants.ENCODING_7BIT;
                language = LANGUAGE_CODES_GROUP_0[dataCodingScheme & 0x0f];
                break;

            case 0x01:
                hasLanguageIndicator = TRUE;
                If ((dataCodingScheme & 0x0f) == 0x01) {
                    encoding = SmsConstants.ENCODING_16BIT;
                } else {
                    encoding = SmsConstants.ENCODING_7BIT;
                }
                break;

            case 0x02:
                encoding = SmsConstants.ENCODING_7BIT;
                language = LANGUAGE_CODES_GROUP_2[dataCodingScheme & 0x0f];
                break;

            case 0x03:
                encoding = SmsConstants.ENCODING_7BIT;
                break;

            case 0x04:
            case 0x05:
                Switch ((dataCodingScheme & 0x0c) >> 2) {
                    case 0x01:
                        encoding = SmsConstants.ENCODING_8BIT;
                        break;

                    case 0x02:
                        encoding = SmsConstants.ENCODING_16BIT;
                        break;

                    case 0x00:
                    default:
                        encoding = SmsConstants.ENCODING_7BIT;
                        break;
                }
                break;

            case 0x06:
            case 0x07:
                // Compression not supported
            case 0x09:
                // UDH structure not supported
            case 0x0e:
                // Defined by the WAP forum not supported
                throw new IllegalArgumentException("Unsupported GSM dataCodingScheme "
                        + dataCodingScheme);

            case 0x0f:
                If (((dataCodingScheme & 0x04) >> 2) == 0x01) {
                    encoding = SmsConstants.ENCODING_8BIT;
                } else {
                    encoding = SmsConstants.ENCODING_7BIT;
                }
                break;

            default:
                // Reserved values are to be treated as 7-bit
                encoding = SmsConstants.ENCODING_7BIT;
                break;
        }

        If (header->IsUmtsFormat()) {
            // Payload may contain multiple pages
            Int32 nrPages = pdu[SmsCbHeader.PDU_HEADER_LENGTH];

            If (pdu.length < SmsCbHeader.PDU_HEADER_LENGTH + 1 + (PDU_BODY_PAGE_LENGTH + 1)
                    * nrPages) {
                throw new IllegalArgumentException("Pdu length " + pdu.length + " does not match "
                        + nrPages + " pages");
            }

            StringBuilder sb = new StringBuilder();

            For (Int32 i = 0; i < nrPages; i++) {
                // Each page is 82 bytes followed by a length octet indicating
                // the number of useful octets within those 82
                Int32 offset = SmsCbHeader.PDU_HEADER_LENGTH + 1 + (PDU_BODY_PAGE_LENGTH + 1) * i;
                Int32 length = pdu[offset + PDU_BODY_PAGE_LENGTH];

                If (length > PDU_BODY_PAGE_LENGTH) {
                    throw new IllegalArgumentException("Page length " + length
                            + " exceeds maximum value " + PDU_BODY_PAGE_LENGTH);
                }

                Pair<String, String> p = UnpackBody(pdu, encoding, offset, length,
                        hasLanguageIndicator, language);
                language = p.first;
                sb->Append(p.second);
            }
            return new Pair<String, String>(language, sb->ToString());
        } else {
            // Payload is one single page
            Int32 offset = SmsCbHeader.PDU_HEADER_LENGTH;
            Int32 length = pdu.length - offset;

            return UnpackBody(pdu, encoding, offset, length, hasLanguageIndicator, language);
        }
    }

    /**
     * Unpack body text from the pdu using the given encoding, position and
     * length within the pdu
     *
     * @param pdu The pdu
     * @param encoding The encoding, as derived from the DCS
     * @param offset Position of the first Byte to unpack
     * @param length Number of bytes to unpack
     * @param hasLanguageIndicator TRUE if the body text is preceded by a
     *            language indicator. If so, this method will as a side-effect
     *            assign the extracted language code into mLanguage
     * @param language the language to return if hasLanguageIndicator is FALSE
     * @return a Pair of Strings containing the language and body of the message
     */
    private static Pair<String, String> UnpackBody(Byte[] pdu, Int32 encoding, Int32 offset, Int32 length,
            Boolean hasLanguageIndicator, String language) {
        String body = NULL;

        Switch (encoding) {
            case SmsConstants.ENCODING_7BIT:
                body = GsmAlphabet->Gsm7BitPackedToString(pdu, offset, length * 8 / 7);

                If (hasLanguageIndicator && body != NULL && body->Length() > 2) {
                    // Language is two GSM characters followed by a CR.
                    // The actual body text is offset by 3 characters.
                    language = body->Substring(0, 2);
                    body = body->Substring(3);
                }
                break;

            case SmsConstants.ENCODING_16BIT:
                If (hasLanguageIndicator && pdu.length >= offset + 2) {
                    // Language is two GSM characters.
                    // The actual body text is offset by 2 bytes.
                    language = GsmAlphabet->Gsm7BitPackedToString(pdu, offset, 2);
                    offset += 2;
                    length -= 2;
                }

                try {
                    body = new String(pdu, offset, (length & 0xfffe), "utf-16");
                } Catch (UnsupportedEncodingException e) {
                    // Apparently it wasn't valid UTF-16.
                    throw new IllegalArgumentException("Error decoding UTF-16 message", e);
                }
                break;

            default:
                break;
        }

        If (body != NULL) {
            // Remove trailing carriage return
            For (Int32 i = body->Length() - 1; i >= 0; i--) {
                If (body->CharAt(i) != CARRIAGE_RETURN) {
                    body = body->Substring(0, i + 1);
                    break;
                }
            }
        } else {
            body = "";
        }

        return new Pair<String, String>(language, body);
    }
}
