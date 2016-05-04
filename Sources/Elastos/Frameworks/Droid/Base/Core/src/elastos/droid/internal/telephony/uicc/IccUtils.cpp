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

package com.android.internal.telephony.uicc;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Telephony::IRlog;


using Elastos::Droid::Internal::Telephony::IGsmAlphabet;
using Elastos::Nio::Charset::ICharset;

/**
 * Various methods, useful for dealing with SIM data.
 */
public class IccUtils {
    static const String LOG_TAG="IccUtils";

    /**
     * Many fields in GSM SIM's are stored as nibble-swizzled BCD
     *
     * Assumes left-justified field that may be padded right with 0xf
     * values.
     *
     * Stops on invalid BCD value, returning string so far
     */
    public static String
    BcdToString(Byte[] data, Int32 offset, Int32 length) {
        StringBuilder ret = new StringBuilder(length*2);

        For (Int32 i = offset ; i < offset + length ; i++) {
            Byte b;
            Int32 v;

            v = data[i] & 0xf;
            If (v > 9)  break;
            ret->Append((Char32)('0' + v));

            v = (data[i] >> 4) & 0xf;
            // Some PLMNs have 'f' as high nibble, ignore it
            If (v == 0xf) continue;
            If (v > 9)  break;
            ret->Append((Char32)('0' + v));
        }

        return ret->ToString();
    }

    /**
     * Decode cdma Byte into String.
     */
    public static String
    CdmaBcdToString(Byte[] data, Int32 offset, Int32 length) {
        StringBuilder ret = new StringBuilder(length);

        Int32 count = 0;
        For (Int32 i = offset; count < length; i++) {
            Int32 v;
            v = data[i] & 0xf;
            If (v > 9)  v = 0;
            ret->Append((Char32)('0' + v));

            If (++count == length) break;

            v = (data[i] >> 4) & 0xf;
            If (v > 9)  v = 0;
            ret->Append((Char32)('0' + v));
            ++count;
        }
        return ret->ToString();
    }

    /**
     * Decodes a GSM-style BCD Byte, returning an Int32 ranging from 0-99.
     *
     * In GSM land, the least significant BCD digit is stored in the most
     * significant nibble.
     *
     * Out-of-range digits are treated as 0 for the sake of the time stamp,
     * because of this:
     *
     * TS 23.040 section 9.2.3.11
     * "if the MS receives a non-integer value in the SCTS, it shall
     * assume the digit is set to 0 but shall store the entire field
     * exactly as received"
     */
    public static Int32
    GsmBcdByteToInt(Byte b) {
        Int32 ret = 0;

        // treat out-of-range BCD values as 0
        If ((b & 0xf0) <= 0x90) {
            ret = (b >> 4) & 0xf;
        }

        If ((b & 0x0f) <= 0x09) {
            ret +=  (b & 0xf) * 10;
        }

        return ret;
    }

    /**
     * Decodes a CDMA style BCD Byte like {@link #gsmBcdByteToInt}, but
     * opposite nibble format. The least significant BCD digit
     * is in the least significant nibble and the most significant
     * is in the most significant nibble.
     */
    public static Int32
    CdmaBcdByteToInt(Byte b) {
        Int32 ret = 0;

        // treat out-of-range BCD values as 0
        If ((b & 0xf0) <= 0x90) {
            ret = ((b >> 4) & 0xf) * 10;
        }

        If ((b & 0x0f) <= 0x09) {
            ret +=  (b & 0xf);
        }

        return ret;
    }

    /**
     * Decodes a string field that's formatted like the EF[ADN] alpha
     * identifier
     *
     * From TS 51.011 10.5.1:
     *   Coding:
     *       this alpha tagging shall use either
     *      -    the SMS default 7 bit coded alphabet as defined in
     *          TS 23.038 [12] with bit 8 set to 0. The alpha identifier
     *          shall be left justified. Unused bytes shall be set to 'FF'; or
     *      -    one of the UCS2 coded options as defined in annex B.
     *
     * Annex B from TS 11.11 V8.13.0:
     *      1)  If the first octet in the alpha string is '80', then the
     *          remaining octets are 16 bit UCS2 characters ...
     *      2)  if the first octet in the alpha string is '81', then the
     *          second octet contains a value indicating the number of
     *          characters in the string, and the third octet contains an
     *          8 bit number which defines bits 15 to 8 of a 16 bit
     *          base pointer, where bit 16 is set to zero and bits 7 to 1
     *          are also set to zero.  These sixteen bits constitute a
     *          base pointer to a "half page" in the UCS2 code space, to be
     *          used with some or all of the remaining octets in the string.
     *          The fourth and subsequent octets contain codings as follows:
     *          If bit 8 of the octet is set to zero, the remaining 7 bits
     *          of the octet contain a GSM Default Alphabet character,
     *          whereas if bit 8 of the octet is set to one, then the
     *          remaining seven bits are an offset value added to the
     *          16 bit base pointer defined earlier...
     *      3)  If the first octet of the alpha string is set to '82', then
     *          the second octet contains a value indicating the number of
     *          characters in the string, and the third and fourth octets
     *          contain a 16 bit number which defines the complete 16 bit
     *          base pointer to a "half page" in the UCS2 code space...
     */
    public static String
    AdnStringFieldToString(Byte[] data, Int32 offset, Int32 length) {
        If (length == 0) {
            return "";
        }
        If (length >= 1) {
            If (data[offset] == (Byte) 0x80) {
                Int32 ucslen = (length - 1) / 2;
                String ret = NULL;

                try {
                    ret = new String(data, offset + 1, ucslen * 2, "utf-16be");
                } Catch (UnsupportedEncodingException ex) {
                    Rlog->E(LOG_TAG, "implausible UnsupportedEncodingException",
                          ex);
                }

                If (ret != NULL) {
                    // trim off trailing FFFF characters

                    ucslen = ret->Length();
                    While (ucslen > 0 && ret->CharAt(ucslen - 1) == '\uFFFF')
                        ucslen--;

                    return ret->Substring(0, ucslen);
                }
            }
        }

        Boolean isucs2 = FALSE;
        Char32 base = '\0';
        Int32 len = 0;

        If (length >= 3 && data[offset] == (Byte) 0x81) {
            len = data[offset + 1] & 0xFF;
            If (len > length - 3)
                len = length - 3;

            base = (Char32) ((data[offset + 2] & 0xFF) << 7);
            offset += 3;
            isucs2 = TRUE;
        } else If (length >= 4 && data[offset] == (Byte) 0x82) {
            len = data[offset + 1] & 0xFF;
            If (len > length - 4)
                len = length - 4;

            base = (Char32) (((data[offset + 2] & 0xFF) << 8) |
                            (data[offset + 3] & 0xFF));
            offset += 4;
            isucs2 = TRUE;
        }

        If (isucs2) {
            StringBuilder ret = new StringBuilder();

            While (len > 0) {
                // UCS2 subset case

                If (data[offset] < 0) {
                    ret->Append((Char32) (base + (data[offset] & 0x7F)));
                    offset++;
                    len--;
                }

                // GSM character set case

                Int32 count = 0;
                While (count < len && data[offset + count] >= 0)
                    count++;

                ret->Append(GsmAlphabet->Gsm8BitUnpackedToString(data,
                           offset, count));

                offset += count;
                len -= count;
            }

            return ret->ToString();
        }

        Resources resource = Resources->GetSystem();
        String defaultCharset = "";
        try {
            defaultCharset =
                    resource->GetString(R::string::gsm_alphabet_default_charset);
        } Catch (NotFoundException e) {
            // Ignore Exception and defaultCharset is set to a empty string.
        }
        return GsmAlphabet->Gsm8BitUnpackedToString(data, offset, length, defaultCharset->Trim());
    }

    static Int32
    HexCharToInt(Char32 c) {
        If (c >= '0' && c <= '9') Return (c - '0');
        If (c >= 'A' && c <= 'F') Return (c - 'A' + 10);
        If (c >= 'a' && c <= 'f') Return (c - 'a' + 10);

        throw new RuntimeException ("invalid hex Char32 '" + c + "'");
    }

    /**
     * Converts a hex String to a Byte array.
     *
     * @param s A string of hexadecimal characters, must be an even number of
     *          chars Int64
     *
     * @return Byte array representation
     *
     * @throws RuntimeException on invalid format
     */
    public static Byte[]
    HexStringToBytes(String s) {
        Byte[] ret;

        If (s == NULL) return NULL;

        Int32 sz = s->Length();

        ret = new Byte[sz/2];

        For (Int32 i=0 ; i <sz ; i+=2) {
            ret[i/2] = (Byte) ((HexCharToInt(s->CharAt(i)) << 4)
                                | HexCharToInt(s->CharAt(i+1)));
        }

        return ret;
    }


    /**
     * Converts a Byte array into a String of hexadecimal characters.
     *
     * @param bytes an array of bytes
     *
     * @return hex string representation of bytes array
     */
    public static String
    BytesToHexString(Byte[] bytes) {
        If (bytes == NULL) return NULL;

        StringBuilder ret = new StringBuilder(2*bytes.length);

        For (Int32 i = 0 ; i < bytes.length ; i++) {
            Int32 b;

            b = 0x0f & (bytes[i] >> 4);

            ret->Append("0123456789abcdef".CharAt(b));

            b = 0x0f & bytes[i];

            ret->Append("0123456789abcdef".CharAt(b));
        }

        return ret->ToString();
    }


    /**
     * Convert a TS 24.008 Section 10.5.3.5a Network Name field to a string
     * "offset" points to "octet 3", the coding scheme Byte
     * empty string returned on decode error
     */
    public static String
    NetworkNameToString(Byte[] data, Int32 offset, Int32 length) {
        String ret;

        If ((data[offset] & 0x80) != 0x80 || length < 1) {
            return "";
        }

        Switch ((data[offset] >>> 4) & 0x7) {
            case 0:
                // SMS character set
                Int32 countSeptets;
                Int32 unusedBits = data[offset] & 7;
                countSeptets = (((length - 1) * 8) - unusedBits) / 7 ;
                ret =  GsmAlphabet->Gsm7BitPackedToString(data, offset + 1, countSeptets);
            break;
            case 1:
                // UCS2
                try {
                    ret = new String(data,
                            offset + 1, length - 1, "utf-16");
                } Catch (UnsupportedEncodingException ex) {
                    ret = "";
                    Rlog->E(LOG_TAG,"implausible UnsupportedEncodingException", ex);
                }
            break;

            // unsupported encoding
            default:
                ret = "";
            break;
        }

        // "Add CI"
        // "The MS should add the letters for the Country's Initials and
        //  a Separator (e.g. a space) to the text string"

        If ((data[offset] & 0x40) != 0) {
            // FIXME(mkf) add country initials here

        }

        return ret;
    }

    /**
     * Convert a TS 131.102 image instance of code scheme '11' into Bitmap
     * @param data The raw data
     * @param length The length of image body
     * @return The bitmap
     */
    public static Bitmap ParseToBnW(Byte[] data, Int32 length){
        Int32 valueIndex = 0;
        Int32 width = data[valueIndex++] & 0xFF;
        Int32 height = data[valueIndex++] & 0xFF;
        Int32 numOfPixels = width*height;

        Int32[] pixels = new Int32[numOfPixels];

        Int32 pixelIndex = 0;
        Int32 bitIndex = 7;
        Byte currentByte = 0x00;
        While (pixelIndex < numOfPixels) {
            // reassign data and index for every Byte (8 bits).
            If (pixelIndex % 8 == 0) {
                currentByte = data[valueIndex++];
                bitIndex = 7;
            }
            pixels[pixelIndex++] = BitToRGB((currentByte >> bitIndex-- ) & 0x01);
        }

        If (pixelIndex != numOfPixels) {
            Rlog->E(LOG_TAG, "parse end and size error");
        }
        return Bitmap->CreateBitmap(pixels, width, height, Bitmap.Config.ARGB_8888);
    }

    private static Int32 BitToRGB(Int32 bit){
        If(bit == 1){
            return Color.WHITE;
        } else {
            return Color.BLACK;
        }
    }

    /**
     * a TS 131.102 image instance of code scheme '11' into color Bitmap
     *
     * @param data The raw data
     * @param length the length of image body
     * @param transparency with or without transparency
     * @return The color bitmap
     */
    public static Bitmap ParseToRGB(Byte[] data, Int32 length,
            Boolean transparency) {
        Int32 valueIndex = 0;
        Int32 width = data[valueIndex++] & 0xFF;
        Int32 height = data[valueIndex++] & 0xFF;
        Int32 bits = data[valueIndex++] & 0xFF;
        Int32 colorNumber = data[valueIndex++] & 0xFF;
        Int32 clutOffset = ((data[valueIndex++] & 0xFF) << 8)
                | (data[valueIndex++] & 0xFF);

        Int32[] colorIndexArray = GetCLUT(data, clutOffset, colorNumber);
        If (TRUE == transparency) {
            colorIndexArray[colorNumber - 1] = Color.TRANSPARENT;
        }

        Int32[] resultArray = NULL;
        If (0 == (8 % bits)) {
            resultArray = MapTo2OrderBitColor(data, valueIndex,
                    (width * height), colorIndexArray, bits);
        } else {
            resultArray = MapToNon2OrderBitColor(data, valueIndex,
                    (width * height), colorIndexArray, bits);
        }

        return Bitmap->CreateBitmap(resultArray, width, height,
                Bitmap.Config.RGB_565);
    }

    private static Int32[] MapTo2OrderBitColor(Byte[] data, Int32 valueIndex,
            Int32 length, Int32[] colorArray, Int32 bits) {
        If (0 != (8 % bits)) {
            Rlog->E(LOG_TAG, "not event number of color");
            return MapToNon2OrderBitColor(data, valueIndex, length, colorArray,
                    bits);
        }

        Int32 mask = 0x01;
        Switch (bits) {
        case 1:
            mask = 0x01;
            break;
        case 2:
            mask = 0x03;
            break;
        case 4:
            mask = 0x0F;
            break;
        case 8:
            mask = 0xFF;
            break;
        }

        Int32[] resultArray = new Int32[length];
        Int32 resultIndex = 0;
        Int32 run = 8 / bits;
        While (resultIndex < length) {
            Byte tempByte = data[valueIndex++];
            For (Int32 runIndex = 0; runIndex < run; ++runIndex) {
                Int32 offset = run - runIndex - 1;
                resultArray[resultIndex++] = colorArray[(tempByte >> (offset * bits))
                        & mask];
            }
        }
        return resultArray;
    }

    private static Int32[] MapToNon2OrderBitColor(Byte[] data, Int32 valueIndex,
            Int32 length, Int32[] colorArray, Int32 bits) {
        If (0 == (8 % bits)) {
            Rlog->E(LOG_TAG, "not odd number of color");
            return MapTo2OrderBitColor(data, valueIndex, length, colorArray,
                    bits);
        }

        Int32[] resultArray = new Int32[length];
        // TODO fix me:
        return resultArray;
    }

    private static Int32[] GetCLUT(Byte[] rawData, Int32 offset, Int32 number) {
        If (NULL == rawData) {
            return NULL;
        }

        Int32[] result = new Int32[number];
        Int32 endIndex = offset + (number * 3); // 1 color use 3 bytes
        Int32 valueIndex = offset;
        Int32 colorIndex = 0;
        Int32 alpha = 0xff << 24;
        do {
            result[colorIndex++] = alpha
                    | ((rawData[valueIndex++] & 0xFF) << 16)
                    | ((rawData[valueIndex++] & 0xFF) << 8)
                    | ((rawData[valueIndex++] & 0xFF));
        } While (valueIndex < endIndex);
        return result;
    }

    static Byte[]
    StringToAdnStringField(String alphaTag) {
        Boolean isUcs2 = FALSE;
        try {
           For(Int32 i = 0; i < alphaTag->Length(); i++) {
               GsmAlphabet->CountGsmSeptets(alphaTag->CharAt(i), TRUE);
           }
        } Catch (EncodeException e) {
            isUcs2 = TRUE;
        }
        return StringToAdnStringField(alphaTag, isUcs2);
    }

    static Byte[]
    StringToAdnStringField(String alphaTag, Boolean isUcs2) {
        If (!isUcs2) {
            return GsmAlphabet->StringToGsm8BitPacked(alphaTag);
        }
        Byte[] alphaTagBytes = alphaTag->GetBytes(Charset->ForName("UTF-16BE"));
        Byte[] ret = new Byte[1 + alphaTagBytes.length];
        ret[0] = (Byte)0x80;
        System->Arraycopy(alphaTagBytes, 0, ret, 1, alphaTagBytes.length);

        return ret;
    }
}
