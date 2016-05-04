/*
 * Copyright (C) 2009 The Android Open Source Project
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
 * SmsBroadcastConfigInfo defines one configuration of Cell Broadcast
 * Message (CBM) to be received by the ME
 *
 * fromServiceId - toServiceId defines a range of CBM message identifiers
 * whose value is 0x0000 - 0xFFFF as defined in TS 23.041 9.4.1.2.2 for GMS
 * and 9.4.4.2.2 for UMTS. All other values can be treated as empty
 * CBM message ID.
 *
 * fromCodeScheme - toCodeScheme defines a range of CBM data coding schemes
 * whose value is 0x00 - 0xFF as defined in TS 23.041 9.4.1.2.3 for GMS
 * and 9.4.4.2.3 for UMTS.
 * All other values can be treated as empty CBM data coding scheme.
 *
 * selected FALSE means message types specified in {@code <fromServiceId, toServiceId>}
 * and {@code <fromCodeScheme, toCodeScheme>} are not accepted, while TRUE means accepted.
 *
 */
public class SmsBroadcastConfigInfo {
    private Int32 mFromServiceId;
    private Int32 mToServiceId;
    private Int32 mFromCodeScheme;
    private Int32 mToCodeScheme;
    private Boolean mSelected;

    /**
     * Initialize the object from rssi and cid.
     */
    public SmsBroadcastConfigInfo(Int32 fromId, Int32 toId, Int32 fromScheme,
            Int32 toScheme, Boolean selected) {
        mFromServiceId = fromId;
        mToServiceId = toId;
        mFromCodeScheme = fromScheme;
        mToCodeScheme = toScheme;
        mSelected = selected;
    }

    /**
     * @param fromServiceId the fromServiceId to set
     */
    CARAPI SetFromServiceId(Int32 fromServiceId) {
        mFromServiceId = fromServiceId;
    }

    /**
     * @return the fromServiceId
     */
    public Int32 GetFromServiceId() {
        return mFromServiceId;
    }

    /**
     * @param toServiceId the toServiceId to set
     */
    CARAPI SetToServiceId(Int32 toServiceId) {
        mToServiceId = toServiceId;
    }

    /**
     * @return the toServiceId
     */
    public Int32 GetToServiceId() {
        return mToServiceId;
    }

    /**
     * @param fromCodeScheme the fromCodeScheme to set
     */
    CARAPI SetFromCodeScheme(Int32 fromCodeScheme) {
        mFromCodeScheme = fromCodeScheme;
    }

    /**
     * @return the fromCodeScheme
     */
    public Int32 GetFromCodeScheme() {
        return mFromCodeScheme;
    }

    /**
     * @param toCodeScheme the toCodeScheme to set
     */
    CARAPI SetToCodeScheme(Int32 toCodeScheme) {
        mToCodeScheme = toCodeScheme;
    }

    /**
     * @return the toCodeScheme
     */
    public Int32 GetToCodeScheme() {
        return mToCodeScheme;
    }

    /**
     * @param selected the selected to set
     */
    CARAPI SetSelected(Boolean selected) {
        mSelected = selected;
    }

    /**
     * @return the selected
     */
    public Boolean IsSelected() {
        return mSelected;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "SmsBroadcastConfigInfo: Id [" +
                mFromServiceId + ',' + mToServiceId + "] Code [" +
                mFromCodeScheme + ',' + mToCodeScheme + "] " +
            (mSelected ? "ENABLED" : "DISABLED");
    }
}
