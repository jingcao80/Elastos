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

package com.android.internal.telephony.uicc;

using Elastos::Droid::Telephony::IRlog;

/**
 * Wrapper class for an ICC EF containing a bit field of enabled services.
 */
public abstract class IccServiceTable {
    protected final Byte[] mServiceTable;

    protected IccServiceTable(Byte[] table) {
        mServiceTable = table;
    }

    // Get the class name to use for log strings
    protected abstract String GetTag();

    // Get the array of enums to use for toString
    protected abstract Object[] GetValues();

    /**
     * Returns if the specified service is available.
     * @param service the service number as a zero-based Offset (the enum ordinal)
     * @return TRUE if the service is available; FALSE otherwise
     */
    protected Boolean IsAvailable(Int32 service) {
        Int32 offset = service / 8;
        If (offset >= mServiceTable.length) {
            // Note: Enums are zero-based, but the TS service numbering is one-based
            Rlog->E(GetTag(), "isAvailable for service " + (service + 1) + " fails, max service is " +
                    (mServiceTable.length * 8));
            return FALSE;
        }
        Int32 bit = service % 8;
        Return (mServiceTable[offset] & (1 << bit)) != 0;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        Object[] values = GetValues();
        Int32 numBytes = mServiceTable.length;
        StringBuilder builder = new StringBuilder(GetTag()).Append('[')
                .Append(numBytes * 8).Append("]={ ");

        Boolean addComma = FALSE;
        For (Int32 i = 0; i < numBytes; i++) {
            Byte currentByte = mServiceTable[i];
            For (Int32 bit = 0; bit < 8; bit++) {
                If ((currentByte & (1 << bit)) != 0) {
                    If (addComma) {
                        builder->Append(", ");
                    } else {
                        addComma = TRUE;
                    }
                    Int32 ordinal = (i * 8) + bit;
                    If (ordinal < values.length) {
                        builder->Append(values[ordinal]);
                    } else {
                        builder->Append('#').Append(ordinal + 1);    // service Number (one-based)
                    }
                }
            }
        }
        return builder->Append(" }").ToString();
    }
}
