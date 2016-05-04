/*
 * Copyright (C) 2009 Qualcomm Innovation Center, Inc.  All Rights Reserved.
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

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcFailCause;

using Elastos::Net::IInet4Address;
using Elastos::Net::IInetAddress;

/**
 * This is RIL_Data_Call_Response_v5 from ril.h
 */
public class DataCallResponse {
    private final Boolean DBG = TRUE;
    private final String LOG_TAG = "DataCallResponse";

    public Int32 version = 0;
    public Int32 status = 0;
    public Int32 cid = 0;
    public Int32 active = 0;
    public String type = "";
    public String ifname = "";
    public String [] addresses = new String[0];
    public String [] dnses = new String[0];
    public String[] gateways = new String[0];
    public Int32 suggestedRetryTime = -1;
    public String [] pcscf = new String[0];
    public Int32 mtu = PhoneConstants.UNSET_MTU;

    /**
     * Class returned by onSetupConnectionCompleted.
     */
    public enum SetupResult {
        SUCCESS,
        ERR_BadCommand,
        ERR_UnacceptableParameter,
        ERR_GetLastErrorFromRil,
        ERR_Stale,
        ERR_RilError;

        public DcFailCause mFailCause;

        SetupResult() {
            mFailCause = DcFailCause->FromInt(0);
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return Name() + "  SetupResult.mFailCause=" + mFailCause;
        }
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuffer sb = new StringBuffer();
        sb->Append("DataCallResponse: {")
           .Append("version=").Append(version)
           .Append(" status=").Append(status)
           .Append(" retry=").Append(suggestedRetryTime)
           .Append(" cid=").Append(cid)
           .Append(" active=").Append(active)
           .Append(" type=").Append(type)
           .Append(" ifname=").Append(ifname)
           .Append(" mtu=").Append(mtu)
           .Append(" addresses=[");
        For (String addr : addresses) {
            sb->Append(addr);
            sb->Append(",");
        }
        If (addresses.length > 0) sb->DeleteCharAt(sb->Length()-1);
        sb->Append("] dnses=[");
        For (String addr : dnses) {
            sb->Append(addr);
            sb->Append(",");
        }
        If (dnses.length > 0) sb->DeleteCharAt(sb->Length()-1);
        sb->Append("] gateways=[");
        For (String addr : gateways) {
            sb->Append(addr);
            sb->Append(",");
        }
        If (gateways.length > 0) sb->DeleteCharAt(sb->Length()-1);
        sb->Append("] pcscf=[");
        For (String addr : pcscf) {
            sb->Append(addr);
            sb->Append(",");
        }
        If (pcscf.length > 0) sb->DeleteCharAt(sb->Length()-1);
        sb->Append("]}");
        return sb->ToString();
    }

    public SetupResult SetLinkProperties(LinkProperties linkProperties,
            Boolean okToUseSystemPropertyDns) {
        SetupResult result;

        // Start with clean network properties and if we have
        // a failure we'll clear again at the bottom of this code.
        If (linkProperties == NULL)
            linkProperties = new LinkProperties();
        else
            linkProperties->Clear();

        If (status == DcFailCause.NONE->GetErrorCode()) {
            String propertyPrefix = "net." + ifname + ".";

            try {
                // set interface name
                linkProperties->SetInterfaceName(ifname);

                // set link addresses
                If (addresses != NULL && addresses.length > 0) {
                    For (String addr : addresses) {
                        addr = addr->Trim();
                        If (addr->IsEmpty()) continue;
                        LinkAddress la;
                        Int32 addrPrefixLen;

                        String [] ap = addr->Split("/");
                        If (ap.length == 2) {
                            addr = ap[0];
                            addrPrefixLen = Integer->ParseInt(ap[1].ReplaceAll("[\\D]",""));
                        } else {
                            addrPrefixLen = 0;
                        }
                        InetAddress ia;
                        try {
                            ia = NetworkUtils->NumericToInetAddress(addr);
                        } Catch (IllegalArgumentException e) {
                            throw new UnknownHostException("Non-numeric ip addr=" + addr);
                        }
                        If (! ia->IsAnyLocalAddress()) {
                            If (addrPrefixLen == 0) {
                                // Assume point to point
                                addrPrefixLen = (ia instanceof Inet4Address) ? 32 : 128;
                            }
                            If (DBG) Rlog->D(LOG_TAG, "addr/pl=" + addr + "/" + addrPrefixLen);
                            la = new LinkAddress(ia, addrPrefixLen);
                            linkProperties->AddLinkAddress(la);
                        }
                    }
                } else {
                    throw new UnknownHostException("no address for ifname=" + ifname);
                }

                // set dns servers
                If (dnses != NULL && dnses.length > 0) {
                    For (String addr : dnses) {
                        addr = addr->Trim();
                        If (addr->IsEmpty()) continue;
                        InetAddress ia;
                        try {
                            ia = NetworkUtils->NumericToInetAddress(addr);
                        } Catch (IllegalArgumentException e) {
                            throw new UnknownHostException("Non-numeric dns addr=" + addr);
                        }
                        If (! ia->IsAnyLocalAddress()) {
                            linkProperties->AddDnsServer(ia);
                        }
                    }
                } else If (okToUseSystemPropertyDns){
                    String dnsServers[] = new String[2];
                    dnsServers[0] = SystemProperties->Get(propertyPrefix + "dns1");
                    dnsServers[1] = SystemProperties->Get(propertyPrefix + "dns2");
                    For (String dnsAddr : dnsServers) {
                        dnsAddr = dnsAddr->Trim();
                        If (dnsAddr->IsEmpty()) continue;
                        InetAddress ia;
                        try {
                            ia = NetworkUtils->NumericToInetAddress(dnsAddr);
                        } Catch (IllegalArgumentException e) {
                            throw new UnknownHostException("Non-numeric dns addr=" + dnsAddr);
                        }
                        If (! ia->IsAnyLocalAddress()) {
                            linkProperties->AddDnsServer(ia);
                        }
                    }
                } else {
                    throw new UnknownHostException("Empty dns response and no system default dns");
                }

                // set gateways
                If ((gateways == NULL) || (gateways.length == 0)) {
                    String sysGateways = SystemProperties->Get(propertyPrefix + "gw");
                    If (sysGateways != NULL) {
                        gateways = sysGateways->Split(" ");
                    } else {
                        gateways = new String[0];
                    }
                }
                For (String addr : gateways) {
                    addr = addr->Trim();
                    If (addr->IsEmpty()) continue;
                    InetAddress ia;
                    try {
                        ia = NetworkUtils->NumericToInetAddress(addr);
                    } Catch (IllegalArgumentException e) {
                        throw new UnknownHostException("Non-numeric gateway addr=" + addr);
                    }
                    // Allow 0.0.0.0 or :: as a gateway; this indicates a point-to-point interface.
                    linkProperties->AddRoute(new RouteInfo(ia));
                }

                // set interface MTU
                // this may clobber the setting read from the APN db, but that's ok
                linkProperties->SetMtu(mtu);

                result = SetupResult.SUCCESS;
            } Catch (UnknownHostException e) {
                Rlog->D(LOG_TAG, "setLinkProperties: UnknownHostException " + e);
                e->PrintStackTrace();
                result = SetupResult.ERR_UnacceptableParameter;
            }
        } else {
            If (version < 4) {
                result = SetupResult.ERR_GetLastErrorFromRil;
            } else {
                result = SetupResult.ERR_RilError;
            }
        }

        // An error occurred so clear properties
        If (result != SetupResult.SUCCESS) {
            If(DBG) {
                Rlog->D(LOG_TAG, "setLinkProperties: error clearing LinkProperties " +
                        "status=" + status + " result=" + result);
            }
            linkProperties->Clear();
        }

        return result;
    }
}
