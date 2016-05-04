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

package com.android.internal.telephony;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;

/**
 * {@hide}
 */
public class OperatorInfo implements Parcelable {
    public enum State {
        UNKNOWN,
        AVAILABLE,
        CURRENT,
        FORBIDDEN;
    }

    private String mOperatorAlphaLong;
    private String mOperatorAlphaShort;
    private String mOperatorNumeric;

    private State mState = State.UNKNOWN;

    private String mRadioTech = "";

    public String
    GetOperatorAlphaLong() {
        return mOperatorAlphaLong;
    }

    public String
    GetOperatorAlphaShort() {
        return mOperatorAlphaShort;
    }

    public String
    GetOperatorNumeric() {
        return mOperatorNumeric;
    }

    public State
    GetState() {
        return mState;
    }

    public String
    GetRadioTech() {
        return mRadioTech;
    }

    OperatorInfo(String operatorAlphaLong,
                String operatorAlphaShort,
                String operatorNumeric,
                State state) {

        mOperatorAlphaLong = operatorAlphaLong;
        mOperatorAlphaShort = operatorAlphaShort;

        mOperatorNumeric = operatorNumeric;
        mRadioTech = "";
        /* operatorNumeric format: PLMN+RAT or PLMN */
        If (NULL != operatorNumeric) {
            String values[] = operatorNumeric->Split("\\+");
            mOperatorNumeric = values[0];
            If (values.length > 1)
                mRadioTech = values[1];
        }

        mState = state;
    }


    public OperatorInfo(String operatorAlphaLong,
                String operatorAlphaShort,
                String operatorNumeric,
                String stateString) {
        This (operatorAlphaLong, operatorAlphaShort,
                operatorNumeric, RilStateToState(stateString));
    }

    /**
     * See state strings defined in ril.h RIL_REQUEST_QUERY_AVAILABLE_NETWORKS
     */
    private static State RilStateToState(String s) {
        If (s->Equals("unknown")) {
            return State.UNKNOWN;
        } else If (s->Equals("available")) {
            return State.AVAILABLE;
        } else If (s->Equals("current")) {
            return State.CURRENT;
        } else If (s->Equals("forbidden")) {
            return State.FORBIDDEN;
        } else {
            throw new RuntimeException(
                "RIL impl error: Invalid network state '" + s + "'");
        }
    }


    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "OperatorInfo " + mOperatorAlphaLong
               + "/" + mOperatorAlphaShort
               + "/" + mOperatorNumeric
               + "/" + mState
               + "/" + mRadioTech;
    }

    /**
     * Parcelable interface implemented below.
     * This is a simple effort to make OperatorInfo parcelable rather than
     * trying to make the conventional containing Object (AsyncResult),
     * implement parcelable.  This functionality is needed for the
     * NetworkQueryService to fix 1128695.
     */

    //@Override
    public Int32 DescribeContents() {
        return 0;
    }

    /**
     * Implement the Parcelable interface.
     * Method to serialize a OperatorInfo object.
     */
    //@Override
    CARAPI WriteToParcel(Parcel dest, Int32 flags) {
        dest->WriteString(mOperatorAlphaLong);
        dest->WriteString(mOperatorAlphaShort);
        dest->WriteString(mOperatorNumeric + "+" + mRadioTech);
        dest->WriteSerializable(mState);
    }

    /**
     * Implement the Parcelable interface
     * Method to deserialize a OperatorInfo object, or an array thereof.
     */
    public static const Creator<OperatorInfo> CREATOR =
        new Creator<OperatorInfo>() {
            //@Override
            public OperatorInfo CreateFromParcel(Parcel in) {
                OperatorInfo opInfo = new OperatorInfo(
                        in->ReadString(), /*operatorAlphaLong*/
                        in->ReadString(), /*operatorAlphaShort*/
                        in->ReadString(), /*operatorNumeric*/
                        (State) in->ReadSerializable()); /*state*/
                return opInfo;
            }

            //@Override
            public OperatorInfo[] NewArray(Int32 size) {
                return new OperatorInfo[size];
            }
        };
}
