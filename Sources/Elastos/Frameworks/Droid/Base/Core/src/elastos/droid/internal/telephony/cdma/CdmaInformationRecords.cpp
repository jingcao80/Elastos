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

package com.android.internal.telephony.cdma;

using Elastos::Droid::Os::IParcel;

public class CdmaInformationRecords {
    public Object record;

    /**
     * Record type identifier
     */
    public static const Int32 RIL_CDMA_DISPLAY_INFO_REC = 0;
    public static const Int32 RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC = 1;
    public static const Int32 RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC = 2;
    public static const Int32 RIL_CDMA_CONNECTED_NUMBER_INFO_REC = 3;
    public static const Int32 RIL_CDMA_SIGNAL_INFO_REC = 4;
    public static const Int32 RIL_CDMA_REDIRECTING_NUMBER_INFO_REC = 5;
    public static const Int32 RIL_CDMA_LINE_CONTROL_INFO_REC = 6;
    public static const Int32 RIL_CDMA_EXTENDED_DISPLAY_INFO_REC = 7;
    public static const Int32 RIL_CDMA_T53_CLIR_INFO_REC = 8;
    public static const Int32 RIL_CDMA_T53_RELEASE_INFO_REC = 9;
    public static const Int32 RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC = 10;

    public CdmaInformationRecords(Parcel p) {
        Int32 id = p->ReadInt();
        Switch (id) {
            case RIL_CDMA_DISPLAY_INFO_REC:
            case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC:
                record  = new CdmaDisplayInfoRec(id, p->ReadString());
                break;

            case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
            case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
            case RIL_CDMA_CONNECTED_NUMBER_INFO_REC:
                record = new CdmaNumberInfoRec(id, p->ReadString(), p->ReadInt(), p->ReadInt(),
                        p->ReadInt(), p->ReadInt());
                break;

            case RIL_CDMA_SIGNAL_INFO_REC:
                record = new CdmaSignalInfoRec(p->ReadInt(), p->ReadInt(), p->ReadInt(), p->ReadInt());
                break;

            case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
                record = new CdmaRedirectingNumberInfoRec(p->ReadString(), p->ReadInt(), p->ReadInt(),
                        p->ReadInt(), p->ReadInt(), p->ReadInt());
                break;

            case RIL_CDMA_LINE_CONTROL_INFO_REC:
                record = new CdmaLineControlInfoRec(p->ReadInt(), p->ReadInt(), p->ReadInt(),
                        p->ReadInt());
                break;

            case RIL_CDMA_T53_CLIR_INFO_REC:
                record = new CdmaT53ClirInfoRec(p->ReadInt());
                break;

            case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC:
                record = new CdmaT53AudioControlInfoRec(p->ReadInt(), p->ReadInt());
                break;

            case RIL_CDMA_T53_RELEASE_INFO_REC:
                // TODO: WHAT to do, for now fall through and throw exception
            default:
                throw new RuntimeException("RIL_UNSOL_CDMA_INFO_REC: unsupported record. Got "
                                            + CdmaInformationRecords->IdToString(id) + " ");

        }
    }

    public static String IdToString(Int32 id) {
        Switch(id) {
        case RIL_CDMA_DISPLAY_INFO_REC: return "RIL_CDMA_DISPLAY_INFO_REC";
        case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC: return "RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC";
        case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC: return "RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC";
        case RIL_CDMA_CONNECTED_NUMBER_INFO_REC: return "RIL_CDMA_CONNECTED_NUMBER_INFO_REC";
        case RIL_CDMA_SIGNAL_INFO_REC: return "RIL_CDMA_SIGNAL_INFO_REC";
        case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC: return "RIL_CDMA_REDIRECTING_NUMBER_INFO_REC";
        case RIL_CDMA_LINE_CONTROL_INFO_REC: return "RIL_CDMA_LINE_CONTROL_INFO_REC";
        case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC: return "RIL_CDMA_EXTENDED_DISPLAY_INFO_REC";
        case RIL_CDMA_T53_CLIR_INFO_REC: return "RIL_CDMA_T53_CLIR_INFO_REC";
        case RIL_CDMA_T53_RELEASE_INFO_REC: return "RIL_CDMA_T53_RELEASE_INFO_REC";
        case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC: return "RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC";
        default: return "<unknown record>";
        }
    }

    /**
     * Signal Information record from 3GPP2 C.S005 3.7.5.5
     */
    public static class CdmaSignalInfoRec {
        public Boolean isPresent;   /* non-zero if signal information record is present */
        public Int32 signalType;
        public Int32 alertPitch;
        public Int32 signal;

        public CdmaSignalInfoRec() {}

        public CdmaSignalInfoRec(Int32 isPresent, Int32 signalType, Int32 alertPitch, Int32 signal) {
            this.isPresent = isPresent != 0;
            this.signalType = signalType;
            this.alertPitch = alertPitch;
            this.signal = signal;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "CdmaSignalInfo: {" +
                    " isPresent: " + isPresent +
                    ", signalType: " + signalType +
                    ", alertPitch: " + alertPitch +
                    ", signal: " + signal +
                    " }";
        }
    }

    public static class CdmaDisplayInfoRec {
        public Int32 id;
        public String alpha;

        public CdmaDisplayInfoRec(Int32 id, String alpha) {
            this.id = id;
            this.alpha = alpha;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "CdmaDisplayInfoRec: {" +
                    " id: " + CdmaInformationRecords->IdToString(id) +
                    ", alpha: " + alpha +
                    " }";
        }
    }

    public static class CdmaNumberInfoRec {
        public Int32 id;
        public String number;
        public Byte numberType;
        public Byte numberPlan;
        public Byte pi;
        public Byte si;

        public CdmaNumberInfoRec(Int32 id, String number, Int32 numberType, Int32 numberPlan, Int32 pi,
                Int32 si) {
            this.number = number;
            this.numberType = (Byte)numberType;
            this.numberPlan = (Byte)numberPlan;
            this.pi = (Byte)pi;
            this.si = (Byte)si;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "CdmaNumberInfoRec: {" +
                    " id: " + CdmaInformationRecords->IdToString(id) +
                    ", number: " + number +
                    ", numberType: " + numberType +
                    ", numberPlan: " + numberPlan +
                    ", pi: " + pi +
                    ", si: " + si +
                    " }";
        }
    }

    public static class CdmaRedirectingNumberInfoRec {
        public static const Int32 REASON_UNKNOWN = 0;
        public static const Int32 REASON_CALL_FORWARDING_BUSY = 1;
        public static const Int32 REASON_CALL_FORWARDING_NO_REPLY = 2;
        public static const Int32 REASON_CALLED_DTE_OUT_OF_ORDER = 9;
        public static const Int32 REASON_CALL_FORWARDING_BY_THE_CALLED_DTE = 10;
        public static const Int32 REASON_CALL_FORWARDING_UNCONDITIONAL = 15;

        public CdmaNumberInfoRec numberInfoRec;
        public Int32 redirectingReason;

        public CdmaRedirectingNumberInfoRec(String number, Int32 numberType, Int32 numberPlan,
                Int32 pi, Int32 si, Int32 reason) {
            numberInfoRec = new CdmaNumberInfoRec(RIL_CDMA_REDIRECTING_NUMBER_INFO_REC,
                                                  number, numberType, numberPlan, pi, si);
            redirectingReason = reason;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "CdmaNumberInfoRec: {" +
                    " numberInfoRec: " + numberInfoRec +
                    ", redirectingReason: " + redirectingReason +
                    " }";
        }
    }

    public static class CdmaLineControlInfoRec {
        public Byte lineCtrlPolarityIncluded;
        public Byte lineCtrlToggle;
        public Byte lineCtrlReverse;
        public Byte lineCtrlPowerDenial;

        public CdmaLineControlInfoRec(Int32 lineCtrlPolarityIncluded, Int32 lineCtrlToggle,
                Int32 lineCtrlReverse, Int32 lineCtrlPowerDenial) {
            this.lineCtrlPolarityIncluded = (Byte)lineCtrlPolarityIncluded;
            this.lineCtrlToggle = (Byte)lineCtrlToggle;
            this.lineCtrlReverse = (Byte)lineCtrlReverse;
            this.lineCtrlPowerDenial = (Byte)lineCtrlPowerDenial;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "CdmaLineControlInfoRec: {" +
                    " lineCtrlPolarityIncluded: " + lineCtrlPolarityIncluded +
                    " lineCtrlToggle: " + lineCtrlToggle +
                    " lineCtrlReverse: " + lineCtrlReverse +
                    " lineCtrlPowerDenial: " + lineCtrlPowerDenial +
                    " }";
        }
    }

    public static class CdmaT53ClirInfoRec {
        public Byte cause;

        public CdmaT53ClirInfoRec(Int32 cause) {
            this.cause = (Byte)cause;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "CdmaT53ClirInfoRec: {" +
                    " cause: " + cause +
                    " }";
        }
    }

    public static class CdmaT53AudioControlInfoRec {
        public Byte uplink;
        public Byte downlink;

        public CdmaT53AudioControlInfoRec(Int32 uplink, Int32 downlink) {
            this.uplink = (Byte) uplink;
            this.downlink = (Byte) downlink;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "CdmaT53AudioControlInfoRec: {" +
                    " uplink: " + uplink +
                    " downlink: " + downlink +
                    " }";
        }
    }
}
