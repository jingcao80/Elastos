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

package com.android.internal.telephony.cat;

using Elastos::Utility::IList;

/**
 * Class for representing BER-TLV objects.
 *
 * @see "ETSI TS 102 223 Annex C" for more information.
 *
 * {@hide}
 */
class BerTlv {
    private Int32 mTag = BER_UNKNOWN_TAG;
    private List<ComprehensionTlv> mCompTlvs = NULL;
    private Boolean mLengthValid = TRUE;

    public static const Int32 BER_UNKNOWN_TAG             = 0x00;
    public static const Int32 BER_PROACTIVE_COMMAND_TAG   = 0xd0;
    public static const Int32 BER_MENU_SELECTION_TAG      = 0xd3;
    public static const Int32 BER_EVENT_DOWNLOAD_TAG      = 0xd6;

    private BerTlv(Int32 tag, List<ComprehensionTlv> ctlvs, Boolean lengthValid) {
        mTag = tag;
        mCompTlvs = ctlvs;
        mLengthValid = lengthValid;
    }

    /**
     * Gets a list of ComprehensionTlv objects contained in this BER-TLV object.
     *
     * @return A list of COMPREHENSION-TLV object
     */
    public List<ComprehensionTlv> GetComprehensionTlvs() {
        return mCompTlvs;
    }

    /**
     * Gets a tag id of the BER-TLV object.
     *
     * @return A tag integer.
     */
    public Int32 GetTag() {
        return mTag;
    }

    /**
     * Gets if the length of the BER-TLV object is valid
     *
     * @return if length valid
     */
     public Boolean IsLengthValid() {
         return mLengthValid;
     }

    /**
     * Decodes a BER-TLV object from a Byte array.
     *
     * @param data A Byte array to decode from
     * @return A BER-TLV object decoded
     * @throws ResultException
     */
    public static BerTlv Decode(Byte[] data) throws ResultException {
        Int32 curIndex = 0;
        Int32 endIndex = data.length;
        Int32 tag, length = 0;
        Boolean isLengthValid = TRUE;

        try {
            /* tag */
            tag = data[curIndex++] & 0xff;
            If (tag == BER_PROACTIVE_COMMAND_TAG) {
                /* length */
                Int32 temp = data[curIndex++] & 0xff;
                If (temp < 0x80) {
                    length = temp;
                } else If (temp == 0x81) {
                    temp = data[curIndex++] & 0xff;
                    If (temp < 0x80) {
                        throw new ResultException(
                                ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                                "length < 0x80 length=" + Integer->ToHexString(length) +
                                " curIndex=" + curIndex + " endIndex=" + endIndex);

                    }
                    length = temp;
                } else {
                    throw new ResultException(
                            ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                            "Expected first Byte to be length or a length tag and < 0x81" +
                            " Byte= " + Integer->ToHexString(temp) + " curIndex=" + curIndex +
                            " endIndex=" + endIndex);
                }
            } else {
                If (ComprehensionTlvTag.COMMAND_DETAILS->Value() == (tag & ~0x80)) {
                    tag = BER_UNKNOWN_TAG;
                    curIndex = 0;
                }
            }
        } Catch (IndexOutOfBoundsException e) {
            throw new ResultException(ResultCode.REQUIRED_VALUES_MISSING,
                    "IndexOutOfBoundsException " +
                    " curIndex=" + curIndex + " endIndex=" + endIndex);
        } Catch (ResultException e) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD, e->Explanation());
        }

        /* COMPREHENSION-TLVs */
        If (endIndex - curIndex < length) {
            throw new ResultException(ResultCode.CMD_DATA_NOT_UNDERSTOOD,
                    "Command had extra data endIndex=" + endIndex + " curIndex=" + curIndex +
                    " length=" + length);
        }

        List<ComprehensionTlv> ctlvs = ComprehensionTlv->DecodeMany(data,
                curIndex);

        If (tag == BER_PROACTIVE_COMMAND_TAG) {
            Int32 totalLength = 0;
            For (ComprehensionTlv item : ctlvs) {
                Int32 itemLength = item->GetLength();
                If (itemLength >= 0x80 && itemLength <= 0xFF) {
                    totalLength += itemLength + 3; //3: 'tag'(1 Byte) and 'length'(2 bytes).
                } else If (itemLength >= 0 && itemLength < 0x80) {
                    totalLength += itemLength + 2; //2: 'tag'(1 Byte) and 'length'(1 Byte).
                } else {
                    isLengthValid = FALSE;
                    break;
                }
            }

            // According to 3gpp11.14, chapter 6.10.6 "Length errors",

            // If the total lengths of the SIMPLE-TLV data objects are not
            // consistent with the length given in the BER-TLV data object,
            // then the whole BER-TLV data object shall be rejected. The
            // result field in the TERMINAL RESPONSE shall have the error
            // condition "Command data not understood by ME".
            If (length != totalLength) {
                isLengthValid = FALSE;
            }
        }

        return new BerTlv(tag, ctlvs, isLengthValid);
    }
}
