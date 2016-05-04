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

public class RestrictedState {

    /**
     * Set TRUE to block packet data access due to restriction
     */
    private Boolean mPsRestricted;
    /**
     * Set TRUE to block all normal voice/SMS/USSD/SS/AV64 due to restriction
     */
    private Boolean mCsNormalRestricted;
    /**
     * Set TRUE to block emergency call due to restriction
     */
    private Boolean mCsEmergencyRestricted;

    public RestrictedState() {
        SetPsRestricted(FALSE);
        SetCsNormalRestricted(FALSE);
        SetCsEmergencyRestricted(FALSE);
    }

    /**
     * @param csEmergencyRestricted the csEmergencyRestricted to set
     */
    CARAPI SetCsEmergencyRestricted(Boolean csEmergencyRestricted) {
        mCsEmergencyRestricted = csEmergencyRestricted;
    }

    /**
     * @return the csEmergencyRestricted
     */
    public Boolean IsCsEmergencyRestricted() {
        return mCsEmergencyRestricted;
    }

    /**
     * @param csNormalRestricted the csNormalRestricted to set
     */
    CARAPI SetCsNormalRestricted(Boolean csNormalRestricted) {
        mCsNormalRestricted = csNormalRestricted;
    }

    /**
     * @return the csNormalRestricted
     */
    public Boolean IsCsNormalRestricted() {
        return mCsNormalRestricted;
    }

    /**
     * @param psRestricted the psRestricted to set
     */
    CARAPI SetPsRestricted(Boolean psRestricted) {
        mPsRestricted = psRestricted;
    }

    /**
     * @return the psRestricted
     */
    public Boolean IsPsRestricted() {
        return mPsRestricted;
    }

    public Boolean IsCsRestricted() {
        return mCsNormalRestricted && mCsEmergencyRestricted;
    }

    //@Override
    public Boolean Equals (Object o) {
        RestrictedState s;

        try {
            s = (RestrictedState) o;
        } Catch (ClassCastException ex) {
            return FALSE;
        }

        If (o == NULL) {
            return FALSE;
        }

        return mPsRestricted == s.mPsRestricted
        && mCsNormalRestricted == s.mCsNormalRestricted
        && mCsEmergencyRestricted == s.mCsEmergencyRestricted;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        String csString = "none";

        If (mCsEmergencyRestricted && mCsNormalRestricted) {
            csString = "all";
        } else If (mCsEmergencyRestricted && !mCsNormalRestricted) {
            csString = "emergency";
        } else If (!mCsEmergencyRestricted && mCsNormalRestricted) {
            csString = "normal call";
        }

        return  "Restricted State CS: " + csString + " PS:" + mPsRestricted;
    }

}
