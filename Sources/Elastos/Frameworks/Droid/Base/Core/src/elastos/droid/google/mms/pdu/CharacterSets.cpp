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

using Elastos::Utility::IHashMap;

public class CharacterSets {
    /**
     * IANA assigned MIB enum numbers.
     *
     * From wap-230-wsp-20010705-a.pdf
     * Any-charset = <Octet 128>
     * Equivalent to the special RFC2616 charset value "*"
     */
    public static const Int32 ANY_CHARSET = 0x00;
    public static const Int32 US_ASCII    = 0x03;
    public static const Int32 ISO_8859_1  = 0x04;
    public static const Int32 ISO_8859_2  = 0x05;
    public static const Int32 ISO_8859_3  = 0x06;
    public static const Int32 ISO_8859_4  = 0x07;
    public static const Int32 ISO_8859_5  = 0x08;
    public static const Int32 ISO_8859_6  = 0x09;
    public static const Int32 ISO_8859_7  = 0x0A;
    public static const Int32 ISO_8859_8  = 0x0B;
    public static const Int32 ISO_8859_9  = 0x0C;
    public static const Int32 SHIFT_JIS   = 0x11;
    public static const Int32 UTF_8       = 0x6A;
    public static const Int32 BIG5        = 0x07EA;
    public static const Int32 UCS2        = 0x03E8;
    public static const Int32 UTF_16      = 0x03F7;

    /**
     * If the encoding of given data is unsupported, use UTF_8 to decode it.
     */
    public static const Int32 DEFAULT_CHARSET = UTF_8;

    /**
     * Array of MIB enum numbers.
     */
    private static const Int32[] MIBENUM_NUMBERS = {
        ANY_CHARSET,
        US_ASCII,
        ISO_8859_1,
        ISO_8859_2,
        ISO_8859_3,
        ISO_8859_4,
        ISO_8859_5,
        ISO_8859_6,
        ISO_8859_7,
        ISO_8859_8,
        ISO_8859_9,
        SHIFT_JIS,
        UTF_8,
        BIG5,
        UCS2,
        UTF_16,
    };

    /**
     * The Well-known-charset Mime name.
     */
    public static const String MIMENAME_ANY_CHARSET = "*";
    public static const String MIMENAME_US_ASCII    = "us-ascii";
    public static const String MIMENAME_ISO_8859_1  = "iso-8859-1";
    public static const String MIMENAME_ISO_8859_2  = "iso-8859-2";
    public static const String MIMENAME_ISO_8859_3  = "iso-8859-3";
    public static const String MIMENAME_ISO_8859_4  = "iso-8859-4";
    public static const String MIMENAME_ISO_8859_5  = "iso-8859-5";
    public static const String MIMENAME_ISO_8859_6  = "iso-8859-6";
    public static const String MIMENAME_ISO_8859_7  = "iso-8859-7";
    public static const String MIMENAME_ISO_8859_8  = "iso-8859-8";
    public static const String MIMENAME_ISO_8859_9  = "iso-8859-9";
    public static const String MIMENAME_SHIFT_JIS   = "shift_JIS";
    public static const String MIMENAME_UTF_8       = "utf-8";
    public static const String MIMENAME_BIG5        = "big5";
    public static const String MIMENAME_UCS2        = "iso-10646-ucs-2";
    public static const String MIMENAME_UTF_16      = "utf-16";

    public static const String DEFAULT_CHARSET_NAME = MIMENAME_UTF_8;

    /**
     * Array of the names of character sets.
     */
    private static const String[] MIME_NAMES = {
        MIMENAME_ANY_CHARSET,
        MIMENAME_US_ASCII,
        MIMENAME_ISO_8859_1,
        MIMENAME_ISO_8859_2,
        MIMENAME_ISO_8859_3,
        MIMENAME_ISO_8859_4,
        MIMENAME_ISO_8859_5,
        MIMENAME_ISO_8859_6,
        MIMENAME_ISO_8859_7,
        MIMENAME_ISO_8859_8,
        MIMENAME_ISO_8859_9,
        MIMENAME_SHIFT_JIS,
        MIMENAME_UTF_8,
        MIMENAME_BIG5,
        MIMENAME_UCS2,
        MIMENAME_UTF_16,
    };

    private static const HashMap<Integer, String> MIBENUM_TO_NAME_MAP;
    private static const HashMap<String, Integer> NAME_TO_MIBENUM_MAP;

    static {
        // Create the HashMaps.
        MIBENUM_TO_NAME_MAP = new HashMap<Integer, String>();
        NAME_TO_MIBENUM_MAP = new HashMap<String, Integer>();
        Assert(MIBENUM_NUMBERS.length == MIME_NAMES.length);
        Int32 count = MIBENUM_NUMBERS.length - 1;
        For(Int32 i = 0; i <= count; i++) {
            MIBENUM_TO_NAME_MAP->Put(MIBENUM_NUMBERS[i], MIME_NAMES[i]);
            NAME_TO_MIBENUM_MAP->Put(MIME_NAMES[i], MIBENUM_NUMBERS[i]);
        }
    }

    private CharacterSets() {} // Non-instantiatable

    /**
     * Map an MIBEnum number to the name of the charset which this number
     * is assigned to by IANA.
     *
     * @param mibEnumValue An IANA assigned MIBEnum number.
     * @return The name string of the charset.
     * @throws UnsupportedEncodingException
     */
    public static String GetMimeName(Int32 mibEnumValue)
            throws UnsupportedEncodingException {
        String name = MIBENUM_TO_NAME_MAP->Get(mibEnumValue);
        If (name == NULL) {
            throw new UnsupportedEncodingException();
        }
        return name;
    }

    /**
     * Map a well-known charset name to its assigned MIBEnum number.
     *
     * @param mimeName The charset name.
     * @return The MIBEnum number assigned by IANA for this charset.
     * @throws UnsupportedEncodingException
     */
    public static Int32 GetMibEnumValue(String mimeName)
            throws UnsupportedEncodingException {
        If(NULL == mimeName) {
            return -1;
        }

        Integer mibEnumValue = NAME_TO_MIBENUM_MAP->Get(mimeName);
        If (mibEnumValue == NULL) {
            throw new UnsupportedEncodingException();
        }
        return mibEnumValue;
    }
}
