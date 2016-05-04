/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * Not a Contribution.
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

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;

/**
 * This class represents a apn setting for create PDP link
 */
public class ApnSetting {

    static const String V2_FORMAT_REGEX = "^\\[ApnSettingV2\\]\\s*";
    static const String V3_FORMAT_REGEX = "^\\[ApnSettingV3\\]\\s*";

    public final String carrier;
    public final String apn;
    public final String proxy;
    public final String port;
    public final String mmsc;
    public final String mmsProxy;
    public final String mmsPort;
    public final String user;
    public final String password;
    public final Int32 authType;
    public String[] types;
    public final Int32 id;
    public final String numeric;
    public final String protocol;
    public final String roamingProtocol;
    public final Int32 mtu;

    /**
      * Current status of APN
      * TRUE : enabled APN, FALSE : disabled APN.
      */
    public final Boolean carrierEnabled;
    /**
      * Radio Access Technology info
      * To check what values can hold, refer to ServiceState.java.
      * This should be spread to other technologies,
      * but currently only used for LTE(14) and EHRPD(13).
      */
    public final Int32 bearer;

    /* ID of the profile in the modem */
    public Int32 profileId;
    public final Boolean modemCognitive;
    public final Int32 maxConns;
    public final Int32 waitTime;
    public final Int32 maxConnsTime;

    /**
      * MVNO match type. Possible values:
      *   "spn": Service provider name.
      *   "imsi": IMSI.
      *   "gid": Group identifier level 1.
      */
    public final String mvnoType;
    /**
      * MVNO data. Examples:
      *   "spn": A MOBILE, BEN NL
      *   "imsi": 302720x94, 2060188
      *   "gid": 4E, 33
      */
    public final String mvnoMatchData;

    public enum ApnProfileType {
        PROFILE_TYPE_APN(0),
        PROFILE_TYPE_CDMA(1),
        PROFILE_TYPE_OMH(2);

        Int32 id;

        private ApnProfileType(Int32 i) {
            this.id = i;
        }

        public Int32 Getid() {
            return id;
        }
    }

    public ApnSetting(Int32 id, String numeric, String carrier, String apn,
            String proxy, String port,
            String mmsc, String mmsProxy, String mmsPort,
            String user, String password, Int32 authType, String[] types,
            String protocol, String roamingProtocol, Boolean carrierEnabled, Int32 bearer,
            Int32 profileId, Boolean modemCognitive, Int32 maxConns, Int32 waitTime, Int32 maxConnsTime,
            Int32 mtu, String mvnoType, String mvnoMatchData) {
        this.id = id;
        this.numeric = numeric;
        this.carrier = carrier;
        this.apn = apn;
        this.proxy = proxy;
        this.port = port;
        this.mmsc = mmsc;
        this.mmsProxy = mmsProxy;
        this.mmsPort = mmsPort;
        this.user = user;
        this.password = password;
        this.authType = authType;
        this.types = new String[types.length];
        For (Int32 i = 0; i < types.length; i++) {
            this.types[i] = types[i].ToLowerCase(Locale.ROOT);
        }
        this.protocol = protocol;
        this.roamingProtocol = roamingProtocol;
        this.carrierEnabled = carrierEnabled;
        this.bearer = bearer;
        this.profileId = profileId;
        this.modemCognitive = modemCognitive;
        this.maxConns = maxConns;
        this.waitTime = waitTime;
        this.maxConnsTime = maxConnsTime;
        this.mtu = mtu;
        this.mvnoType = mvnoType;
        this.mvnoMatchData = mvnoMatchData;

    }

    /**
     * Creates an ApnSetting object from a string.
     *
     * @param data the string to read.
     *
     * The string must be in one of two Formats (newlines added for clarity,
     * spaces are optional):
     *
     * v1 format:
     *   <carrier>, <apn>, <proxy>, <port>, <user>, <password>, <server>,
     *   <mmsc>, <mmsproxy>, <mmsport>, <mcc>, <mnc>, <authtype>,
     *   <type>[| <type>...],
     *
     * v2 format:
     *   [ApnSettingV2] <carrier>, <apn>, <proxy>, <port>, <user>, <password>, <server>,
     *   <mmsc>, <mmsproxy>, <mmsport>, <mcc>, <mnc>, <authtype>,
     *   <type>[| <type>...], <protocol>, <roaming_protocol>, <carrierEnabled>, <bearer>,
     *
     * v3 format:
     *   [ApnSettingV3] <carrier>, <apn>, <proxy>, <port>, <user>, <password>, <server>,
     *   <mmsc>, <mmsproxy>, <mmsport>, <mcc>, <mnc>, <authtype>,
     *   <type>[| <type>...], <protocol>, <roaming_protocol>, <carrierEnabled>, <bearer>,
     *   <profileId>, <modemCognitive>, <maxConns>, <waitTime>, <maxConnsTime>, <mtu>,
     *   <mvnoType>, <mvnoMatchData>
     *
     * Note that the strings generated by ToString() do not contain the username
     * and password and thus cannot be read by this method.
     */
    public static ApnSetting FromString(String data) {
        If (data == NULL) return NULL;

        Int32 version;
        // Matches() operates on the whole string, so append .* to the regex.
        If (data->Matches(V3_FORMAT_REGEX + ".*")) {
            version = 3;
            data = data->ReplaceFirst(V3_FORMAT_REGEX, "");
        } else If (data->Matches(V2_FORMAT_REGEX + ".*")) {
            version = 2;
            data = data->ReplaceFirst(V2_FORMAT_REGEX, "");
        } else {
            version = 1;
        }

        String[] a = data->Split("\\s*,\\s*");
        If (a.length < 14) {
            return NULL;
        }

        Int32 authType;
        try {
            authType = Integer->ParseInt(a[12]);
        } Catch (NumberFormatException e) {
            authType = 0;
        }

        String[] typeArray;
        String protocol, roamingProtocol;
        Boolean carrierEnabled;
        Int32 bearer = 0;
        Int32 profileId = 0;
        Boolean modemCognitive = FALSE;
        Int32 maxConns = 0;
        Int32 waitTime = 0;
        Int32 maxConnsTime = 0;
        Int32 mtu = PhoneConstants.UNSET_MTU;
        String mvnoType = "";
        String mvnoMatchData = "";
        If (version == 1) {
            typeArray = new String[a.length - 13];
            System->Arraycopy(a, 13, typeArray, 0, a.length - 13);
            protocol = RILConstants.SETUP_DATA_PROTOCOL_IP;
            roamingProtocol = RILConstants.SETUP_DATA_PROTOCOL_IP;
            carrierEnabled = TRUE;
            bearer = 0;
        } else {
            If (a.length < 18) {
                return NULL;
            }
            typeArray = a[13].Split("\\s*\\|\\s*");
            protocol = a[14];
            roamingProtocol = a[15];
            carrierEnabled = Boolean->ParseBoolean(a[16]);

            try {
                bearer = Integer->ParseInt(a[17]);
            } Catch (NumberFormatException ex) {
            }

            If (a.length > 22) {
                modemCognitive = Boolean->ParseBoolean(a[19]);
                try {
                    profileId = Integer->ParseInt(a[18]);
                    maxConns = Integer->ParseInt(a[20]);
                    waitTime = Integer->ParseInt(a[21]);
                    maxConnsTime = Integer->ParseInt(a[22]);
                } Catch (NumberFormatException e) {
                }
            }
            If (a.length > 23) {
                try {
                    mtu = Integer->ParseInt(a[23]);
                } Catch (NumberFormatException e) {
                }
            }
            If (a.length > 25) {
                mvnoType = a[24];
                mvnoMatchData = a[25];
            }
        }

        return new ApnSetting(-1,a[10]+a[11],a[0],a[1],a[2],a[3],a[7],a[8],
                a[9],a[4],a[5],authType,typeArray,protocol,roamingProtocol,carrierEnabled,bearer,
                profileId, modemCognitive, maxConns, waitTime, maxConnsTime, mtu,
                mvnoType, mvnoMatchData);
    }

    /**
     * Creates an array of ApnSetting objects from a string.
     *
     * @param data the string to read.
     *
     * Builds on top of the same format used by fromString, but allows for multiple entries
     * separated by "; ".
     */
    public static List<ApnSetting> ArrayFromString(String data) {
        List<ApnSetting> retVal = new ArrayList<ApnSetting>();
        If (TextUtils->IsEmpty(data)) {
            return retVal;
        }
        String[] apnStrings = data->Split("\\s*;\\s*");
        For (String apnString : apnStrings) {
            ApnSetting apn = FromString(apnString);
            If (apn != NULL) {
                retVal->Add(apn);
            }
        }
        return retVal;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder sb = new StringBuilder();
        sb->Append("[ApnSettingV3] ")
        .Append(carrier)
        .Append(", ").Append(id)
        .Append(", ").Append(numeric)
        .Append(", ").Append(apn)
        .Append(", ").Append(proxy)
        .Append(", ").Append(mmsc)
        .Append(", ").Append(mmsProxy)
        .Append(", ").Append(mmsPort)
        .Append(", ").Append(port)
        .Append(", ").Append(authType).Append(", ");
        For (Int32 i = 0; i < types.length; i++) {
            sb->Append(types[i]);
            If (i < types.length - 1) {
                sb->Append(" | ");
            }
        }
        sb->Append(", ").Append(protocol);
        sb->Append(", ").Append(roamingProtocol);
        sb->Append(", ").Append(carrierEnabled);
        sb->Append(", ").Append(bearer);
        sb->Append(", ").Append(profileId);
        sb->Append(", ").Append(modemCognitive);
        sb->Append(", ").Append(maxConns);
        sb->Append(", ").Append(waitTime);
        sb->Append(", ").Append(maxConnsTime);
        sb->Append(", ").Append(mtu);
        sb->Append(", ").Append(mvnoType);
        sb->Append(", ").Append(mvnoMatchData);
        return sb->ToString();
    }

    /**
     * Returns TRUE if there are MVNO params specified.
     */
    public Boolean HasMvnoParams() {
        return !TextUtils->IsEmpty(mvnoType) && !TextUtils->IsEmpty(mvnoMatchData);
    }

    public Boolean CanHandleType(String type) {
        If (!carrierEnabled) return FALSE;
        For (String t : types) {
            // DEFAULT handles all, and HIPRI is handled by DEFAULT
            If (t->EqualsIgnoreCase(type) ||
                    t->EqualsIgnoreCase(PhoneConstants.APN_TYPE_ALL) ||
                    (t->EqualsIgnoreCase(PhoneConstants.APN_TYPE_DEFAULT) &&
                    type->EqualsIgnoreCase(PhoneConstants.APN_TYPE_HIPRI))) {
                return TRUE;
            }
        }
        return FALSE;
    }

    // TODO - if we have this function we should also have hashCode.
    // Also should handle changes in type order and perhaps case-insensitivity
    //@Override
    public Boolean Equals(Object o) {
        If (o instanceof ApnSetting == FALSE) return FALSE;
        Return (ToString()->Equals(o->ToString()));
    }

    public ApnProfileType GetApnProfileType() {
        return ApnProfileType.PROFILE_TYPE_APN;
    }

    public Int32 GetProfileId() {
        return profileId;
    }

    public String ToShortString() {
        return "ApnSetting";
    }

    public String ToHash() {
        return this->ToString();
    }
}
