/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.android.internal.telephony.cat;

using Elastos::Droid::Telephony::IRlog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;


/**
 * Class for representing COMPREHENSION-TLV objects.
 *
 * @see "ETSI TS 101 220 subsection 7.1.1"
 *
 * {@hide}
 */
class ComprehensionTlv {
    private static const String LOG_TAG = "ComprehensionTlv";
    private Int32 mTag;
    private Boolean mCr;
    private Int32 mLength;
    private Int32 mValueIndex;
    private Byte[] mRawValue;

    /**
     * Constructor. Private on purpose. Use
     * {@link #DecodeMany(Byte[], Int32) decodeMany} or
     * {@link #Decode(Byte[], Int32) decode} method.
     *
     * @param tag The tag for this object
     * @param cr Comprehension Required flag
     * @param length Length of the value
     * @param data Byte array containing the value
     * @param valueIndex Index in data at which the value starts
     */
    protected ComprehensionTlv(Int32 tag, Boolean cr, Int32 length, Byte[] data,
            Int32 valueIndex) {
        mTag = tag;
        mCr = cr;
        mLength = length;
        mValueIndex = valueIndex;
        mRawValue = data;
    }

    public Int32 GetTag() {
        return mTag;
    }

    public Boolean IsComprehensionRequired() {
        return mCr;
    }

    public Int32 GetLength() {
        return mLength;
    }

    public Int32 GetValueIndex() {
        return mValueIndex;
    }

    public Byte[] GetRawValue() {
        return mRawValue;
    }

    /**
     * Parses a list of COMPREHENSION-TLV objects from a Byte array.
     *
     * @param data A Byte array containing data to be parsed
     * @param startIndex Index in data at which to start parsing
     * @return A list of COMPREHENSION-TLV objects parsed
     * @throws ResultException
     */
    public static List<ComprehensionTlv> DecodeMany(Byte[] data, Int32 startIndex)
            throws ResultException {
        ArrayList<ComprehensionTlv> items = new ArrayList<ComprehensionTlv>();
        Int32 endIndex = data.length;
        While (startIndex < endIndex) {
            ComprehensionTlv ctlv = ComprehensionTlv->Decode(data, startIndex);
            If (ctlv != NULL) {
                items->Add(ctlv);
                startIndex = ctlv.mValueIndex + ctlv.mLength;
            } else {
                CatLogger::D(LOG_TAG, "decodeMany: ctlv is NULL, stop decoding");
                break;
            }
        }

        return items;
    }

    /**
     * Parses an COMPREHENSION-TLV object from a Byte array.
     *
     * @param data A Byte array containing data to be parsed
     * @param startIndex Index in data at which to start parsing
     * @return A COMPREHENSION-TLV object parsed
     * @throws ResultException
     */
    public static ComprehensionTlv Decode(Byte[] data, Int32 startIndex)
            throws ResultException {
        Int32 curIndex = startIndex;
        Int32 endIndex = data.length;

        try {
            /* tag */
            Int32 tag;
            Boolean cr; // Comprehension required flag
            Int32 temp = data[curIndex++] & 0xff;
            Switch (temp) {
            case 0:
            case 0xff:
            case 0x80:
                Rlog->D("CAT     ", "decode: unexpected first tag Byte=" + Integer->ToHexString(temp) +
                        ", startIndex=" + startIndex + " curIndex=" + curIndex +
                        " endIndex=" + endIndex);
                // Return NULL which will stop decoding, this has occurred
                // with Ghana MTN simcard and JDI simcard.
                return NULL;

            case 0x7f: // tag is in three-Byte format
                tag = ((data[curIndex] & 0xff) << 8)
                        | (data[curIndex + 1] & 0xff);
                cr = (tag & 0x8000) != 0;
                tag &= ~0x8000;
                curIndex += 2;
                break;

            default: // tag is in single-Byte format
                tag = temp;
                cr = (tag & 0x80) != 0;
                tag &= ~0x80;
                break;
            }

            /* length */
            Int32 length;
            temp = data[curIndex++] & 0xff;
            If (temp < 0x80) {
                length = temp;
            } else If (temp == 0x81) {
                length = data[curIndex++] & 0xff;
                If (length < 0x80) {
                    throw new ResultException(
                            ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                            "length < 0x80 length=" + Integer->ToHexString(length) +
                            " startIndex=" + startIndex + " curIndex=" + curIndex +
                            " endIndex=" + endIndex);
                }
            } else If (temp == 0x82) {
                length = ((data[curIndex] & 0xff) << 8)
                        | (data[curIndex + 1] & 0xff);
                curIndex += 2;
                If (length < 0x100) {
                    throw new ResultException(
                            ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                            "two Byte length < 0x100 length=" + Integer->ToHexString(length) +
                            " startIndex=" + startIndex + " curIndex=" + curIndex +
                            " endIndex=" + endIndex);
                }
            } else If (temp == 0x83) {
                length = ((data[curIndex] & 0xff) << 16)
                        | ((data[curIndex + 1] & 0xff) << 8)
                        | (data[curIndex + 2] & 0xff);
                curIndex += 3;
                If (length < 0x10000) {
                    throw new ResultException(
                            ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                            "three Byte length < 0x10000 length=0x" + Integer->ToHexString(length) +
                            " startIndex=" + startIndex + " curIndex=" + curIndex +
                            " endIndex=" + endIndex);
                }
            } else {
                throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                        "Bad length modifer=" + temp +
                        " startIndex=" + startIndex + " curIndex=" + curIndex +
                        " endIndex=" + endIndex);

            }

            return new ComprehensionTlv(tag, cr, length, data, curIndex);

        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                    "IndexOutOfBoundsException" + " startIndex=" + startIndex +
                    " curIndex=" + curIndex + " endIndex=" + endIndex);
        }
    }
}
