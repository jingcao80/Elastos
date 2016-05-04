/*
 * Copyright (C) 2014 The Android Open Source Project
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

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Telephony::IServiceState;

public class DataProfile {

    static const Int32 TYPE_COMMON = 0;
    static const Int32 TYPE_3GPP = 1;
    static const Int32 TYPE_3GPP2 = 2;

    //id of the data profile
    public final Int32 profileId;
    //the APN to connect to
    public final String apn;
    //one of the PDP_type values in TS 27.007 section 10.1.1.
    //For example, "IP", "IPV6", "IPV4V6", or "PPP".
    public final String protocol;
    //authentication protocol used for this PDP context
    //(None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3)
    public final Int32 authType;
    //the username for APN, or NULL
    public final String user;
    //the password for APN, or NULL
    public final String password;
    //the profile type, TYPE_COMMON, TYPE_3GPP, TYPE_3GPP2
    public final Int32 type;
    //the period in seconds to limit the maximum connections
    public final Int32 maxConnsTime;
    //the maximum connections during maxConnsTime
    public final Int32 maxConns;
    //the required wait time in seconds after a successful UE initiated
    //disconnect of a given PDN connection before the device can send
    //a new PDN connection request for that given PDN
    public final Int32 waitTime;
    //TRUE to enable the profile, FALSE to disable
    public final Boolean enabled;


    DataProfile(Int32 profileId, String apn, String protocol, Int32 authType,
            String user, String password, Int32 type, Int32 maxConnsTime, Int32 maxConns,
            Int32 waitTime, Boolean enabled) {

        this.profileId = profileId;
        this.apn = apn;
        this.protocol = protocol;
        this.authType = authType;
        this.user = user;
        this.password = password;
        this.type = type;
        this.maxConnsTime = maxConnsTime;
        this.maxConns = maxConns;
        this.waitTime = waitTime;
        this.enabled = enabled;
    }

    DataProfile(ApnSetting apn, Boolean isRoaming) {
        This(apn.profileId, apn.apn, isRoaming? apn.protocol : apn.roamingProtocol,
                apn.authType, apn.user, apn.password, apn.bearer == 0 ? TYPE_COMMON :
                (ServiceState->IsCdma(apn.bearer) ? TYPE_3GPP2 : TYPE_3GPP), apn.maxConnsTime,
                apn.maxConns, apn.waitTime, apn.carrierEnabled);
    }

    public static Parcel ToParcel(Parcel pc, DataProfile[] dps) {

        If(pc == NULL) {
            return NULL;
        }

        pc->WriteInt(dps.length);
        For(Int32 i = 0; i < dps.length; i++) {
            pc->WriteInt(dps[i].profileId);
            pc->WriteString(dps[i].apn);
            pc->WriteString(dps[i].protocol);
            pc->WriteInt(dps[i].authType);
            pc->WriteString(dps[i].user);
            pc->WriteString(dps[i].password);
            pc->WriteInt(dps[i].type);
            pc->WriteInt(dps[i].maxConnsTime);
            pc->WriteInt(dps[i].maxConns);
            pc->WriteInt(dps[i].waitTime);
            pc->WriteInt(dps[i].enabled ? 1 : 0);
        }
        return pc;
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "DataProfile " + profileId + "/" + apn + "/" + protocol + "/" + authType
                + "/" + user + "/" + password + "/" + type + "/" + maxConnsTime
                + "/" + maxConns + "/" + waitTime + "/" + enabled;
    }

    //@Override
    public Boolean Equals(Object o) {
        If (o instanceof DataProfile == FALSE) return FALSE;
        Return (ToString()->Equals(o->ToString()));
    }
}
