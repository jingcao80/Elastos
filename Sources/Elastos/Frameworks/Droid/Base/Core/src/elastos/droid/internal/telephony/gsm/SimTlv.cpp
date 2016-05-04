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

package com.android.internal.telephony.gsm;

/**
 * SIM Tag-Length-Value record
 * TS 102 223 Annex C
 *
 * {@hide}
 *
 */
public class SimTlv
{
    //***** Private Instance Variables

    Byte mRecord[];
    Int32 mTlvOffset;
    Int32 mTlvLength;
    Int32 mCurOffset;
    Int32 mCurDataOffset;
    Int32 mCurDataLength;
    Boolean mHasValidTlvObject;

    public SimTlv(Byte[] record, Int32 offset, Int32 length) {
        mRecord = record;

        mTlvOffset = offset;
        mTlvLength = length;
        mCurOffset = offset;

        mHasValidTlvObject = ParseCurrentTlvObject();
    }

    public Boolean NextObject() {
        If (!mHasValidTlvObject) return FALSE;
        mCurOffset = mCurDataOffset + mCurDataLength;
        mHasValidTlvObject = ParseCurrentTlvObject();
        return mHasValidTlvObject;
    }

    public Boolean IsValidObject() {
        return mHasValidTlvObject;
    }

    /**
     * Returns the tag for the current TLV object
     * Return 0 if !IsValidObject()
     * 0 and 0xff are invalid tag values
     * valid tags range from 1 - 0xfe
     */
    public Int32 GetTag() {
        If (!mHasValidTlvObject) return 0;
        return mRecord[mCurOffset] & 0xff;
    }

    /**
     * Returns data associated with current TLV object
     * returns NULL if !IsValidObject()
     */

    public Byte[] GetData() {
        If (!mHasValidTlvObject) return NULL;

        Byte[] ret = new Byte[mCurDataLength];
        System->Arraycopy(mRecord, mCurDataOffset, ret, 0, mCurDataLength);
        return ret;
    }

    /**
     * Updates curDataLength and curDataOffset
     * @return FALSE on invalid record, TRUE on valid record
     */

    private Boolean ParseCurrentTlvObject() {
        // 0x00 and 0xff are invalid tag values

        try {
            If (mRecord[mCurOffset] == 0 || (mRecord[mCurOffset] & 0xff) == 0xff) {
                return FALSE;
            }

            If ((mRecord[mCurOffset + 1] & 0xff) < 0x80) {
                // one Byte length 0 - 0x7f
                mCurDataLength = mRecord[mCurOffset + 1] & 0xff;
                mCurDataOffset = mCurOffset + 2;
            } else If ((mRecord[mCurOffset + 1] & 0xff) == 0x81) {
                // two Byte length 0x80 - 0xff
                mCurDataLength = mRecord[mCurOffset + 2] & 0xff;
                mCurDataOffset = mCurOffset + 3;
            } else {
                return FALSE;
            }
        } Catch (ArrayIndexOutOfBoundsException ex) {
            return FALSE;
        }

        If (mCurDataLength + mCurDataOffset > mTlvOffset + mTlvLength) {
            return FALSE;
        }

        return TRUE;
    }

}
