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

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::I*;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Utility::IBitSet;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Telephony::IServiceState;

/**
 * TelephonyDevController - provides a unified view of the
 * telephony hardware resources on a device.
 *
 * manages the set of HardwareConfig for the framework.
 */
public class TelephonyDevController extends Handler {
    private static const String LOG_TAG = "TDC";
    private static const Boolean DBG = TRUE;
    private static const Object mLock = new Object();

    private static const Int32 EVENT_HARDWARE_CONFIG_CHANGED = 1;

    private static TelephonyDevController sTelephonyDevController;
    private static ArrayList<HardwareConfig> mModems = new ArrayList<HardwareConfig>();
    private static ArrayList<HardwareConfig> mSims = new ArrayList<HardwareConfig>();

    private static Message sRilHardwareConfig;

    private static void Logd(String s) {
        Rlog->D(LOG_TAG, s);
    }

    private static void Loge(String s) {
        Rlog->E(LOG_TAG, s);
    }

    public static TelephonyDevController Create() {
        Synchronized (mLock) {
            If (sTelephonyDevController != NULL) {
                throw new RuntimeException("TelephonyDevController already created!?!");
            }
            sTelephonyDevController = new TelephonyDevController();
            return sTelephonyDevController;
        }
    }

    public static TelephonyDevController GetInstance() {
        Synchronized (mLock) {
            If (sTelephonyDevController == NULL) {
                throw new RuntimeException("TelephonyDevController not yet created!?!");
            }
            return sTelephonyDevController;
        }
    }

    private void InitFromResource() {
        Resources resource = Resources->GetSystem();
        String[] hwStrings = resource->GetStringArray(
            R.array.config_telephonyHardware);
        If (hwStrings != NULL) {
            For (String hwString : hwStrings) {
                HardwareConfig hw = new HardwareConfig(hwString);
                If (hw != NULL) {
                    If (hw.type == HardwareConfig.DEV_HARDWARE_TYPE_MODEM) {
                        UpdateOrInsert(hw, mModems);
                    } else If (hw.type == HardwareConfig.DEV_HARDWARE_TYPE_SIM) {
                        UpdateOrInsert(hw, mSims);
                    }
                }
            }
        }
    }

    private TelephonyDevController() {
        InitFromResource();

        mModems->TrimToSize();
        mSims->TrimToSize();
    }

    /**
     * each RIL call this interface to register/unregister the unsolicited hardware
     * configuration callback data it can provide.
     */
    public static void RegisterRIL(CommandsInterface cmdsIf) {
        /* get the current configuration from this ril... */
        cmdsIf->GetHardwareConfig(sRilHardwareConfig);
        /* ... process it ... */
        If (sRilHardwareConfig != NULL) {
            AsyncResult ar = (AsyncResult) sRilHardwareConfig.obj;
            If (ar.exception == NULL) {
                HandleGetHardwareConfigChanged(ar);
            }
        }
        /* and register for async device configuration change. */
        cmdsIf->RegisterForHardwareConfigChanged(sTelephonyDevController, EVENT_HARDWARE_CONFIG_CHANGED, NULL);
    }

    public static void UnregisterRIL(CommandsInterface cmdsIf) {
        cmdsIf->UnregisterForHardwareConfigChanged(sTelephonyDevController);
    }

    /**
     * handle callbacks from RIL.
     */
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        Switch (msg.what) {
            case EVENT_HARDWARE_CONFIG_CHANGED:
                If (DBG) Logd("handleMessage: received EVENT_HARDWARE_CONFIG_CHANGED");
                ar = (AsyncResult) msg.obj;
                HandleGetHardwareConfigChanged(ar);
            break;
            default:
                Loge("handleMessage: Unknown Event " + msg.what);
        }
    }

    /**
     * hardware configuration update or insert.
     */
    private static void UpdateOrInsert(HardwareConfig hw, ArrayList<HardwareConfig> list) {
        Int32 size;
        HardwareConfig item;
        Synchronized (mLock) {
            size = list->Size();
            For (Int32 i = 0 ; i < size ; i++) {
                item = list->Get(i);
                If (item.uuid->CompareTo(hw.uuid) == 0) {
                    If (DBG) Logd("updateOrInsert: removing: " + item);
                    list->Remove(i);
                }
            }
            If (DBG) Logd("updateOrInsert: inserting: " + hw);
            list->Add(hw);
        }
    }

    /**
     * hardware configuration changed.
     */
    private static void HandleGetHardwareConfigChanged(AsyncResult ar) {
        If ((ar.exception == NULL) && (ar.result != NULL)) {
            List hwcfg = (List)ar.result;
            For (Int32 i = 0 ; i < hwcfg->Size() ; i++) {
                HardwareConfig hw = NULL;

                hw = (HardwareConfig) hwcfg->Get(i);
                If (hw != NULL) {
                    If (hw.type == HardwareConfig.DEV_HARDWARE_TYPE_MODEM) {
                        UpdateOrInsert(hw, mModems);
                    } else If (hw.type == HardwareConfig.DEV_HARDWARE_TYPE_SIM) {
                        UpdateOrInsert(hw, mSims);
                    }
                }
            }
        } else {
            /* error detected, ignore.  are we missing some real time configutation
             * at this point?  what to do...
             */
            Loge("handleGetHardwareConfigChanged - returned an error.");
        }
    }

    /**
     * get total number of registered modem.
     */
    public static Int32 GetModemCount() {
        Synchronized (mLock) {
            Int32 count = mModems->Size();
            If (DBG) Logd("getModemCount: " + count);
            return count;
        }
    }

    /**
     * get modem at index 'index'.
     */
    public HardwareConfig GetModem(Int32 index) {
        Synchronized (mLock) {
            If (mModems->IsEmpty()) {
                Loge("getModem: no registered modem device?!?");
                return NULL;
            }

            If (index > GetModemCount()) {
                Loge("getModem: out-of-bounds access for modem device " + index + " max: " + GetModemCount());
                return NULL;
            }

            If (DBG) Logd("getModem: " + index);
            return mModems->Get(index);
        }
    }

    /**
     * get total number of registered sims.
     */
    public Int32 GetSimCount() {
        Synchronized (mLock) {
            Int32 count = mSims->Size();
            If (DBG) Logd("getSimCount: " + count);
            return count;
        }
    }

    /**
     * get sim at index 'index'.
     */
    public HardwareConfig GetSim(Int32 index) {
        Synchronized (mLock) {
            If (mSims->IsEmpty()) {
                Loge("getSim: no registered sim device?!?");
                return NULL;
            }

            If (index > GetSimCount()) {
                Loge("getSim: out-of-bounds access for sim device " + index + " max: " + GetSimCount());
                return NULL;
            }

            If (DBG) Logd("getSim: " + index);
            return mSims->Get(index);
        }
    }

    /**
     * get modem associated with sim index 'simIndex'.
     */
    public HardwareConfig GetModemForSim(Int32 simIndex) {
        Synchronized (mLock) {
            If (mModems->IsEmpty() || mSims->IsEmpty()) {
                Loge("getModemForSim: no registered modem/sim device?!?");
                return NULL;
            }

            If (simIndex > GetSimCount()) {
                Loge("getModemForSim: out-of-bounds access for sim device " + simIndex + " max: " + GetSimCount());
                return NULL;
            }

            If (DBG) Logd("getModemForSim " + simIndex);

            HardwareConfig sim = GetSim(simIndex);
            For (HardwareConfig modem: mModems) {
                If (modem.uuid->Equals(sim.modemUuid)) {
                    return modem;
                }
            }

            return NULL;
        }
    }

    /**
     * get all sim's associated with modem at index 'modemIndex'.
     */
    public ArrayList<HardwareConfig> GetAllSimsForModem(Int32 modemIndex) {
        Synchronized (mLock) {
            If (mSims->IsEmpty()) {
                Loge("getAllSimsForModem: no registered sim device?!?");
                return NULL;
            }

            If (modemIndex > GetModemCount()) {
                Loge("getAllSimsForModem: out-of-bounds access for modem device " + modemIndex + " max: " + GetModemCount());
                return NULL;
            }

            If (DBG) Logd("getAllSimsForModem " + modemIndex);

            ArrayList<HardwareConfig> result = new ArrayList<HardwareConfig>();
            HardwareConfig modem = GetModem(modemIndex);
            For (HardwareConfig sim: mSims) {
                If (sim.modemUuid->Equals(modem.uuid)) {
                    result->Add(sim);
                }
            }
            return result;
        }
    }

    /**
     * get all modem's registered.
     */
    public ArrayList<HardwareConfig> GetAllModems() {
        Synchronized (mLock) {
            ArrayList<HardwareConfig> modems = new ArrayList<HardwareConfig>();
            If (mModems->IsEmpty()) {
                If (DBG) Logd("getAllModems: empty list.");
            } else {
                For (HardwareConfig modem: mModems) {
                    modems->Add(modem);
                }
            }

            return modems;
        }
    }

    /**
     * get all sim's registered.
     */
    public ArrayList<HardwareConfig> GetAllSims() {
        Synchronized (mLock) {
            ArrayList<HardwareConfig> sims = new ArrayList<HardwareConfig>();
            If (mSims->IsEmpty()) {
                If (DBG) Logd("getAllSims: empty list.");
            } else {
                For (HardwareConfig sim: mSims) {
                    sims->Add(sim);
                }
            }

            return sims;
        }
    }
}
