/*
 * Copyright (c) 2010-2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.android.internal.telephony.dataconnection;

using Elastos::Utility::IArrayList;

using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::Dataconnection::IApnSetting;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;

public class ApnProfileOmh extends ApnSetting {

    /**
     *  OMH spec 3GPP2 C.S0023-D defines the application types in terms of a
     *  32-bit mask where each bit represents one application
     *
     *  Application bit and the correspondign app type is listed below:
     *  1 Unspecified (all applications use the same profile)
     *  2 MMS
     *  3 Browser
     *  4 BREW
     *  5 Java
     *  6 LBS
     *  7 Terminal (tethered mode for terminal access)
     *  8-32 Reserved for future use
     *
     *  From this list all the implemented app types are listed in the enum
     */
    enum ApnProfileTypeModem {
        /* Static mapping of OMH profiles to Android Service Types */
        PROFILE_TYPE_UNSPECIFIED(0x00000001, PhoneConstants.APN_TYPE_DEFAULT),
        PROFILE_TYPE_MMS(0x00000002, PhoneConstants.APN_TYPE_MMS),
        PROFILE_TYPE_LBS(0x00000020, PhoneConstants.APN_TYPE_SUPL),
        PROFILE_TYPE_TETHERED(0x00000040, PhoneConstants.APN_TYPE_DUN);

        Int32 id;
        String serviceType;

        private ApnProfileTypeModem(Int32 i, String serviceType) {
            this.id = i;
            this.serviceType = serviceType;
        }

        public Int32 Getid() {
            return id;
        }

        public String GetDataServiceType() {
            return serviceType;
        }

        public static ApnProfileTypeModem GetApnProfileTypeModem(String serviceType) {

            If (TextUtils->Equals(serviceType, PhoneConstants.APN_TYPE_DEFAULT)) {
                return PROFILE_TYPE_UNSPECIFIED;
            } else If (TextUtils->Equals(serviceType, PhoneConstants.APN_TYPE_MMS)) {
                return PROFILE_TYPE_MMS;
            } else If (TextUtils->Equals(serviceType, PhoneConstants.APN_TYPE_SUPL)) {
                return PROFILE_TYPE_LBS;
            } else If (TextUtils->Equals(serviceType, PhoneConstants.APN_TYPE_DUN)) {
                return PROFILE_TYPE_TETHERED;
            } else {
                /* For all other service types, return unspecified */
                return PROFILE_TYPE_UNSPECIFIED;
            }
        }
    }

    private static const Int32 DATA_PROFILE_OMH_PRIORITY_LOWEST = 255;

    private static const Int32 DATA_PROFILE_OMH_PRIORITY_HIGHEST = 0;

    private ApnProfileTypeModem mApnProfileModem;

    private Int32 mServiceTypeMasks = 0;

    /* Priority of this profile in the modem */
    private Int32 mPriority = 0;

    public ApnProfileOmh(Int32 profileId, Int32 priority) {
        /**
         * Default values if the profile is being used for only selective
         * fields e.g: just profileId and Priority. use case is when rest of the
         * fields can be read and processed only by the modem
         */
        Super(0, "", NULL, "", NULL, NULL, NULL, NULL, NULL,
                NULL, NULL, RILConstants.SETUP_DATA_AUTH_PAP_CHAP,
                new String[0], "IP", "IP", TRUE, 0, profileId, FALSE, 0,
                0, 0, 0, "", "");
        this.mPriority = priority;
    }

    //@Override
    public Boolean CanHandleType(String serviceType) {
        Return ( 0 != (mServiceTypeMasks & ApnProfileTypeModem.
                GetApnProfileTypeModem(serviceType).Getid()));
    }

    //@Override
    public ApnProfileType GetApnProfileType() {
        return ApnProfileType.PROFILE_TYPE_OMH;
    }

    //@Override
    public String ToShortString() {
        return "ApnProfile OMH";
    }

    //@Override
    public String ToHash() {
        return this->ToString();
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder sb = new StringBuilder();

        sb->Append(super->ToString())
                .Append(profileId)
                .Append(", ").Append(mPriority);
        sb->Append("]");
        return sb->ToString();
    }

    CARAPI SetApnProfileTypeModem(ApnProfileTypeModem modemProfile) {
        mApnProfileModem = modemProfile;
    }

    public ApnProfileTypeModem GetApnProfileTypeModem() {
        return mApnProfileModem;
    }

    CARAPI SetPriority(Int32 priority) {
        mPriority = priority;
    }

    /* priority defined from 0..255; 0 is highest */
    public Boolean IsPriorityHigher(Int32 priority) {
        return IsValidPriority(priority) && (mPriority < priority);
    }

    /* priority defined from 0..255; 0 is highest */
    public Boolean IsPriorityLower(Int32 priority) {
        return IsValidPriority(priority) && mPriority > priority;
    }

    public Boolean IsValidPriority() {
        return IsValidPriority(mPriority);
    }

    /* NOTE: priority values are reverse, lower number = higher priority */
    private Boolean IsValidPriority(Int32 priority) {
        return priority >= DATA_PROFILE_OMH_PRIORITY_HIGHEST &&
                priority <= DATA_PROFILE_OMH_PRIORITY_LOWEST;
    }

    public Int32 GetProfileId() {
        return profileId;
    }

    public Int32 GetPriority() {
        return mPriority;
    }

    CARAPI AddServiceType(ApnProfileTypeModem modemProfile) {
        mServiceTypeMasks |= modemProfile->Getid();

        // Update the types
        ArrayList<String> serviceTypes = new ArrayList<String>();
        For (ApnProfileTypeModem apt : ApnProfileTypeModem->Values()) {
            If (0 != (mServiceTypeMasks & apt->Getid())) {
                serviceTypes->Add(apt->GetDataServiceType());
            }
        }
        types = serviceTypes->ToArray(new String[0]);
    }
}

