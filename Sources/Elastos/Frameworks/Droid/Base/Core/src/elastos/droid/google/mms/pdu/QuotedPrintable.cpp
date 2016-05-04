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

using Elastos::IO::IByteArrayOutputStream;

public class QuotedPrintable {
    private static Byte ESCAPE_CHAR = '=';

    /**
     * Decodes an array quoted-printable characters into an array of original bytes.
     * Escaped characters are converted back to their original representation.
     *
     * <p>
     * This function implements a subset of
     * quoted-printable encoding Specification (rule #1 and rule #2)
     * as defined in RFC 1521.
     * </p>
     *
     * @param bytes array of quoted-printable characters
     * @return array of original bytes,
     *         NULL if quoted-printable decoding is unsuccessful.
     */
    public static const Byte[] DecodeQuotedPrintable(Byte[] bytes) {
        If (bytes == NULL) {
            return NULL;
        }
        ByteArrayOutputStream buffer = new ByteArrayOutputStream();
        For (Int32 i = 0; i < bytes.length; i++) {
            Int32 b = bytes[i];
            If (b == ESCAPE_CHAR) {
                try {
                    If('\r' == (Char32)bytes[i + 1] &&
                            '\n' == (Char32)bytes[i + 2]) {
                        i += 2;
                        continue;
                    }
                    Int32 u = Character->Digit((Char32) bytes[++i], 16);
                    Int32 l = Character->Digit((Char32) bytes[++i], 16);
                    If (u == -1 || l == -1) {
                        return NULL;
                    }
                    buffer->Write((Char32) ((u << 4) + l));
                } Catch (ArrayIndexOutOfBoundsException e) {
                    return NULL;
                }
            } else {
                buffer->Write(b);
            }
        }
        return buffer->ToByteArray();
    }
}
