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

using Elastos::Droid::Telephony::IRlog;
using Elastos::Lang::IComparable;
using Elastos::Droid::Telephony::IPhoneNumberUtils;

/**
 * {@hide}
 */
public class DriverCall implements Comparable<DriverCall> {
    static const String LOG_TAG = "DriverCall";

    public enum State {
        ACTIVE,
        HOLDING,
        DIALING,    // MO call only
        ALERTING,   // MO call only
        INCOMING,   // MT call only
        WAITING;    // MT call only
        // If you add a state, make sure to look for the Switch()
        // statements that use this enum
    }

    public Int32 index;
    public Boolean isMT;
    public State state;     // May be NULL if unavail
    public Boolean isMpty;
    public String number;
    public Int32 TOA;
    public Boolean isVoice;
    public Boolean isVoicePrivacy;
    public Int32 als;
    public Int32 numberPresentation;
    public String name;
    public Int32 namePresentation;
    public UUSInfo uusInfo;

    /** returns NULL on error */
    static DriverCall
    FromCLCCLine(String line) {
        DriverCall ret = new DriverCall();

        //+CLCC: 1,0,2,0,0,\"+18005551212\",145
        //     index,isMT,state,mode,IsMpty(,number,TOA)?
        ATResponseParser p = new ATResponseParser(line);

        try {
            ret.index = p->NextInt();
            ret.isMT = p->NextBoolean();
            ret.state = StateFromCLCC(p->NextInt());

            ret.isVoice = (0 == p->NextInt());
            ret.isMpty = p->NextBoolean();

            // use ALLOWED as default presentation while parsing CLCC
            ret.numberPresentation = PhoneConstants.PRESENTATION_ALLOWED;

            If (p->HasMore()) {
                // Some lame implementations return strings
                // like "NOT AVAILABLE" in the CLCC line
                ret.number = PhoneNumberUtils->ExtractNetworkPortionAlt(p->NextString());

                If (ret.number->Length() == 0) {
                    ret.number = NULL;
                }

                ret.TOA = p->NextInt();

                // Make sure there's a leading + on addresses with a TOA
                // of 145

                ret.number = PhoneNumberUtils->StringFromStringAndTOA(
                                ret.number, ret.TOA);

            }
        } Catch (ATParseEx ex) {
            Rlog->E(LOG_TAG,"Invalid CLCC line: '" + line + "'");
            return NULL;
        }

        return ret;
    }

    public
    DriverCall() {
    }

    //@Override
    public String
    ToString() {
        return "id=" + index + ","
                + state + ","
                + "toa=" + TOA + ","
                + (isMpty ? "conf" : "norm") + ","
                + (isMT ? "mt" : "mo") + ","
                + als + ","
                + (isVoice ? "voc" : "nonvoc") + ","
                + (isVoicePrivacy ? "evp" : "noevp") + ","
                /*+ "number=" + number */ + ",cli=" + numberPresentation + ","
                /*+ "name="+ name */ + "," + namePresentation;
    }

    public static State
    StateFromCLCC(Int32 state) throws ATParseEx {
        Switch(state) {
            case 0: return State.ACTIVE;
            case 1: return State.HOLDING;
            case 2: return State.DIALING;
            case 3: return State.ALERTING;
            case 4: return State.INCOMING;
            case 5: return State.WAITING;
            default:
                throw new ATParseEx("illegal call state " + state);
        }
    }

    public static Int32
    PresentationFromCLIP(Int32 cli) throws ATParseEx
    {
        Switch(cli) {
            case 0: return PhoneConstants.PRESENTATION_ALLOWED;
            case 1: return PhoneConstants.PRESENTATION_RESTRICTED;
            case 2: return PhoneConstants.PRESENTATION_UNKNOWN;
            case 3: return PhoneConstants.PRESENTATION_PAYPHONE;
            default:
                throw new ATParseEx("illegal presentation " + cli);
        }
    }

    //***** Comparable Implementation

    /** For sorting by index */
    //@Override
    public Int32
    CompareTo(DriverCall dc) {

        If (index < dc.index) {
            return -1;
        } else If (index == dc.index) {
            return 0;
        } else { /*index > dc.index*/
            return 1;
        }
    }
}
