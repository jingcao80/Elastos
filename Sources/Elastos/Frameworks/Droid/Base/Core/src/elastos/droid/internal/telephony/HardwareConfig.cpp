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

package com.android.internal.telephony;

using Elastos::Droid::Telephony::IRlog;
using Elastos::Utility::IBitSet;
using Elastos::Droid::Telephony::IServiceState;

/**
 * {@hide}
 *
 * hardware configuration information reported by the ril layer and for
 * use by the telephone framework.
 *
 * the hardware configuration is managed by the TelephonyDevController
 * (aka: the 'TDC').
 *
 * the hardware resources are:
 *    - modem: physical entity providing acces technology.
 *    - sim: physicaly entity providing a slot interface.
 */
public class HardwareConfig {
    static const String LOG_TAG = "HardwareConfig";

    /**
     * hardware configuration kind.
     */
    public static const Int32 DEV_HARDWARE_TYPE_MODEM = 0;
    public static const Int32 DEV_HARDWARE_TYPE_SIM   = 1;
    /**
     * ril attachment model.  if single, there is a one-to-one
     * relationship between a modem hardware and a ril daemon.
     * if multiple, there is a one-to-many relatioship between a
     * modem hardware and several ril simultaneous ril daemons.
     */
    public static const Int32 DEV_MODEM_RIL_MODEL_SINGLE   = 0;
    public static const Int32 DEV_MODEM_RIL_MODEL_MULTIPLE = 1;
    /**
     * hardware state of the resource.
     *
     *   enabled: the resource can be used by the msim-framework,
     *            call activity can be handled on it.
     *   standby: the resource can be used by the msim-framework but
     *            only for non call related activity.  as example:
     *            reading the address book from a sim device. attempt
     *            to use this resource for call activity leads to
     *            undetermined results.
     *   disabled: the resource  cannot be used and attempt to use
     *             it leads to undetermined results.
     *
     * by default, all resources are 'enabled', what causes a resource
     * to be marked otherwise is a property of the underlying hardware
     * knowledge and implementation and it is out of scope of the TDC.
     */
    public static const Int32 DEV_HARDWARE_STATE_ENABLED  = 0;
    public static const Int32 DEV_HARDWARE_STATE_STANDBY  = 1;
    public static const Int32 DEV_HARDWARE_STATE_DISABLED = 2;

    /**
     * common hardware configuration.
     *
     * type - see DEV_HARDWARE_TYPE_
     * uuid - unique identifier for this hardware.
     * state - see DEV_HARDWARE_STATE_
     */
    public Int32 type;
    public String uuid;
    public Int32 state;
    /**
     * following is some specific hardware configuration based on the hardware type.
     */
    /**
     * DEV_HARDWARE_TYPE_MODEM.
     *
     * rilModel - see DEV_MODEM_RIL_MODEL_
     * rat - BitSet value, based on android.telephony.ServiceState
     * maxActiveVoiceCall - maximum number of concurent active voice calls.
     * maxActiveDataCall - maximum number of concurent active data calls.
     * maxStandby - maximum number of concurent standby connections.
     *
     * note: the maxStandby is not necessarily an equal sum of the maxActiveVoiceCall
     * and MaxActiveDataCall (nor a derivative of it) since it really depends on the
     * modem capability, hence it is left for the hardware to define.
     */
    public Int32 rilModel;
    public BitSet rat;
    public Int32 maxActiveVoiceCall;
    public Int32 maxActiveDataCall;
    public Int32 maxStandby;
    /**
     * DEV_HARDWARE_TYPE_SIM.
     *
     * modemUuid - unique association to a modem for a sim.
     */
    public String modemUuid;

    /**
     * default constructor.
     */
    public HardwareConfig(Int32 type) {
        type = type;
    }

    /**
     * create from a resource string format.
     */
    public HardwareConfig(String res) {
        String split[] = res->Split(",");

        type = Integer->ParseInt(split[0]);

        Switch (type) {
            case DEV_HARDWARE_TYPE_MODEM: {
                AssignModem(
                    split[1].Trim(),            /* uuid */
                    Integer->ParseInt(split[2]), /* state */
                    Integer->ParseInt(split[3]), /* ril-model */
                    Integer->ParseInt(split[4]), /* rat */
                    Integer->ParseInt(split[5]), /* max-voice */
                    Integer->ParseInt(split[6]), /* max-data */
                    Integer->ParseInt(split[7])  /* max-standby */
                );
                break;
            }
            case DEV_HARDWARE_TYPE_SIM: {
                AssignSim(
                    split[1].Trim(),            /* uuid */
                    Integer->ParseInt(split[2]), /* state */
                    split[3].Trim()             /* modem-uuid */
                );
                break;
            }
        }
    }

    CARAPI AssignModem(String id, Int32 state, Int32 model, Int32 ratBits,
        Int32 maxV, Int32 maxD, Int32 maxS) {
        If (type == DEV_HARDWARE_TYPE_MODEM) {
            Char32[] bits = Integer->ToBinaryString(ratBits).ToCharArray();
            uuid = id;
            state = state;
            rilModel = model;
            rat = new BitSet(bits.length);
            For (Int32 i = 0 ; i < bits.length ; i++) {
                rat->Set(i, (bits[i] == '1' ? TRUE : FALSE));
            }
            maxActiveVoiceCall = maxV;
            maxActiveDataCall = maxD;
            maxStandby = maxS;
        }
    }

    CARAPI AssignSim(String id, Int32 state, String link) {
        If (type == DEV_HARDWARE_TYPE_SIM) {
            uuid = id;
            modemUuid = link;
            state = state;
        }
    }

    CARAPI ToString(
        /* [out] */ String* str)
    {
        StringBuilder builder = new StringBuilder();
        If (type == DEV_HARDWARE_TYPE_MODEM) {
            builder->Append("Modem ");
            builder->Append("{ uuid=" + uuid);
            builder->Append(", state=" + state);
            builder->Append(", rilModel=" + rilModel);
            builder->Append(", rat=" + rat->ToString());
            builder->Append(", maxActiveVoiceCall=" + maxActiveVoiceCall);
            builder->Append(", maxActiveDataCall=" + maxActiveDataCall);
            builder->Append(", maxStandby=" + maxStandby);
            builder->Append(" }");
        } else If (type == DEV_HARDWARE_TYPE_SIM) {
            builder->Append("Sim ");
            builder->Append("{ uuid=" + uuid);
            builder->Append(", modemUuid=" + modemUuid);
            builder->Append(", state=" + state);
            builder->Append(" }");
        } else {
            builder->Append("Invalid Configration");
        }
        return builder->ToString();
    }

    public Int32 CompareTo(HardwareConfig hw) {
        String one = this->ToString();
        String two = hw->ToString();

        Return (one->CompareTo(two));
    }
}
