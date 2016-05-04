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

public class Base64 {
    /**
     * Used to get the number of Quadruples.
     */
    static const Int32 FOURBYTE = 4;

    /**
     * Byte used to pad output.
     */
    static const Byte PAD = (Byte) '=';

    /**
     * The base length.
     */
    static const Int32 BASELENGTH = 255;

    // Create arrays to hold the base64 characters
    private static Byte[] base64Alphabet = new Byte[BASELENGTH];

    // Populating the character arrays
    static {
        For (Int32 i = 0; i < BASELENGTH; i++) {
            base64Alphabet[i] = (Byte) -1;
        }
        For (Int32 i = 'Z'; i >= 'A'; i--) {
            base64Alphabet[i] = (Byte) (i - 'A');
        }
        For (Int32 i = 'z'; i >= 'a'; i--) {
            base64Alphabet[i] = (Byte) (i - 'a' + 26);
        }
        For (Int32 i = '9'; i >= '0'; i--) {
            base64Alphabet[i] = (Byte) (i - '0' + 52);
        }

        base64Alphabet['+'] = 62;
        base64Alphabet['/'] = 63;
    }

    /**
     * Decodes Base64 data into octects
     *
     * @param base64Data Byte array containing Base64 data
     * @return Array containing decoded data.
     */
    public static Byte[] DecodeBase64(Byte[] base64Data) {
        // RFC 2045 requires that we discard ALL non-Base64 characters
        base64Data = DiscardNonBase64(base64Data);

        // handle the edge case, so we don't have to worry about it later
        If (base64Data.length == 0) {
            return new Byte[0];
        }

        Int32 numberQuadruple = base64Data.length / FOURBYTE;
        Byte decodedData[] = NULL;
        Byte b1 = 0, b2 = 0, b3 = 0, b4 = 0, marker0 = 0, marker1 = 0;

        // Throw away anything not in base64Data

        Int32 encodedIndex = 0;
        Int32 dataIndex = 0;
        {
            // this sizes the output array properly - rlw
            Int32 lastData = base64Data.length;
            // ignore the '=' padding
            While (base64Data[lastData - 1] == PAD) {
                If (--lastData == 0) {
                    return new Byte[0];
                }
            }
            decodedData = new Byte[lastData - numberQuadruple];
        }

        For (Int32 i = 0; i < numberQuadruple; i++) {
            dataIndex = i * 4;
            marker0 = base64Data[dataIndex + 2];
            marker1 = base64Data[dataIndex + 3];

            b1 = base64Alphabet[base64Data[dataIndex]];
            b2 = base64Alphabet[base64Data[dataIndex + 1]];

            If (marker0 != PAD && marker1 != PAD) {
                //No PAD e.g 3cQl
                b3 = base64Alphabet[marker0];
                b4 = base64Alphabet[marker1];

                decodedData[encodedIndex] = (Byte) (b1 << 2 | b2 >> 4);
                decodedData[encodedIndex + 1] =
                    (Byte) (((b2 & 0xf) << 4) | ((b3 >> 2) & 0xf));
                decodedData[encodedIndex + 2] = (Byte) (b3 << 6 | b4);
            } else If (marker0 == PAD) {
                //Two PAD e.g. 3c[Pad][Pad]
                decodedData[encodedIndex] = (Byte) (b1 << 2 | b2 >> 4);
            } else If (marker1 == PAD) {
                //One PAD e.g. 3cQ[Pad]
                b3 = base64Alphabet[marker0];

                decodedData[encodedIndex] = (Byte) (b1 << 2 | b2 >> 4);
                decodedData[encodedIndex + 1] =
                    (Byte) (((b2 & 0xf) << 4) | ((b3 >> 2) & 0xf));
            }
            encodedIndex += 3;
        }
        return decodedData;
    }

    /**
     * Check octect wheter it is a base64 encoding.
     *
     * @param octect to be checked Byte
     * @return ture if it is base64 encoding, FALSE otherwise.
     */
    private static Boolean IsBase64(Byte octect) {
        If (octect == PAD) {
            return TRUE;
        } else If (base64Alphabet[octect] == -1) {
            return FALSE;
        } else {
            return TRUE;
        }
    }

    /**
     * Discards any characters outside of the base64 alphabet, per
     * the requirements on page 25 of RFC 2045 - "Any characters
     * outside of the base64 alphabet are to be ignored in base64
     * encoded data."
     *
     * @param data The base-64 encoded data to groom
     * @return The data, less non-base64 Characters (see RFC 2045).
     */
    static Byte[] DiscardNonBase64(Byte[] data) {
        Byte groomedData[] = new Byte[data.length];
        Int32 bytesCopied = 0;

        For (Int32 i = 0; i < data.length; i++) {
            If (IsBase64(data[i])) {
                groomedData[bytesCopied++] = data[i];
            }
        }

        Byte packedData[] = new Byte[bytesCopied];

        System->Arraycopy(groomedData, 0, packedData, 0, bytesCopied);

        return packedData;
    }
}
