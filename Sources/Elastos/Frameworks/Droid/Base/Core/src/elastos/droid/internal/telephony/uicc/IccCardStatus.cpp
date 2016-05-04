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
 * See also RIL_CardStatus in include/telephony/ril.h
 *
 * {@hide}
 */
public class IccCardStatus {
    public static const Int32 CARD_MAX_APPS = 8;

    public enum CardState {
        CARDSTATE_ABSENT,
        CARDSTATE_PRESENT,
        CARDSTATE_ERROR;

        public Boolean IsCardPresent() {
            return this == CARDSTATE_PRESENT;
        }
    }

    public enum PinState {
        PINSTATE_UNKNOWN,
        PINSTATE_ENABLED_NOT_VERIFIED,
        PINSTATE_ENABLED_VERIFIED,
        PINSTATE_DISABLED,
        PINSTATE_ENABLED_BLOCKED,
        PINSTATE_ENABLED_PERM_BLOCKED;

        Boolean IsPermBlocked() {
            return this == PINSTATE_ENABLED_PERM_BLOCKED;
        }

        Boolean IsPinRequired() {
            return this == PINSTATE_ENABLED_NOT_VERIFIED;
        }

        Boolean IsPukRequired() {
            return this == PINSTATE_ENABLED_BLOCKED;
        }
    }

    public CardState  mCardState;
    public PinState   mUniversalPinState;
    public Int32        mGsmUmtsSubscriptionAppIndex;
    public Int32        mCdmaSubscriptionAppIndex;
    public Int32        mImsSubscriptionAppIndex;

    public IccCardApplicationStatus[] mApplications;

    CARAPI SetCardState(Int32 state) {
        Switch(state) {
        case 0:
            mCardState = CardState.CARDSTATE_ABSENT;
            break;
        case 1:
            mCardState = CardState.CARDSTATE_PRESENT;
            break;
        case 2:
            mCardState = CardState.CARDSTATE_ERROR;
            break;
        default:
            throw new RuntimeException("Unrecognized RIL_CardState: " + state);
        }
    }

    CARAPI SetUniversalPinState(Int32 state) {
        Switch(state) {
        case 0:
            mUniversalPinState = PinState.PINSTATE_UNKNOWN;
            break;
        case 1:
            mUniversalPinState = PinState.PINSTATE_ENABLED_NOT_VERIFIED;
            break;
        case 2:
            mUniversalPinState = PinState.PINSTATE_ENABLED_VERIFIED;
            break;
        case 3:
            mUniversalPinState = PinState.PINSTATE_DISABLED;
            break;
        case 4:
            mUniversalPinState = PinState.PINSTATE_ENABLED_BLOCKED;
            break;
        case 5:
            mUniversalPinState = PinState.PINSTATE_ENABLED_PERM_BLOCKED;
            break;
        default:
            throw new RuntimeException("Unrecognized RIL_PinState: " + state);
        }
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        IccCardApplicationStatus app;

        StringBuilder sb = new StringBuilder();
        sb->Append("IccCardState {").Append(mCardState).Append(",")
        .Append(mUniversalPinState)
        .Append(",num_apps=").Append(mApplications.length)
        .Append(",gsm_id=").Append(mGsmUmtsSubscriptionAppIndex);
        If (mGsmUmtsSubscriptionAppIndex >=0
                && mGsmUmtsSubscriptionAppIndex <CARD_MAX_APPS) {
            app = mApplications[mGsmUmtsSubscriptionAppIndex];
            sb->Append(app == NULL ? "NULL" : app);
        }

        sb->Append(",cdma_id=").Append(mCdmaSubscriptionAppIndex);
        If (mCdmaSubscriptionAppIndex >=0
                && mCdmaSubscriptionAppIndex <CARD_MAX_APPS) {
            app = mApplications[mCdmaSubscriptionAppIndex];
            sb->Append(app == NULL ? "NULL" : app);
        }

        sb->Append(",ims_id=").Append(mImsSubscriptionAppIndex);
        If (mImsSubscriptionAppIndex >=0
                && mImsSubscriptionAppIndex <CARD_MAX_APPS) {
            app = mApplications[mImsSubscriptionAppIndex];
            sb->Append(app == NULL ? "NULL" : app);
        }

        sb->Append("}");

        return sb->ToString();
    }

}
