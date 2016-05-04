/*
* Copyright (C) 2011-2014 MediaTek Inc.
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

using static android::Manifest::Permission::IREAD_PHONE_STATE;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardStatus::ICardState;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::Uicc::ISpnOverride;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::Utility::IList;

/**
 *@hide
 */
public class SubInfoRecordUpdater extends Handler {
    private static const String LOG_TAG = "SUB";
    private static const Int32 PROJECT_SIM_NUM = TelephonyManager->GetDefault()->GetPhoneCount();
    private static const Int32 EVENT_OFFSET = 8;
    private static const Int32 EVENT_QUERY_ICCID_DONE = 1;
    private static const Int32 EVENT_ICC_CHANGED = 2;
    private static const Int32 EVENT_STACK_READY = 3;
    private static const String ICCID_STRING_FOR_NO_SIM = "";
    private static const String ICCID_STRING_FOR_NV = "DUMMY_NV_ID";
    /**
     *  Int32[] sInsertSimState maintains all slots' SIM inserted status currently,
     *  it may contain 4 kinds of values:
     *    SIM_NOT_INSERT : no SIM inserted in slot i now
     *    SIM_CHANGED    : a valid SIM insert in slot i and is different SIM from last time
     *                     it will later become SIM_NEW or SIM_REPOSITION during update procedure
     *    SIM_NOT_CHANGE : a valid SIM insert in slot i and is the same SIM as last time
     *    SIM_NEW        : a valid SIM insert in slot i and is a new SIM
     *    SIM_REPOSITION : a valid SIM insert in slot i and is inserted in different slot last time
     *    positive integer #: index to distinguish SIM cards with the same IccId
     */
    public static const Int32 SIM_NOT_CHANGE = 0;
    public static const Int32 SIM_CHANGED    = -1;
    public static const Int32 SIM_NEW        = -2;
    public static const Int32 SIM_REPOSITION = -3;
    public static const Int32 SIM_NOT_INSERT = -99;

    public static const Int32 STATUS_NO_SIM_INSERTED = 0x00;
    public static const Int32 STATUS_SIM1_INSERTED = 0x01;
    public static const Int32 STATUS_SIM2_INSERTED = 0x02;
    public static const Int32 STATUS_SIM3_INSERTED = 0x04;
    public static const Int32 STATUS_SIM4_INSERTED = 0x08;

    private static Phone[] sPhone;
    private static Context sContext = NULL;
    private static CardState[] sCardState = new CardState[PROJECT_SIM_NUM];
    private static UiccController mUiccController = NULL;
    private static CommandsInterface[] sCi;
    private static IccFileHandler[] sFh = new IccFileHandler[PROJECT_SIM_NUM];
    private static String sIccId[] = new String[PROJECT_SIM_NUM];
    private static Int32[] sInsertSimState = new Int32[PROJECT_SIM_NUM];
    private static TelephonyManager sTelephonyMgr = NULL;
    // To prevent repeatedly update flow every time receiver SIM_STATE_CHANGE
    private static Boolean sNeedUpdate = TRUE;
    private Boolean isNVSubAvailable = FALSE;

    public SubInfoRecordUpdater(Context context, Phone[] phoneProxy, CommandsInterface[] ci) {
        Logd("Constructor invoked");

        sContext = context;
        sPhone = phoneProxy;
        sCi = ci;
        SubscriptionHelper->Init(context, ci);
        mUiccController = UiccController->GetInstance();
        mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);
        ModemStackController->GetInstance()->RegisterForStackReady(this, EVENT_STACK_READY, NULL);
        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            sCardState[i] = CardState.CARDSTATE_ABSENT;
        }
        IntentFilter intentFilter = new IntentFilter(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
        sContext->RegisterReceiver(sReceiver, intentFilter);
    }

    private static Int32 EncodeEventId(Int32 event, Int32 slotId) {
        return event << (slotId * EVENT_OFFSET);
    }

    private final BroadcastReceiver sReceiver = new  BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            Logd("[Receiver]+");
            String action = intent->GetAction();
            Int32 slotId;
            Logd("Action: " + action);
            If (action->Equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
                String simStatus = intent->GetStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                slotId = intent->GetIntExtra(PhoneConstants.SLOT_KEY,
                        SubscriptionManager.INVALID_SLOT_ID);
                Logd("slotId: " + slotId + " simStatus: " + simStatus);
                If (slotId == SubscriptionManager.INVALID_SLOT_ID) {
                    return;
                }

                If (IccCardConstants.INTENT_VALUE_ICC_LOADED->Equals(simStatus)) {
                    If (sTelephonyMgr == NULL) {
                        sTelephonyMgr = TelephonyManager->From(sContext);
                    }

                    Int64 subId = intent->GetLongExtra(PhoneConstants.SUBSCRIPTION_KEY,
                            SubscriptionManager.INVALID_SUB_ID);

                    If (SubscriptionManager->IsValidSubId(subId)) {
                        String msisdn = TelephonyManager->GetDefault()->GetLine1NumberForSubscriber(subId);
                        ContentResolver contentResolver = sContext->GetContentResolver();

                        If (msisdn != NULL) {
                            ContentValues number = new ContentValues(1);
                            number->Put(SubscriptionManager.NUMBER, msisdn);
                            contentResolver->Update(SubscriptionManager.CONTENT_URI, number,
                                    SubscriptionManager._ID + "=" + Long->ToString(subId), NULL);
                        }

                        SubInfoRecord subInfo =
                                SubscriptionManager->GetSubInfoForSubscriber(subId);

                        If (subInfo != NULL
                                && subInfo.nameSource != SubscriptionManager.NAME_SOURCE_USER_INPUT) {
                            SpnOverride mSpnOverride = new SpnOverride();
                            String nameToSet;
                            String Carrier =
                                    TelephonyManager->GetDefault()->GetSimOperator(subId);
                            Logd("Carrier = " + Carrier);
                            String CarrierName =
                                    TelephonyManager->GetDefault()->GetSimOperatorName(subId);
                            Logd("CarrierName = " + CarrierName);

                            If (mSpnOverride->ContainsCarrier(Carrier)) {
                                nameToSet = mSpnOverride->GetSpn(Carrier);
                                Logd("Found, name = " + nameToSet);
                            } else If (CarrierName != "") {
                                nameToSet = CarrierName;
                            } else {
                                nameToSet = "SIM " + Integer->ToString(slotId + 1);
                                Logd("Not found, name = " + nameToSet);
                            }

                            ContentValues name = new ContentValues(1);
                            name->Put(SubscriptionManager.DISPLAY_NAME, nameToSet);
                            contentResolver->Update(SubscriptionManager.CONTENT_URI, name,
                                    SubscriptionManager._ID + "=" + Long->ToString(subId), NULL);
                        }
                    } else {
                        Logd("[Receiver] Invalid subId, could not update ContentResolver");
                    }
                }
            }
            Logd("[Receiver]-");
        }
    };

    private Boolean IsAllIccIdQueryDone() {
        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            If (sIccId[i] == NULL) {
                Logd("Wait for SIM" + (i + 1) + " IccId");
                return FALSE;
            }
        }
        Logd("All IccIds query complete");

        return TRUE;
    }

    public static void SetDisplayNameForNewSub(String newSubName, Int32 subId, Int32 newNameSource) {
        SubInfoRecord subInfo = SubscriptionManager->GetSubInfoForSubscriber(subId);
        If (subInfo != NULL) {
            // overwrite SIM display name if it is not assigned by user
            Int32 oldNameSource = subInfo.nameSource;
            String oldSubName = subInfo.displayName;
            Logd("[setDisplayNameForNewSub] mSubInfoIdx = " + subInfo.subId + ", oldSimName = "
                    + oldSubName + ", oldNameSource = " + oldNameSource + ", newSubName = "
                    + newSubName + ", newNameSource = " + newNameSource);
            If (oldSubName == NULL ||
                (oldNameSource == SubscriptionManager.NAME_SOURCE_DEFAULT_SOURCE && newSubName != NULL) ||
                (oldNameSource == SubscriptionManager.NAME_SOURCE_SIM_SOURCE && newSubName != NULL
                        && !newSubName->Equals(oldSubName))) {
                SubscriptionManager->SetDisplayName(newSubName,
                        subInfo.subId, newNameSource);
            }
        } else {
            Logd("SUB" + (subId + 1) + " SubInfo not created yet");
        }
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar = (AsyncResult)msg.obj;
        Int32 msgNum = msg.what;
        Int32 slotId;
        For (slotId = PhoneConstants.SUB1; slotId <= PhoneConstants.SUB3; slotId++) {
            Int32 pivot = 1 << (slotId * EVENT_OFFSET);
            If (msgNum >= pivot) {
                continue;
            } else {
                break;
            }
        }
        slotId--;
        Int32 event = msgNum >> (slotId * EVENT_OFFSET);
        Switch (event) {
            case EVENT_QUERY_ICCID_DONE:
                Logd("handleMessage : <EVENT_QUERY_ICCID_DONE> SIM" + (slotId + 1));
                If (ar.exception == NULL) {
                    If (ar.result != NULL) {
                        Byte[] data = (Byte[])ar.result;
                        sIccId[slotId] = IccUtils->BcdToString(data, 0, data.length);
                    } else {
                        Logd("Null ar");
                        sIccId[slotId] = ICCID_STRING_FOR_NO_SIM;
                    }
                } else {
                    sIccId[slotId] = ICCID_STRING_FOR_NO_SIM;
                    Logd("Query IccId fail: " + ar.exception);
                }
                Logd("sIccId[" + slotId + "] = " + sIccId[slotId]);
                If (IsAllIccIdQueryDone() && sNeedUpdate) {
                    UpdateSimInfoByIccId();
                }
                break;
            case EVENT_ICC_CHANGED:
                Integer cardIndex = new Integer(PhoneConstants.DEFAULT_CARD_INDEX);
                If (ar.result != NULL) {
                    cardIndex = (Integer) ar.result;
                } else {
                    Rlog->E(LOG_TAG, "Error: Invalid card index EVENT_ICC_CHANGED ");
                    return;
                }
                UpdateIccAvailability(cardIndex);
                break;
            case EVENT_STACK_READY:
                Logd("EVENT_STACK_READY" );
                If (IsAllIccIdQueryDone() && PROJECT_SIM_NUM > 1) {
                    SubscriptionHelper->GetInstance()->UpdateSubActivation(sInsertSimState, TRUE);
                }
                break;
            default:
                Logd("Unknown msg:" + msg.what);
        }
    }

    private void UpdateIccAvailability(Int32 slotId) {
        If (NULL == mUiccController) {
            return;
        }
        SubscriptionHelper subHelper = SubscriptionHelper->GetInstance();
        Logd("updateIccAvailability: Enter, slotId " + slotId);
        If (PROJECT_SIM_NUM > 1 && !subHelper->ProceedToHandleIccEvent(slotId)) {
            Logd("updateIccAvailability: radio is OFF/unavailable, ignore ");
            If (!subHelper->IsApmSIMNotPwdn()) {
                // set the iccid to NULL so that once SIM card detected
                //  ICCID will be read from the card again.
                sIccId[slotId] = NULL;
            }
            return;
        }

        CardState newState = CardState.CARDSTATE_ABSENT;
        UiccCard newCard = mUiccController->GetUiccCard(slotId);
        If (newCard != NULL) {
            newState = newCard->GetCardState();
        }
        CardState oldState = sCardState[slotId];
        sCardState[slotId] = newState;
        Logd("Slot[" + slotId + "]: New Card State = "
                + newState + " " + "Old Card State = " + oldState);
        If (!newState->IsCardPresent()) {
            //Card moved to ABSENT State
            If (sIccId[slotId] != NULL && !sIccId[slotId].Equals(ICCID_STRING_FOR_NO_SIM)) {
                Logd("SIM" + (slotId + 1) + " hot plug out");
                sNeedUpdate = TRUE;
            }
            sFh[slotId] = NULL;
            sIccId[slotId] = ICCID_STRING_FOR_NO_SIM;
            If (IsAllIccIdQueryDone() && sNeedUpdate) {
                UpdateSimInfoByIccId();
            }
        } else If (!oldState->IsCardPresent() && newState->IsCardPresent()) {
            // Card moved to PRESENT State.
            If (sIccId[slotId] != NULL && sIccId[slotId].Equals(ICCID_STRING_FOR_NO_SIM)) {
                Logd("SIM" + (slotId + 1) + " hot plug in");
                sIccId[slotId] = NULL;
                sNeedUpdate = TRUE;
            }
            QueryIccId(slotId);
        } else If (oldState->IsCardPresent() && newState->IsCardPresent() &&
                (!subHelper->IsApmSIMNotPwdn()) && (sIccId[slotId] == NULL)) {
            Logd("SIM" + (slotId + 1) + " powered up from APM ");
            sFh[slotId] = NULL;
            sNeedUpdate = TRUE;
            QueryIccId(slotId);
        }
    }

    private void QueryIccId(Int32 slotId) {
        Logd("queryIccId: slotid=" + slotId);
        If (sFh[slotId] == NULL) {
            Logd("Getting IccFileHandler");

            UiccCardApplication validApp = NULL;
            UiccCard uiccCard = mUiccController->GetUiccCard(slotId);
            Int32 numApps = uiccCard->GetNumApplications();
            For (Int32 i = 0; i < numApps; i++) {
                UiccCardApplication app = uiccCard->GetApplicationIndex(i);
                If (app != NULL && app->GetType() != AppType.APPTYPE_UNKNOWN) {
                    validApp = app;
                    break;
                }
            }
            If (validApp != NULL) sFh[slotId] = validApp->GetIccFileHandler();
        }

        If (sFh[slotId] != NULL) {
            String iccId = sIccId[slotId];
            If (iccId == NULL) {
                Logd("Querying IccId");
                sFh[slotId].LoadEFTransparent(IccConstants.EF_ICCID, ObtainMessage(EncodeEventId(EVENT_QUERY_ICCID_DONE, slotId)));
            } else {
                Logd("NOT Querying IccId its already set sIccid[" + slotId + "]=" + iccId);
            }
        } else {
            //Reset to CardState to ABSENT so that on next EVENT_ICC_CHANGED, ICCID can be read.
            sCardState[slotId] = CardState.CARDSTATE_ABSENT;
            Logd("sFh[" + slotId + "] is NULL, SIM not inserted");
        }
    }

    CARAPI UpdateSubIdForNV(Int32 slotId) {
        sIccId[slotId] = ICCID_STRING_FOR_NV;
        sNeedUpdate = TRUE;
        Logd("[updateSubIdForNV]+ Start");
        If (IsAllIccIdQueryDone()) {
            Logd("[updateSubIdForNV]+ updating");
            UpdateSimInfoByIccId();
            isNVSubAvailable = TRUE;
        }
    }

    synchronized CARAPI UpdateSimInfoByIccId() {
        Logd("[updateSimInfoByIccId]+ Start");
        sNeedUpdate = FALSE;

        SubscriptionManager->ClearSubInfo();

        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            sInsertSimState[i] = SIM_NOT_CHANGE;
        }

        Int32 insertedSimCount = PROJECT_SIM_NUM;
        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            If (ICCID_STRING_FOR_NO_SIM->Equals(sIccId[i])) {
                insertedSimCount--;
                sInsertSimState[i] = SIM_NOT_INSERT;
            }
        }
        Logd("insertedSimCount = " + insertedSimCount);

        Int32 index = 0;
        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            If (sInsertSimState[i] == SIM_NOT_INSERT) {
                continue;
            }
            index = 2;
            For (Int32 j = i + 1; j < PROJECT_SIM_NUM; j++) {
                If (sInsertSimState[j] == SIM_NOT_CHANGE && sIccId[i].Equals(sIccId[j])) {
                    sInsertSimState[i] = 1;
                    sInsertSimState[j] = index;
                    index++;
                }
            }
        }

        ContentResolver contentResolver = sContext->GetContentResolver();
        String[] oldIccId = new String[PROJECT_SIM_NUM];
        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            oldIccId[i] = NULL;
            List<SubInfoRecord> oldSubInfo =
                    SubscriptionController->GetInstance()->GetSubInfoUsingSlotIdWithCheck(i, FALSE);
            If (oldSubInfo != NULL) {
                oldIccId[i] = oldSubInfo->Get(0).iccId;
                Logd("oldSubId = " + oldSubInfo->Get(0).subId);
                If (sInsertSimState[i] == SIM_NOT_CHANGE && !sIccId[i].Equals(oldIccId[i])) {
                    sInsertSimState[i] = SIM_CHANGED;
                }
                If (sInsertSimState[i] != SIM_NOT_CHANGE) {
                    ContentValues value = new ContentValues(1);
                    value->Put(SubscriptionManager.SIM_ID, SubscriptionManager.INVALID_SLOT_ID);
                    contentResolver->Update(SubscriptionManager.CONTENT_URI, value,
                            SubscriptionManager._ID + "="
                            + Long->ToString(oldSubInfo->Get(0).subId), NULL);
                }
            } else {
                If (sInsertSimState[i] == SIM_NOT_CHANGE) {
                    // no SIM inserted last time, but there is one SIM inserted now
                    sInsertSimState[i] = SIM_CHANGED;
                }
                oldIccId[i] = ICCID_STRING_FOR_NO_SIM;
                Logd("No SIM in slot " + i + " last time");
            }
        }

        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            Logd("oldIccId[" + i + "] = " + oldIccId[i] + ", sIccId[" + i + "] = " + sIccId[i]);
        }

        //check if the inserted SIM is new SIM
        Int32 nNewCardCount = 0;
        Int32 nNewSimStatus = 0;
        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            If (sInsertSimState[i] == SIM_NOT_INSERT) {
                Logd("No SIM inserted in slot " + i + " this time");
            } else {
                If (sInsertSimState[i] > 0) {
                    //some special SIMs may have the same IccIds, add suffix to distinguish them
                    //FIXME: addSubInfoRecord can return an error.
                    SubscriptionManager->AddSubInfoRecord(sIccId[i]
                            + Integer->ToString(sInsertSimState[i]), i);
                    Logd("SUB" + (i + 1) + " has invalid IccId");
                } else /*If (sInsertSimState[i] != SIM_NOT_INSERT)*/ {
                    SubscriptionManager->AddSubInfoRecord(sIccId[i], i);
                }
                If (IsNewSim(sIccId[i], oldIccId)) {
                    nNewCardCount++;
                    Switch (i) {
                        case PhoneConstants.SUB1:
                            nNewSimStatus |= STATUS_SIM1_INSERTED;
                            break;
                        case PhoneConstants.SUB2:
                            nNewSimStatus |= STATUS_SIM2_INSERTED;
                            break;
                        case PhoneConstants.SUB3:
                            nNewSimStatus |= STATUS_SIM3_INSERTED;
                            break;
                        //case PhoneConstants.SUB3:
                        //    nNewSimStatus |= STATUS_SIM4_INSERTED;
                        //    break;
                    }

                    sInsertSimState[i] = SIM_NEW;
                }
            }
        }

        For (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            If (sInsertSimState[i] == SIM_CHANGED) {
                sInsertSimState[i] = SIM_REPOSITION;
            }
        }
        SubscriptionHelper->GetInstance()->UpdateNwMode();
        If (ModemStackController->GetInstance()->IsStackReady() && PROJECT_SIM_NUM > 1) {
            SubscriptionHelper->GetInstance()->UpdateSubActivation(sInsertSimState, FALSE);
        }

        List<SubInfoRecord> subInfos = SubscriptionManager->GetActiveSubInfoList();
        Int32 nSubCount = (subInfos == NULL) ? 0 : subInfos->Size();
        Logd("nSubCount = " + nSubCount);
        For (Int32 i=0; i<nSubCount; i++) {
            SubInfoRecord temp = subInfos->Get(i);

            String msisdn = TelephonyManager->GetDefault()->GetLine1NumberForSubscriber(temp.subId);

            If (msisdn != NULL) {
                ContentValues value = new ContentValues(1);
                value->Put(SubscriptionManager.NUMBER, msisdn);
                contentResolver->Update(SubscriptionManager.CONTENT_URI, value,
                        SubscriptionManager._ID + "=" + Long->ToString(temp.subId), NULL);
            }
        }

        // TRUE if any slot has no SIM this time, but has SIM last time
        Boolean hasSimRemoved = FALSE;
        For (Int32 i=0; i < PROJECT_SIM_NUM; i++) {
            If (sIccId[i] != NULL && sIccId[i].Equals(ICCID_STRING_FOR_NO_SIM)
                    && !oldIccId[i].Equals("")) {
                hasSimRemoved = TRUE;
                break;
            }
        }

        If (nNewCardCount == 0) {
            Int32 i;
            If (hasSimRemoved) {
                // no new SIM, at least one SIM is removed, check if any SIM is repositioned first
                For (i=0; i < PROJECT_SIM_NUM; i++) {
                    If (sInsertSimState[i] == SIM_REPOSITION) {
                        Logd("No new SIM detected and SIM repositioned");
                        SetUpdatedData(SubscriptionManager.EXTRA_VALUE_REPOSITION_SIM,
                                nSubCount, nNewSimStatus);
                        break;
                    }
                }
                If (i == PROJECT_SIM_NUM) {
                    // no new SIM, no SIM is repositioned => at least one SIM is removed
                    Logd("No new SIM detected and SIM removed");
                    SetUpdatedData(SubscriptionManager.EXTRA_VALUE_REMOVE_SIM,
                            nSubCount, nNewSimStatus);
                }
            } else {
                // no SIM is removed, no new SIM, just check if any SIM is repositioned
                For (i=0; i< PROJECT_SIM_NUM; i++) {
                    If (sInsertSimState[i] == SIM_REPOSITION) {
                        Logd("No new SIM detected and SIM repositioned");
                        SetUpdatedData(SubscriptionManager.EXTRA_VALUE_REPOSITION_SIM,
                                nSubCount, nNewSimStatus);
                        break;
                    }
                }
                If (i == PROJECT_SIM_NUM) {
                    // all status remain unchanged
                    Logd("[updateSimInfoByIccId] All SIM inserted into the same slot");
                    SetUpdatedData(SubscriptionManager.EXTRA_VALUE_NOCHANGE,
                            nSubCount, nNewSimStatus);
                }
            }
        } else {
            Logd("New SIM detected");
            SetUpdatedData(SubscriptionManager.EXTRA_VALUE_NEW_SIM, nSubCount, nNewSimStatus);
        }

        Logd("[updateSimInfoByIccId]- SimInfo update complete");
    }

    private static void SetUpdatedData(Int32 detectedType, Int32 subCount, Int32 newSimStatus) {

        Intent intent = new Intent(TelephonyIntents.ACTION_SUBINFO_RECORD_UPDATED);

        Logd("[setUpdatedData]+ ");

        If (detectedType == SubscriptionManager.EXTRA_VALUE_NEW_SIM ) {
            intent->PutExtra(SubscriptionManager.INTENT_KEY_DETECT_STATUS,
                    SubscriptionManager.EXTRA_VALUE_NEW_SIM);
            intent->PutExtra(SubscriptionManager.INTENT_KEY_SIM_COUNT, subCount);
            intent->PutExtra(SubscriptionManager.INTENT_KEY_NEW_SIM_SLOT, newSimStatus);
        } else If (detectedType == SubscriptionManager.EXTRA_VALUE_REPOSITION_SIM) {
            intent->PutExtra(SubscriptionManager.INTENT_KEY_DETECT_STATUS,
                    SubscriptionManager.EXTRA_VALUE_REPOSITION_SIM);
            intent->PutExtra(SubscriptionManager.INTENT_KEY_SIM_COUNT, subCount);
        } else If (detectedType == SubscriptionManager.EXTRA_VALUE_REMOVE_SIM) {
            intent->PutExtra(SubscriptionManager.INTENT_KEY_DETECT_STATUS,
                    SubscriptionManager.EXTRA_VALUE_REMOVE_SIM);
            intent->PutExtra(SubscriptionManager.INTENT_KEY_SIM_COUNT, subCount);
        } else If (detectedType == SubscriptionManager.EXTRA_VALUE_NOCHANGE) {
            intent->PutExtra(SubscriptionManager.INTENT_KEY_DETECT_STATUS,
                    SubscriptionManager.EXTRA_VALUE_NOCHANGE);
        }

        Logd("broadcast intent ACTION_SUBINFO_RECORD_UPDATED : [" + detectedType + ", "
                + subCount + ", " + newSimStatus+ "]");
        ActivityManagerNative->BroadcastStickyIntent(intent, READ_PHONE_STATE, UserHandle.USER_ALL);
        Logd("[setUpdatedData]- ");
    }

    private static Boolean IsNewSim(String iccId, String[] oldIccId) {
        Boolean newSim = TRUE;
        For(Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
            If(iccId->Equals(oldIccId[i])) {
                newSim = FALSE;
                break;
            }
        }
        Logd("newSim = " + newSim);

        return newSim;
    }

    CARAPI Dispose() {
        Logd("[dispose]");
        sContext->UnregisterReceiver(sReceiver);
    }

    private static void Logd(String message) {
        Rlog->D(LOG_TAG, "[SubInfoRecordUpdater]" + message);
    }
}

