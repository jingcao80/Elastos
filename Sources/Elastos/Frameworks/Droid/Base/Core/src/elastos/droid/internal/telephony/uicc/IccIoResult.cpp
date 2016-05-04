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


/**
 * {@hide}
 */
public class
IccIoResult {
    public Int32 sw1;
    public Int32 sw2;

    public Byte[] payload;

    public IccIoResult(Int32 sw1, Int32 sw2, Byte[] payload) {
        this.sw1 = sw1;
        this.sw2 = sw2;
        this.payload = payload;
    }

    public IccIoResult(Int32 sw1, Int32 sw2, String hexString) {
        This(sw1, sw2, IccUtils->HexStringToBytes(hexString));
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "IccIoResponse sw1:0x" + Integer->ToHexString(sw1) + " sw2:0x"
                + Integer->ToHexString(sw2);
    }

    /**
     * TRUE if this operation was successful
     * See GSM 11.11 Section 9.4
     * (the fun stuff is absent in 51.011)
     */
    public Boolean Success() {
        return sw1 == 0x90 || sw1 == 0x91 || sw1 == 0x9e || sw1 == 0x9f;
    }

    /**
     * Returns exception on error or NULL if success
     */
    public IccException GetException() {
        If (Success()) return NULL;

        Switch (sw1) {
            case 0x94:
                If (sw2 == 0x08) {
                    return new IccFileTypeMismatch();
                } else {
                    return new IccFileNotFound();
                }
            default:
                return new IccException("sw1:" + sw1 + " sw2:" + sw2);
        }
    }
}
