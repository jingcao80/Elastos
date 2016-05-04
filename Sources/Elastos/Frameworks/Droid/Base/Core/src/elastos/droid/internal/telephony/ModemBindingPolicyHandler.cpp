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

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Droid::Internal::Telephony::ModemStackController::IModemCapabilityInfo;

using Elastos::Utility::IHashMap;

/*
 * ModemBindingPolicyHandler: Class used to determine stack binding policy based on
 * Network mode selected by the user.
 *
 * Before sending NwMode to RIL, checks if the NwMode is supported on the Stack and
 * if possible initiate a cross mapping sequence.
 * Includes:
 *    Current preferred NwModes
 *    Logic for determining NwMode supported on particular Stack
 *    Logic for determining stack binding policy based on Network mode
 *    selected by user.
 *    Send request for Stack binding update based on above policy.
 */

public class ModemBindingPolicyHandler extends Handler {
    static const String LOG_TAG = "ModemBindingPolicyHandler";

    //*****Network Mode bit vector mask
    //Basic Network Modes
    private static const Int32 NETWORK_MASK_GSM_ONLY =
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_GSM |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_GPRS |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_EDGE;
    private static const Int32 NETWORK_MASK_WCDMA_ONLY =
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_UMTS |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_HSDPA |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_HSUPA |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_HSPA |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_HSPAP;
    private static const Int32 NETWORK_MASK_CDMA_NO_EVDO =
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_IS95A |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_IS95B |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_1xRTT;
    private static const Int32 NETWORK_MASK_EVDO_NO_CDMA =
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_0 |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_A |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_B |
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD;
    private static const Int32 NETWORK_MASK_LTE_ONLY =
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_LTE;
    private static const Int32 NETWORK_MASK_TD_SCDMA_ONLY =
            1 << ServiceState.RIL_RADIO_TECHNOLOGY_TD_SCDMA;

    //Complex modes, i.e. Combinations of above basic Network Modes
    private static const Int32 NETWORK_MASK_GSM_UMTS =
            NETWORK_MASK_GSM_ONLY |
            NETWORK_MASK_WCDMA_ONLY;
    private static const Int32 NETWORK_MASK_CDMA =
            NETWORK_MASK_CDMA_NO_EVDO |
            NETWORK_MASK_EVDO_NO_CDMA;
    private static const Int32 NETWORK_MASK_WCDMA_PREF =
            NETWORK_MASK_GSM_UMTS;
    private static const Int32 NETWORK_MASK_GLOBAL =
            NETWORK_MASK_GSM_UMTS |
            NETWORK_MASK_CDMA;
    private static const Int32 NETWORK_MASK_LTE_CDMA_EVDO =
            NETWORK_MASK_LTE_ONLY |
            NETWORK_MASK_CDMA;
    private static const Int32 NETWORK_MASK_LTE_GSM_WCDMA =
            NETWORK_MASK_LTE_ONLY |
            NETWORK_MASK_GSM_UMTS;
    private static const Int32 NETWORK_MASK_LTE_CMDA_EVDO_GSM_WCDMA =
            NETWORK_MASK_LTE_ONLY |
            NETWORK_MASK_CDMA |
            NETWORK_MASK_GSM_UMTS;
    private static const Int32 NETWORK_MASK_LTE_WCDMA =
            NETWORK_MASK_LTE_ONLY |
            NETWORK_MASK_WCDMA_ONLY;
    private static const Int32 NETWORK_MASK_TD_SCDMA_WCDMA =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_WCDMA_ONLY;
    private static const Int32 NETWORK_MASK_TD_SCDMA_LTE =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_LTE_ONLY;
    private static const Int32 NETWORK_MASK_TD_SCDMA_GSM =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_GSM_ONLY;
    private static const Int32 NETWORK_MASK_TD_SCDMA_GSM_LTE =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_GSM_ONLY |
            NETWORK_MASK_LTE_ONLY;
    private static const Int32 NETWORK_MASK_TD_SCDMA_GSM_WCDMA =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_GSM_UMTS;
    private static const Int32 NETWORK_MASK_TD_SCDMA_WCDMA_LTE =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_LTE_WCDMA;
    private static const Int32 NETWORK_MASK_TD_SCDMA_GSM_WCDMA_LTE =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_LTE_GSM_WCDMA;
    private static const Int32 NETWORK_MASK_TD_SCDMA_CDMA_EVDO_GSM_WCDMA =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_CDMA |
            NETWORK_MASK_GSM_UMTS;
    private static const Int32 NETWORK_MASK_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA =
            NETWORK_MASK_TD_SCDMA_ONLY |
            NETWORK_MASK_LTE_CMDA_EVDO_GSM_WCDMA;

    //***** Events
    private static const Int32 EVENT_MODEM_RAT_CAPS_AVAILABLE = 1;
    private static const Int32 EVENT_UPDATE_BINDING_DONE = 2;
    private static const Int32 EVENT_SET_NW_MODE_DONE = 3;

    //*****Constants
    private static const Int32 SUCCESS = 1;
    private static const Int32 FAILURE = 0;

    //***** Class Variables
    private static ModemBindingPolicyHandler sModemBindingPolicyHandler;
    private static ModemStackController mModemStackController;
    private CommandsInterface[] mCi;
    private Context mContext;
    private Int32 mNumOfSetPrefNwModeSuccess = 0;
    private Int32 mNumPhones = TelephonyManager->GetDefault()->GetPhoneCount();
    private Boolean mModemRatCapabilitiesAvailable = FALSE;
    private Boolean mIsSetPrefNwModeInProgress = FALSE;
    //private Boolean mIsBindingInProgress = FALSE;
    private Int32[] mPreferredStackId = new Int32[mNumPhones];
    private Int32[] mCurrentStackId = new Int32[mNumPhones];
    private Int32[] mPrefNwMode = new Int32[mNumPhones];
    private Int32[] mNwModeinSubIdTable = new Int32[mNumPhones];
    private HashMap<Integer, Message> mStoredResponse = new HashMap<Integer, Message>();

    //Modem capabilities as per StackId
    private ModemCapabilityInfo[] mModemCapInfo = NULL;

    //***** Class Methods
    public static ModemBindingPolicyHandler Make(Context context, UiccController uiccMgr,
            CommandsInterface[] ci) {
        Rlog->D(LOG_TAG, "getInstance");
        If (sModemBindingPolicyHandler == NULL) {
            sModemBindingPolicyHandler = new ModemBindingPolicyHandler(context, uiccMgr, ci);
        } else {
            throw new RuntimeException("ModemBindingPolicyHandler->Make() should be called once");
        }
        return sModemBindingPolicyHandler;
    }

    public static ModemBindingPolicyHandler GetInstance() {
        If (sModemBindingPolicyHandler == NULL) {
            throw new RuntimeException("ModemBindingPolicyHdlr.getInstance called before Make()");
        }
        return sModemBindingPolicyHandler;
    }

    //***** Constructor
    private ModemBindingPolicyHandler(Context context, UiccController uiccManager,
            CommandsInterface[] ci) {
        Logd("Constructor - Enter");

        mCi = ci;
        mContext = context;
        mModemStackController = ModemStackController->GetInstance();
        mModemCapInfo = new ModemCapabilityInfo[mNumPhones];

        mModemStackController.registerForModemRatCapsAvailable
                (this, EVENT_MODEM_RAT_CAPS_AVAILABLE, NULL);

        For (Int32 i = 0; i < mNumPhones; i++) {
            mPreferredStackId[i] = i;
            mCurrentStackId[i] = i;
            mStoredResponse->Put(i, NULL);
        }

        Logd("Constructor - Exit");
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        Switch(msg.what) {
            case EVENT_UPDATE_BINDING_DONE:
                ar = (AsyncResult)msg.obj;
                HandleUpdateBindingDone(ar);
                break;

            case EVENT_MODEM_RAT_CAPS_AVAILABLE:
                HandleModemRatCapsAvailable();
                break;

            case EVENT_SET_NW_MODE_DONE:
                HandleSetPreferredNetwork(msg);
                break;

            default:
                break;
        }
    }

    private void HandleSetPreferredNetwork(Message msg) {
        AsyncResult ar = (AsyncResult) msg.obj;
        Int32 index = (Integer) ar.userObj;
        If (ar.exception == NULL) {
            mNumOfSetPrefNwModeSuccess++;
            // set nw mode success for all the subs, then update value to DB
            If (mNumOfSetPrefNwModeSuccess == mNumPhones) {
                For (Int32 i = 0; i < mNumPhones; i++) {
                    Logd("Updating network mode in DB for slot[" + i + "] with "
                            + mNwModeinSubIdTable[i]);
                    TelephonyManager->PutIntAtIndex(mContext->GetContentResolver(),
                            android.provider.Settings::Global::PREFERRED_NETWORK_MODE,
                            i, mNwModeinSubIdTable[i]);
                }
                mNumOfSetPrefNwModeSuccess = 0;
            }
        } else {
            Logd("Failed to set preferred network mode for slot" + index);
            mNumOfSetPrefNwModeSuccess = 0;
        }

    }

    private void HandleUpdateBindingDone(AsyncResult ar) {
        mIsSetPrefNwModeInProgress = FALSE;

        For (Int32 i = 0; i < mNumPhones; i++) {
            Int32 errorCode = RILConstants.SUCCESS;
            Message resp = mStoredResponse->Get(i);
            If (resp != NULL) {
                If (ar.exception != NULL) {
                    errorCode = RILConstants.GENERIC_FAILURE;
                }
                SendResponseToTarget(resp, errorCode);
                mStoredResponse->Put(i, NULL);
            }
        }
    }

    /*
    * updatePrefNwTypeIfRequired: Method used to set pref network type if required.
    *
    * Description: If Network mode for a subid in simInfo table is valid and and is not
    * equal to value in DB, then update the DB value and send request to RIL.
    */
    CARAPI UpdatePrefNwTypeIfRequired(){
        Boolean updateRequired = FALSE;
        SyncPreferredNwModeFromDB();
        SubscriptionController subCtrlr = SubscriptionController->GetInstance();
        For (Int32 i=0; i < mNumPhones; i++ ) {
            Int64[] subIdList = subCtrlr->GetSubId(i);
            If (subIdList != NULL && subIdList[0] > 0) {
                Int64 subId = subIdList[0];
                mNwModeinSubIdTable[i] = subCtrlr->GetNwMode(subId);
                If (mNwModeinSubIdTable[i] == SubscriptionManager.DEFAULT_NW_MODE){
                    updateRequired = FALSE;
                    break;
                }
                If (mNwModeinSubIdTable[i] != mPrefNwMode[i]) {
                    updateRequired = TRUE;
                }
            }
        }

        If (updateRequired) {
            If (FAILURE == UpdateStackBindingIfRequired(FALSE)) {
                //In case of Update Stack Binding not required or failure, send setPrefNwType to
                //RIL immediately. In case of success after stack binding completed setPrefNwType
                //request is anyways sent.
                For (Int32 i=0; i < mNumPhones; i++ ) {
                    Message msg = ObtainMessage(EVENT_SET_NW_MODE_DONE, i);
                    mCi[i].SetPreferredNetworkType(mNwModeinSubIdTable[i], msg);
                }
            }
       }
    }

    private void HandleModemRatCapsAvailable() {
        mModemRatCapabilitiesAvailable = TRUE;
        //Initialization sequence: Need to send Bind request always, so override is TRUE.
        If (SUCCESS == UpdateStackBindingIfRequired(TRUE)) mIsSetPrefNwModeInProgress = TRUE;
    }

    private void SyncCurrentStackInfo() {
        //Get Current phoneId to stack mapping.
        For (Int32 i = 0; i < mNumPhones; i++) {
            mCurrentStackId[i] = mModemStackController->GetCurrentStackIdForPhoneId(i);
            mModemCapInfo[mCurrentStackId[i]] = mModemStackController->GetModemRatCapsForPhoneId(i);
            //reset Preferred to current.
            mPreferredStackId[i] = (mCurrentStackId[i] >= 0) ? mCurrentStackId[i] : i;
        }
    }

    /*
    * updateStackBindingIfRequired: Method used to update stack binding if required.
    * @param: Boolean override - if TRUE send update stack binding request always
    * @return: Int32 SUCCESS or FAILURE. i.e. Request acepted or not.
    *   SUCCESS, if update stack binding request is sent and it returns SUCCESS
    *   FAILURE, if update stack binding request is not sent or
    *       if update stack binding request is sent and it returns FAILURE.
    */
    private Int32 UpdateStackBindingIfRequired(Boolean isBootUp) {
        Boolean isUpdateStackBindingRequired = FALSE;
        Int32 response = FAILURE;

        UpdatePreferredStackIds();

        For (Int32 i = 0; i < mNumPhones; i++) {
            If (mPreferredStackId[i] != mCurrentStackId[i]) {
                //if preferred stackId is different from current, bindupdate is required.
                isUpdateStackBindingRequired = TRUE;
                break;
            }
        }
        If (isBootUp || isUpdateStackBindingRequired) {
            Message msg = Message->Obtain(this, EVENT_UPDATE_BINDING_DONE, NULL);
            response = mModemStackController->UpdateStackBinding(mPreferredStackId, isBootUp, msg);
        }

        return response;
    }

    /*
    * updatePreferredStackIds: Method used to get isCrossBindingRequired and set the Preferred
    * StackIds as per Network Mode for phoneId.
    * @return: Boolean TRUE/FALSE based on crossMapping is required.
    *
    * Description: used to determine the preferred stackIds based on Network Mode selected
    * and return if CrossBindingRequired.
    * Logic:
    * 1. For each phoneId, check if current stack supports the present Network Mode.
    * 2. If supported, continue to next phoneId. If stacks on all phoneId support their respective
    *     Network Mode then crossmapping is not required so preferredStackIds will be same as
    *     CurrentStackIds
    * 3. If stack of any of the phoneId is not supporting their Network Mode, then check if stack of
    *     other phoneIds supports present NetworkMode.
    *    a. If none of the stacks support the present Network Mode, cross mapping is not required
    *       and preferredStackIds will be same as CurrentStackIds.
    *    b. If some other stack supports, check if network mode on other phoneId is supported by
    *       stack after cross mapping.
    *           i. if supported cross mapping is required and set preferredStackIds accordingly.
    *           ii. if not supported cross mapping is not required and preferredStackIds will be
    *              same as CurrentStackIds.
    */
    private void UpdatePreferredStackIds() {
        If (!mModemRatCapabilitiesAvailable) {
            Loge("updatePreferredStackIds: Modem Capabilites are not Available. Return!!");
            return;
        }

        //Get current prefNwMode and Stack info before updating pref Stack
        SyncPreferredNwModeFromDB();
        SyncCurrentStackInfo();

        For (Int32 curPhoneId = 0; curPhoneId < mNumPhones; curPhoneId++) {
            //Continue if current stack supports Network Mode.
            If (IsNwModeSupportedOnStack(mPrefNwMode[curPhoneId], mCurrentStackId[curPhoneId])) {
                Logd("updatePreferredStackIds: current stack[" + mCurrentStackId[curPhoneId] +
                        "]supports NwMode[" + mPrefNwMode[curPhoneId] + "] on phoneId["
                        + curPhoneId + "]");
                continue;
            }

            //Current stack i do not support present Network Mode.
            //Check if any other stack supports the present Network Mode
            For (Int32 otherPhoneId = 0; otherPhoneId < mNumPhones; otherPhoneId++) {
                //continue for same stack, check only on other stacks
                If (otherPhoneId == curPhoneId) continue;

                If (IsNwModeSupportedOnStack(mPrefNwMode[curPhoneId],
                        mCurrentStackId[otherPhoneId])) {
                    //Some other stack supports present Network Mode, Check if Network Mode
                    //on other PhoneId is supported on current Stack after Cross binding.
                    If (IsNwModeSupportedOnStack(mPrefNwMode[otherPhoneId],
                            mCurrentStackId[curPhoneId])) {
                        Logd("updatePreferredStackIds: Cross Binding is possible between phoneId["
                                + curPhoneId + "] and phoneId[" + otherPhoneId + "]");
                        //set preferred stackid of curPhoneId as current stack id of otherPhoneId
                        //and preferred stackid of otherPhoneId as current stack id of curPhoneId
                        mPreferredStackId[curPhoneId] = mCurrentStackId[otherPhoneId];
                        mPreferredStackId[otherPhoneId] = mCurrentStackId[curPhoneId];
                    }
                }
            }
        }
    }

    private Boolean IsNwModeSupportedOnStack(Int32 nwMode, Int32 stackId) {
        Int32[] numRatSupported = new Int32[mNumPhones];
        Int32 maxNumRatSupported = 0;
        Boolean isSupported = FALSE;

        //Get num of RATs supported for this NwMode on all Stacks
        For (Int32 i = 0; i < mNumPhones; i++) {
            numRatSupported[i] = GetNumOfRatSupportedForNwMode(nwMode, mModemCapInfo[i]);
            If (maxNumRatSupported < numRatSupported[i]) maxNumRatSupported = numRatSupported[i];
        }

        //if current stackId supports Max RATs of all other stacks, then return TRUE.
        If (numRatSupported[stackId] == maxNumRatSupported) isSupported = TRUE;

        Logd("nwMode:" + nwMode + ", on stack:" + stackId + " is " +
                (isSupported ? "Supported" : "Not Supported"));

        return isSupported;
    }

    private void SyncPreferredNwModeFromDB() {
        For (Int32 i = 0; i < mNumPhones; i++) {
            try {
                mPrefNwMode[i] = TelephonyManager->GetIntAtIndex(mContext->GetContentResolver(),
                        android.provider.Settings::Global::PREFERRED_NETWORK_MODE, i);
             } Catch (SettingNotFoundException snfe) {
                Loge("getPreferredNetworkMode: Could not find PREFERRED_NETWORK_MODE!!!");
                mPrefNwMode[i] = Phone.PREFERRED_NT_MODE;
             }
        }
    }

    CARAPI SetPreferredNetworkType(Int32 networkType, Int32 phoneId,
            Message response) {
        //if binding is in progress return failure for this request
        If (mIsSetPrefNwModeInProgress) {
            Loge("setPreferredNetworkType: In Progress:");
            SendResponseToTarget(response, RILConstants.GENERIC_FAILURE);
            return;
        }

        Logd("setPreferredNetworkType: nwMode:" + networkType + ", on phoneId:" + phoneId);

        mIsSetPrefNwModeInProgress = TRUE;

        //If CrossBinding request is not accepted, i.e. return value is FAILURE
        //send request directly to RIL, or else store the setpref Msg for later processing.
        If (UpdateStackBindingIfRequired(FALSE) == SUCCESS) {
            mStoredResponse->Put(phoneId, response);
        } else {
            mCi[phoneId].SetPreferredNetworkType(networkType, response);
            mIsSetPrefNwModeInProgress = FALSE;
        }
    }

    private void SendResponseToTarget(Message response, Int32 responseCode) {
        If (response != NULL) {
            Exception e = CommandException->FromRilErrno(responseCode);
            AsyncResult->ForMessage(response, NULL, e);
            response->SendToTarget();
        }
    }

    private Int32 GetNumOfRatSupportedForNwMode(Int32 nwMode,
            ModemCapabilityInfo modemCaps) {
        Int32 supportedRatMaskForNwMode = 0;
        If (modemCaps == NULL){
            Loge("getNumOfRatSupportedForNwMode: nwMode[" + nwMode + "] modemCaps was NULL!");
            return supportedRatMaskForNwMode;
        }
        Logd("getNumOfRATsSupportedForNwMode: nwMode[" + nwMode +"] modemCaps = " + modemCaps);

        //send result by ANDing corresponding NETWORK MASK and Modem Caps mask.
        Switch (nwMode) {
            case RILConstants.NETWORK_MODE_WCDMA_PREF:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_WCDMA_PREF;
                break;

            case RILConstants.NETWORK_MODE_GSM_ONLY:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_GSM_ONLY;
                break;

            case RILConstants.NETWORK_MODE_WCDMA_ONLY:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_WCDMA_ONLY;
                break;

            case RILConstants.NETWORK_MODE_GSM_UMTS:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_GSM_UMTS;
                break;

            case RILConstants.NETWORK_MODE_CDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_CDMA;
                break;

            case RILConstants.NETWORK_MODE_CDMA_NO_EVDO:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_CDMA_NO_EVDO;
                break;

            case RILConstants.NETWORK_MODE_EVDO_NO_CDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_EVDO_NO_CDMA;
                break;

            case RILConstants.NETWORK_MODE_GLOBAL:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_GLOBAL;
                break;

            case RILConstants.NETWORK_MODE_LTE_CDMA_EVDO:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_LTE_CDMA_EVDO;
                break;

            case RILConstants.NETWORK_MODE_LTE_GSM_WCDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_LTE_GSM_WCDMA;
                break;

            case RILConstants.NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_LTE_CMDA_EVDO_GSM_WCDMA;
                break;

            case RILConstants.NETWORK_MODE_LTE_ONLY:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_LTE_ONLY;
                break;

            case RILConstants.NETWORK_MODE_LTE_WCDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_LTE_WCDMA;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_ONLY:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_ONLY;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_WCDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_WCDMA;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_LTE:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_LTE;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_GSM:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_GSM;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_GSM_LTE:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_GSM_LTE;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_GSM_WCDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_GSM_WCDMA;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_WCDMA_LTE:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_WCDMA_LTE;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_GSM_WCDMA_LTE:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_GSM_WCDMA_LTE;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_CDMA_EVDO_GSM_WCDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_CDMA_EVDO_GSM_WCDMA;
                break;

            case RILConstants.NETWORK_MODE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
                supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                        NETWORK_MASK_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;
                break;

            default:
                break;
        }

        Logd("getNumOfRATsSupportedForNwMode: supportedRatMaskForNwMode:" +
                supportedRatMaskForNwMode);

        return GetNumRatSupportedInMask(supportedRatMaskForNwMode);
    }

    //This method will return no. of 1's in binary representation of Int32.
    private Int32 GetNumRatSupportedInMask(Int32 mask) {
        Int32 noOfOnes = 0;
        /*
         * To count num of 1's in bitmask.
         * The bitwise and of a number and number - 1, will result in zeroing the least
         *  significant non-zero bit in the number. So if a number has n bits that were set to 1,
         *  then after n iterations of above operation, number will be changed to zero.
         */
        While (mask != 0) {
            mask &= mask - 1;
            noOfOnes++;
        }
        return noOfOnes;
    }
    private void Logd(String string) {
        Rlog->D(LOG_TAG, string);
    }

    private void Loge(String string) {
        Rlog->E(LOG_TAG, string);
    }
}
