/*
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
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
 *
 */

package com.android.internal.telephony.gsm;

using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Internal::Telephony::ICallForwardInfo;

using Elastos::Utility::IArrayList;

/**
 * See also RIL_StkCcUnsolSsResponse in include/telephony/ril.h
 *
 * {@hide}
 */
public class SsData {
    public enum ServiceType {
        SS_CFU,
        SS_CF_BUSY,
        SS_CF_NO_REPLY,
        SS_CF_NOT_REACHABLE,
        SS_CF_ALL,
        SS_CF_ALL_CONDITIONAL,
        SS_CLIP,
        SS_CLIR,
        SS_COLP,
        SS_COLR,
        SS_WAIT,
        SS_BAOC,
        SS_BAOIC,
        SS_BAOIC_EXC_HOME,
        SS_BAIC,
        SS_BAIC_ROAMING,
        SS_ALL_BARRING,
        SS_OUTGOING_BARRING,
        SS_INCOMING_BARRING;

        public Boolean IsTypeCF() {
            Return (this == SS_CFU || this == SS_CF_BUSY || this == SS_CF_NO_REPLY ||
                  this == SS_CF_NOT_REACHABLE || this == SS_CF_ALL ||
                  this == SS_CF_ALL_CONDITIONAL);
        }

        public Boolean IsTypeUnConditional() {
            Return (this == SS_CFU || this == SS_CF_ALL);
        }

        public Boolean IsTypeCW() {
            Return (this == SS_WAIT);
        }

        public Boolean IsTypeClip() {
            Return (this == SS_CLIP);
        }

        public Boolean IsTypeClir() {
            Return (this == SS_CLIR);
        }

        public Boolean IsTypeBarring() {
            Return (this == SS_BAOC || this == SS_BAOIC || this == SS_BAOIC_EXC_HOME ||
                  this == SS_BAIC || this == SS_BAIC_ROAMING || this == SS_ALL_BARRING ||
                  this == SS_OUTGOING_BARRING || this == SS_INCOMING_BARRING);
        }
    };

    public enum RequestType {
        SS_ACTIVATION,
        SS_DEACTIVATION,
        SS_INTERROGATION,
        SS_REGISTRATION,
        SS_ERASURE;

        public Boolean IsTypeInterrogation() {
            Return (this == SS_INTERROGATION);
        }
    };

    public enum TeleserviceType {
        SS_ALL_TELE_AND_BEARER_SERVICES,
        SS_ALL_TELESEVICES,
        SS_TELEPHONY,
        SS_ALL_DATA_TELESERVICES,
        SS_SMS_SERVICES,
        SS_ALL_TELESERVICES_EXCEPT_SMS;
    };

    public ServiceType serviceType;
    public RequestType requestType;
    public TeleserviceType teleserviceType;
    public Int32 serviceClass;
    public Int32 result;

    public Int32[] ssInfo; /* This is the response data for most of the SS GET/SET
                            RIL requests. E.g. RIL_REQUSET_GET_CLIR returns
                            two ints, so first two values of ssInfo[] will be
                            used for respone if serviceType is SS_CLIR and
                            requestType is SS_INTERROGATION */

    public CallForwardInfo[] cfInfo; /* This is the response data for SS request
                                        to query call forward status. see
                                        RIL_REQUEST_QUERY_CALL_FORWARD_STATUS */

    public ServiceType ServiceTypeFromRILInt(Int32 type) {
        try {
            return ServiceType->Values()[type];
        } Catch (IndexOutOfBoundsException e) {
            Rlog->E(GSMPhone.LOG_TAG, "Invalid Service type");
            return NULL;
        }
    }

    public RequestType RequestTypeFromRILInt(Int32 type) {
        try {
            return RequestType->Values()[type];
        } Catch (IndexOutOfBoundsException e) {
            Rlog->E(GSMPhone.LOG_TAG, "Invalid Request type");
            return NULL;
        }
    }

    public TeleserviceType TeleserviceTypeFromRILInt(Int32 type) {
        try {
            return TeleserviceType->Values()[type];
        } Catch (IndexOutOfBoundsException e) {
            Rlog->E(GSMPhone.LOG_TAG, "Invalid Teleservice type");
            return NULL;
        }
    }

    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "[SsData] " + "ServiceType: " + serviceType
            + " RequestType: " + requestType
            + " TeleserviceType: " + teleserviceType
            + " ServiceClass: " + serviceClass
            + " Result: " + result
            + " Is Service Type CF: " + serviceType->IsTypeCF();
    }
}
