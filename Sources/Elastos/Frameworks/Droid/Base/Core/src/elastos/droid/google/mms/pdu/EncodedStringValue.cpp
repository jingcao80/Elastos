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

using Elastos::Droid::Utility::ILog;

using Elastos::IO::IByteArrayOutputStream;
using Elastos::Utility::IArrayList;

/**
 * Encoded-string-value = Text-string | Value-length Char-set Text-string
 */
public class EncodedStringValue implements Cloneable {
    private static const String TAG = "EncodedStringValue";
    private static const Boolean DEBUG = FALSE;
    private static const Boolean LOCAL_LOGV = FALSE;

    /**
     * The Char-set value.
     */
    private Int32 mCharacterSet;

    /**
     * The Text-string value.
     */
    private Byte[] mData;

    /**
     * Constructor.
     *
     * @param charset the Char-set value
     * @param data the Text-string value
     * @throws NullPointerException if Text-string value is NULL.
     */
    public EncodedStringValue(Int32 charset, Byte[] data) {
        // TODO: CharSet needs to be validated against MIBEnum.
        If(NULL == data) {
            throw new NullPointerException("EncodedStringValue: Text-string is NULL.");
        }

        mCharacterSet = charset;
        mData = new Byte[data.length];
        System->Arraycopy(data, 0, mData, 0, data.length);
    }

    /**
     * Constructor.
     *
     * @param data the Text-string value
     * @throws NullPointerException if Text-string value is NULL.
     */
    public EncodedStringValue(Byte[] data) {
        This(CharacterSets.DEFAULT_CHARSET, data);
    }

    public EncodedStringValue(String data) {
        try {
            mData = data->GetBytes(CharacterSets.DEFAULT_CHARSET_NAME);
            mCharacterSet = CharacterSets.DEFAULT_CHARSET;
        } Catch (UnsupportedEncodingException e) {
            Logger::E(TAG, "Default encoding must be supported.", e);
        }
    }

    /**
     * Get Char-set value.
     *
     * @return the value
     */
    public Int32 GetCharacterSet() {
        return mCharacterSet;
    }

    /**
     * Set Char-set value.
     *
     * @param charset the Char-set value
     */
    CARAPI SetCharacterSet(Int32 charset) {
        // TODO: CharSet needs to be validated against MIBEnum.
        mCharacterSet = charset;
    }

    /**
     * Get Text-string value.
     *
     * @return the value
     */
    public Byte[] GetTextString() {
        Byte[] byteArray = new Byte[mData.length];

        System->Arraycopy(mData, 0, byteArray, 0, mData.length);
        return byteArray;
    }

    /**
     * Set Text-string value.
     *
     * @param textString the Text-string value
     * @throws NullPointerException if Text-string value is NULL.
     */
    CARAPI SetTextString(Byte[] textString) {
        If(NULL == textString) {
            throw new NullPointerException("EncodedStringValue: Text-string is NULL.");
        }

        mData = new Byte[textString.length];
        System->Arraycopy(textString, 0, mData, 0, textString.length);
    }

    /**
     * Convert this object to a {@link java.lang.String}. If the encoding of
     * the EncodedStringValue is NULL or unsupported, it will be
     * treated as iso-8859-1 encoding.
     *
     * @return The decoded String.
     */
    public String GetString()  {
        If (CharacterSets.ANY_CHARSET == mCharacterSet) {
            return new String(mData); // system default encoding.
        } else {
            try {
                String name = CharacterSets->GetMimeName(mCharacterSet);
                return new String(mData, name);
            } Catch (UnsupportedEncodingException e) {
            	If (LOCAL_LOGV) {
            		Logger::V(TAG, e->GetMessage(), e);
            	}
            	try {
                    return new String(mData, CharacterSets.MIMENAME_ISO_8859_1);
                } Catch (UnsupportedEncodingException _) {
                    return new String(mData); // system default encoding.
                }
            }
        }
    }

    /**
     * Append to Text-string.
     *
     * @param textString the textString to append
     * @throws NullPointerException if the text String is NULL
     *                      or an IOException occured.
     */
    CARAPI AppendTextString(Byte[] textString) {
        If(NULL == textString) {
            throw new NullPointerException("Text-string is NULL.");
        }

        If(NULL == mData) {
            mData = new Byte[textString.length];
            System->Arraycopy(textString, 0, mData, 0, textString.length);
        } else {
            ByteArrayOutputStream newTextString = new ByteArrayOutputStream();
            try {
                newTextString->Write(mData);
                newTextString->Write(textString);
            } Catch (IOException e) {
                e->PrintStackTrace();
                throw new NullPointerException(
                        "appendTextString: failed when write a new Text-string");
            }

            mData = newTextString->ToByteArray();
        }
    }

    /*
     * (non-Javadoc)
     * @see java.lang.Object#Clone()
     */
    //@Override
    public Object Clone() throws CloneNotSupportedException {
        super->Clone();
        Int32 len = mData.length;
        Byte[] dstBytes = new Byte[len];
        System->Arraycopy(mData, 0, dstBytes, 0, len);

        try {
            return new EncodedStringValue(mCharacterSet, dstBytes);
        } Catch (Exception e) {
            Logger::E(TAG, "failed to clone an EncodedStringValue: " + this);
            e->PrintStackTrace();
            throw new CloneNotSupportedException(e->GetMessage());
        }
    }

    /**
     * Split this encoded string around matches of the given pattern.
     *
     * @param pattern the delimiting pattern
     * @return the array of encoded strings computed by splitting this encoded
     *         string around matches of the given pattern
     */
    public EncodedStringValue[] Split(String pattern) {
        String[] temp = GetString()->Split(pattern);
        EncodedStringValue[] ret = new EncodedStringValue[temp.length];
        For (Int32 i = 0; i < ret.length; ++i) {
            try {
                ret[i] = new EncodedStringValue(mCharacterSet,
                        temp[i].GetBytes());
            } Catch (NullPointerException _) {
                // Can't arrive here
                return NULL;
            }
        }
        return ret;
    }

    /**
     * Extract an EncodedStringValue[] from a given String.
     */
    public static EncodedStringValue[] Extract(String src) {
        String[] values = src->Split(";");

        ArrayList<EncodedStringValue> list = new ArrayList<EncodedStringValue>();
        For (Int32 i = 0; i < values.length; i++) {
            If (values[i].Length() > 0) {
                list->Add(new EncodedStringValue(values[i]));
            }
        }

        Int32 len = list->Size();
        If (len > 0) {
            return list->ToArray(new EncodedStringValue[len]);
        } else {
            return NULL;
        }
    }

    /**
     * Concatenate an EncodedStringValue[] into a single String.
     */
    public static String Concat(EncodedStringValue[] addr) {
        StringBuilder sb = new StringBuilder();
        Int32 maxIndex = addr.length - 1;
        For (Int32 i = 0; i <= maxIndex; i++) {
            sb->Append(addr[i].GetString());
            If (i < maxIndex) {
                sb->Append(";");
            }
        }

        return sb->ToString();
    }

    public static EncodedStringValue Copy(EncodedStringValue value) {
        If (value == NULL) {
            return NULL;
        }

        return new EncodedStringValue(value.mCharacterSet, value.mData);
    }
    
    public static EncodedStringValue[] EncodeStrings(String[] array) {
        Int32 count = array.length;
        If (count > 0) {
            EncodedStringValue[] encodedArray = new EncodedStringValue[count];
            For (Int32 i = 0; i < count; i++) {
                encodedArray[i] = new EncodedStringValue(array[i]);
            }
            return encodedArray;
        }
        return NULL;
    }
}
