/*
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

package com.android.internal.telephony;


using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Telephony::IModemBindingPolicyHandler;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;


class SubscriptionHelper extends Handler {
    private static const String LOG_TAG = "SubHelper";
    private static SubscriptionHelper sInstance;

    private Context mContext;
    private CommandsInterface[] mCi;
    private Int32[] mSubStatus;
    private static Int32 sNumPhones;
    // This flag is used to trigger Dds during boot-up
    // and when flex mapping performed
    private static Boolean sTriggerDds = FALSE;

    private static const String APM_SIM_NOT_PWDN_PROPERTY = "persist.radio.apm_sim_not_pwdn";
    private static const Boolean sApmSIMNotPwdn = (SystemProperties->GetInt(
            APM_SIM_NOT_PWDN_PROPERTY, 0) == 1);

    private static const Int32 EVENT_SET_UICC_SUBSCRIPTION_DONE = 1;
    private static const Int32 EVENT_REFRESH = 2;
    private static const Int32 EVENT_REFRESH_OEM = 3;

    public static const Int32 SUB_SET_UICC_FAIL = -100;
    public static const Int32 SUB_SIM_NOT_INSERTED = -99;
    public static const Int32 SUB_INIT_STATE = -1;
    private static Boolean mNwModeUpdated = FALSE;

    private final ContentObserver nwModeObserver =
        new ContentObserver(new Handler()) {
            //@Override
            CARAPI OnChange(Boolean selfUpdate) {
                Logd("NwMode Observer onChange hit !!!");
                If (!mNwModeUpdated) return;
                //get nwMode from all slots in Db and update to subId table.
                UpdateNwModesInSubIdTable(TRUE);
            }
        };


    public static SubscriptionHelper Init(Context c, CommandsInterface[] ci) {
        Synchronized (SubscriptionHelper.class) {
            If (sInstance == NULL) {
                sInstance = new SubscriptionHelper(c, ci);
            } else {
                Log->Wtf(LOG_TAG, "Init() called multiple times!  sInstance = " + sInstance);
            }
            return sInstance;
        }
    }

    public static SubscriptionHelper GetInstance() {
        If (sInstance == NULL) {
           Log->Wtf(LOG_TAG, "getInstance NULL");
        }

        return sInstance;
    }

    private SubscriptionHelper(Context c, CommandsInterface[] ci) {
        mContext = c;
        mCi = ci;
        sNumPhones = TelephonyManager->GetDefault()->GetPhoneCount();
        mSubStatus = new Int32[sNumPhones];
        For (Int32 i=0; i < sNumPhones; i++ ) {
            mSubStatus[i] = SUB_INIT_STATE;
            Integer index = new Integer(i);
            If (mContext->GetResources()->GetBoolean(
                R.bool.config_sim_refresh_for_dual_mode_card)) {
                mCi[i].RegisterForSimRefreshEvent(this, EVENT_REFRESH_OEM, index);
            } else {
                mCi[i].RegisterForIccRefresh(this, EVENT_REFRESH, index);
            }
        }
        mContext->GetContentResolver()->RegisterContentObserver(Settings::Global::>GetUriFor(
                Settings::Global::PREFERRED_NETWORK_MODE), FALSE, nwModeObserver);


        Logd("SubscriptionHelper init by Context, num phones = "
                + sNumPhones + " ApmSIMNotPwdn = " + sApmSIMNotPwdn);
    }

    private void UpdateNwModesInSubIdTable(Boolean override) {
        SubscriptionController subCtrlr = SubscriptionController->GetInstance();
        For (Int32 i=0; i < sNumPhones; i++ ) {
            Int64[] subIdList = subCtrlr->GetSubId(i);
            If (subIdList != NULL && subIdList[0] > 0) {
                Int32 nwModeInDb;
                try {
                    nwModeInDb = TelephonyManager->GetIntAtIndex(mContext->GetContentResolver(),
                            Settings::Global::PREFERRED_NETWORK_MODE, i);
                } Catch (SettingNotFoundException snfe) {
                    Loge("Settings Exception Reading Value At Index[" + i +
                            "] Settings::Global::PREFERRED_NETWORK_MODE");
                    nwModeInDb = RILConstants.PREFERRED_NETWORK_MODE;
                }
                Int32 nwModeinSubIdTable = subCtrlr->GetNwMode(subIdList[0]);
                Logd("updateNwModesInSubIdTable: nwModeinSubIdTable: " + nwModeinSubIdTable
                        + ", nwModeInDb: " + nwModeInDb);

                //store Db value to table only if value in table is default
                //OR if override is set to True.
                If (override || nwModeinSubIdTable == SubscriptionManager.DEFAULT_NW_MODE) {
                    subCtrlr->SetNwMode(subIdList[0], nwModeInDb);
                }
            }
        }
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        Integer index = new Integer(PhoneConstants.DEFAULT_CARD_INDEX);
        AsyncResult ar;
        Switch(msg.what) {
            case EVENT_SET_UICC_SUBSCRIPTION_DONE:
                Logd("EVENT_SET_UICC_SUBSCRIPTION_DONE");
                ProcessSetUiccSubscriptionDone(msg);
                break;
            case EVENT_REFRESH:
            case EVENT_REFRESH_OEM:
                ar = (AsyncResult)msg.obj;
                index = (Integer)ar.userObj;
                Logi(" Received SIM refresh, reset sub state " +
                        index + " old sub state " + mSubStatus[index]);
                mSubStatus[index] = SUB_INIT_STATE;
                break;
           default:
           break;
        }
    }

    CARAPI UpdateSubActivation(Int32[] simStatus, Boolean isStackReadyEvent) {
        Boolean isPrimarySubFeatureEnable =
               SystemProperties->GetBoolean("persist.radio.primarycard", FALSE);
        SubscriptionController subCtrlr = SubscriptionController->GetInstance();
        Boolean setUiccSent = FALSE;
        // When isPrimarySubFeatureEnable is enabled apps will take care
        // of sending DDS on MMode SUB so no need of triggering DDS from here.
        If (isStackReadyEvent && !isPrimarySubFeatureEnable) {
            sTriggerDds = TRUE;
        }

        For (Int32 slotId = 0; slotId < sNumPhones; slotId++) {
            If (simStatus[slotId] == SUB_SIM_NOT_INSERTED) {
                mSubStatus[slotId] = simStatus[slotId];
                Logd(" Sim not inserted in slot [" + slotId + "] simStatus= " + simStatus[slotId]);
                continue;
            }
            Int64[] subId = subCtrlr->GetSubId(slotId);
            Int32 subState = subCtrlr->GetSubState(subId[0]);

            Logd("setUicc for [" + slotId + "] = " + subState + "subId = " + subId[0] +
                    " prev subState = " + mSubStatus[slotId] + " stackReady " + isStackReadyEvent);
            // Do not send SET_UICC if its already sent with same state
            If ((mSubStatus[slotId] != subState) || isStackReadyEvent) {
                // If sim card present in the slot, get the stored sub status and
                // perform the activation/deactivation of subscription
                SetUiccSubscription(slotId, subState);
                setUiccSent = TRUE;
            }
        }
        // If at least one uiccrequest sent, UpdateUserPrefs() will be called
        // from ProcessSetUiccSubscriptionDone()
        If (IsAllSubsAvailable() && (!setUiccSent)) {
            Logd("Received all sim info, update user pref subs, triggerDds= " + sTriggerDds);
            subCtrlr->UpdateUserPrefs(sTriggerDds);
            sTriggerDds = FALSE;
        }
    }

    CARAPI UpdateNwMode() {
        UpdateNwModesInSubIdTable(FALSE);
        ModemBindingPolicyHandler->GetInstance()->UpdatePrefNwTypeIfRequired();
        mNwModeUpdated = TRUE;
    }

    CARAPI SetUiccSubscription(Int32 slotId, Int32 subStatus) {
        Boolean set3GPPDone = FALSE, set3GPP2Done = FALSE;
        UiccCard uiccCard = UiccController->GetInstance()->GetUiccCard(slotId);
        If (uiccCard == NULL) {
            Logd("setUiccSubscription: slotId:" + slotId + " card info not available");
            return;
        }

        //Activate/Deactivate first 3GPP and 3GPP2 app in the sim, if available
        For (Int32 i = 0; i < uiccCard->GetNumApplications(); i++) {
            Int32 appType = uiccCard->GetApplicationIndex(i).GetType()->Ordinal();
            If (set3GPPDone == FALSE && (appType == PhoneConstants.APPTYPE_USIM ||
                    appType == PhoneConstants.APPTYPE_SIM)) {
                Message msgSetUiccSubDone = Message->Obtain(
                        this, EVENT_SET_UICC_SUBSCRIPTION_DONE, slotId, subStatus);
                mCi[slotId].SetUiccSubscription(slotId, i, slotId, subStatus, msgSetUiccSubDone);
                set3GPPDone = TRUE;
            } else If (set3GPP2Done == FALSE && (appType == PhoneConstants.APPTYPE_CSIM ||
                    appType == PhoneConstants.APPTYPE_RUIM)) {
                Message msgSetUiccSubDone = Message->Obtain(
                        this, EVENT_SET_UICC_SUBSCRIPTION_DONE, slotId, subStatus);
                mCi[slotId].SetUiccSubscription(slotId, i, slotId, subStatus, msgSetUiccSubDone);
                set3GPP2Done = TRUE;
            }

            If (set3GPPDone && set3GPP2Done) break;
        }
    }

    /**
     * Handles the EVENT_SET_UICC_SUBSCRPTION_DONE.
     * @param ar
     */
    private void ProcessSetUiccSubscriptionDone(Message msg) {
        SubscriptionController subCtrlr = SubscriptionController->GetInstance();
        AsyncResult ar = (AsyncResult)msg.obj;
        Int32 slotId = msg.arg1;
        Int32 newSubState = msg.arg2;
        Int64[] subId = subCtrlr->GetSubIdUsingSlotId(slotId);

        If (ar.exception != NULL) {
            Loge("Exception in SET_UICC_SUBSCRIPTION, slotId = " + slotId
                    + " newSubState " + newSubState);
            // broadcast set uicc failure
            mSubStatus[slotId] = SUB_SET_UICC_FAIL;
            BroadcastSetUiccResult(slotId, newSubState, PhoneConstants.FAILURE);
            return;
        }

        Int32 subStatus = subCtrlr->GetSubState(subId[0]);
        If (newSubState != subStatus) {
            subCtrlr->SetSubState(subId[0], newSubState);
        }
        BroadcastSetUiccResult(slotId, newSubState, PhoneConstants.SUCCESS);

        mSubStatus[slotId] = newSubState;
        // After activating all subs, updated the user preferred sub values
        If (IsAllSubsAvailable()) {
            Logd("Received all subs, now update user preferred subs, slotid = " + slotId
                    + " newSubState = " + newSubState + " sTriggerDds = " + sTriggerDds);
            subCtrlr->UpdateUserPrefs(sTriggerDds);
            sTriggerDds = FALSE;
        }
    }

    private void BroadcastSetUiccResult(Int32 slotId, Int32 newSubState, Int32 result) {
        Int64[] subId = SubscriptionController->GetInstance()->GetSubIdUsingSlotId(slotId);
        Intent intent = new Intent(TelephonyIntents.ACTION_SUBSCRIPTION_SET_UICC_RESULT);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, slotId, subId[0]);
        intent->PutExtra(TelephonyIntents.EXTRA_RESULT, result);
        intent->PutExtra(TelephonyIntents.EXTRA_NEW_SUB_STATE, newSubState);
        mContext->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    private Boolean IsAllSubsAvailable() {
        Boolean allSubsAvailable = TRUE;

        For (Int32 i=0; i < sNumPhones; i++) {
            If (mSubStatus[i] == SUB_INIT_STATE) {
                allSubsAvailable = FALSE;
            }
        }
        return allSubsAvailable;
    }

    public Boolean IsRadioOn(Int32 phoneId) {
        return mCi[phoneId].GetRadioState()->IsOn();
    }

    public Boolean IsRadioAvailable(Int32 phoneId) {
        return mCi[phoneId].GetRadioState()->IsAvailable();
    }

    public Boolean IsApmSIMNotPwdn() {
        return sApmSIMNotPwdn;
    }

    public Boolean ProceedToHandleIccEvent(Int32 slotId) {
        Int32 apmState = Settings::Global::>GetInt(mContext->GetContentResolver(),
                Settings::Global::AIRPLANE_MODE_ON, 0);

        // If SIM powers down in APM, telephony needs to send SET_UICC
        // once radio turns ON also do not handle process SIM change events
        If ((!sApmSIMNotPwdn) && (!IsRadioOn(slotId) || (apmState == 1))) {
            Logi(" proceedToHandleIccEvent, radio off/unavailable, slotId = " + slotId);
            mSubStatus[slotId] = SUB_INIT_STATE;
        }

        // Do not handle if SIM powers down in APM mode
        If ((apmState == 1) && (!sApmSIMNotPwdn)) {
            Logd(" proceedToHandleIccEvent, sApmSIMNotPwdn = " + sApmSIMNotPwdn);
            return FALSE;
        }


        // Seems SSR happenned or RILD crashed, do not handle SIM change events
        If (!IsRadioAvailable(slotId)) {
            Logi(" proceedToHandleIccEvent, radio not available, slotId = " + slotId);
            return FALSE;
        }
        return TRUE;
    }

    private static void Logd(String message) {
        Rlog->D(LOG_TAG,  message);
    }

    private void Logi(String msg) {
        Rlog->I(LOG_TAG,  msg);
    }

    private void Loge(String msg) {
        Rlog->E(LOG_TAG,  msg);
    }
}
