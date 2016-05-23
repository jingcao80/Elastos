/*
 * Copyright (C) 2011-2012 The Android Open Source Project
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

package com.android.internal.telephony.uicc;

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Telephony::IServiceState;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Nio::IByteBuffer;
using Elastos::Nio::IByteOrder;

/**
 * This class is responsible for keeping all knowledge about
 * Universal Integrated Circuit Card (UICC), also know as SIM's,
 * in the system. It is also used as API to get appropriate
 * applications to pass them to phone and service trackers.
 *
 * UiccController is created with the call to Make() function.
 * UiccController is a singleton and Make() must only be called once
 * and throws an exception if called multiple times.
 *
 * Once created UiccController registers with RIL for "on" and "unsol_sim_status_changed"
 * notifications. When such notification arrives UiccController will call
 * GetIccCardStatus (GET_SIM_STATUS). Based on the response of GET_SIM_STATUS
 * request appropriate tree of uicc objects will be created.
 *
 * Following is class diagram for uicc classes:
 *
 *                       UiccController
 *                            #
 *                            |
 *                        UiccCard
 *                          #   #
 *                          |   ------------------
 *                    UiccCardApplication    CatService
 *                      #            #
 *                      |            |
 *                 IccRecords    IccFileHandler
 *                 ^ ^ ^           ^ ^ ^ ^ ^
 *    SIMRecords---- | |           | | | | ---SIMFileHandler
 *    RuimRecords----- |           | | | ----RuimFileHandler
 *    IsimUiccRecords---           | | -----UsimFileHandler
 *                                 | ------CsimFileHandler
 *                                 ----IsimFileHandler
 *
 * Legend: # stands for Composition
 *         ^ stands for Generalization
 *
 * See also {@link com.android.internal.telephony.IccCard}
 * and {@link com.android.internal.telephony.uicc.IccCardProxy}
 */
public class UiccController extends Handler {
    private static const Boolean DBG = TRUE;
    private static const String LOG_TAG = "UiccController";

    public static const Int32 APP_FAM_UNKNOWN =  -1;
    public static const Int32 APP_FAM_3GPP =  1;
    public static const Int32 APP_FAM_3GPP2 = 2;
    public static const Int32 APP_FAM_IMS   = 3;

    private static const Int32 EVENT_ICC_STATUS_CHANGED = 1;
    private static const Int32 EVENT_GET_ICC_STATUS_DONE = 2;
    private static const Int32 EVENT_RADIO_UNAVAILABLE = 3;
    private static const Int32 EVENT_REFRESH = 4;
    private static const Int32 EVENT_REFRESH_OEM = 5;

    private CommandsInterface[] mCis;
    private UiccCard[] mUiccCards = new UiccCard[TelephonyManager->GetDefault()->GetPhoneCount()];

    private static const Object mLock = new Object();
    private static UiccController mInstance;

    private Context mContext;
/*
    private CommandsInterface mCi;
    private UiccCard mUiccCard;
*/

    protected RegistrantList mIccChangedRegistrants = new RegistrantList();

    private Boolean mOEMHookSimRefresh = FALSE;

/*
    public static UiccController Make(Context c, CommandsInterface ci) {
        {    AutoLock syncLock(mLock);
            If (mInstance != NULL) {
                throw new RuntimeException("UiccController->Make() should only be called once");
            }
            mInstance = new UiccController(c, ci);
            return mInstance;
        }
    }
*/

    public static UiccController Make(Context c, CommandsInterface[] ci) {
        {    AutoLock syncLock(mLock);
            If (mInstance != NULL) {
                throw new RuntimeException("MSimUiccController->Make() should only be called once");
            }
            mInstance = new UiccController(c, ci);
            Return (UiccController)mInstance;
        }
    }

    private UiccController(Context c, CommandsInterface []ci) {
        If (DBG) Log("Creating UiccController");
        mContext = c;
        mCis = ci;
        mOEMHookSimRefresh = mContext->GetResources()->GetBoolean(
                R.bool.config_sim_refresh_for_dual_mode_card);
        For (Int32 i = 0; i < mCis.length; i++) {
            Integer index = new Integer(i);
            If (SystemProperties->GetBoolean("persist.radio.apm_sim_not_pwdn", FALSE)) {
                // Reading ICC status in airplane mode is only supported in QCOM
                // RILs when this property is set to TRUE
                mCis[i].RegisterForAvailable(this, EVENT_ICC_STATUS_CHANGED, index);
            } else {
                mCis[i].RegisterForOn(this, EVENT_ICC_STATUS_CHANGED, index);
            }

            mCis[i].RegisterForIccStatusChanged(this, EVENT_ICC_STATUS_CHANGED, index);
            // TODO remove this once modem correctly notifies the unsols
            mCis[i].RegisterForNotAvailable(this, EVENT_RADIO_UNAVAILABLE, index);
            If (mOEMHookSimRefresh) {
                mCis[i].RegisterForSimRefreshEvent(this, EVENT_REFRESH_OEM, index);
            } else {
                mCis[i].RegisterForIccRefresh(this, EVENT_REFRESH, index);
            }
        }
    }

    public static UiccController GetInstance() {
        {    AutoLock syncLock(mLock);
            If (mInstance == NULL) {
                throw new RuntimeException(
                        "UiccController.getInstance can't be called before Make()");
            }
            return mInstance;
        }
    }

    public UiccCard GetUiccCard() {
        return GetUiccCard(SubscriptionController->GetInstance()->GetPhoneId(SubscriptionController->GetInstance()->GetDefaultSubId()));
    }

    public UiccCard GetUiccCard(Int32 slotId) {
        {    AutoLock syncLock(mLock);
            If (IsValidCardIndex(slotId)) {
                return mUiccCards[slotId];
            }
            return NULL;
        }
    }

    public UiccCard[] GetUiccCards() {
        // Return cloned array since we don't want to give out reference
        // to internal data structure.
        {    AutoLock syncLock(mLock);
            return mUiccCards->Clone();
        }
    }

    // Easy to use API
    public UiccCardApplication GetUiccCardApplication(Int32 family) {
        return GetUiccCardApplication(SubscriptionController->GetInstance()->GetPhoneId(
                SubscriptionController->GetInstance()->GetDefaultSubId()), family);
    }

/*
    // Easy to use API
    public IccRecords GetIccRecords(Int32 family) {
        {    AutoLock syncLock(mLock);
            If (mUiccCard != NULL) {
                UiccCardApplication app = mUiccCard->GetApplication(family);
                If (app != NULL) {
                    return app->GetIccRecords();
                }
            }
            return NULL;
        }
    }
*/

    // Easy to use API
    public IccRecords GetIccRecords(Int32 slotId, Int32 family) {
        {    AutoLock syncLock(mLock);
            UiccCardApplication app = GetUiccCardApplication(slotId, family);
            If (app != NULL) {
                return app->GetIccRecords();
            }
            return NULL;
        }
    }

/*
    // Easy to use API
    public IccFileHandler GetIccFileHandler(Int32 family) {
        {    AutoLock syncLock(mLock);
            If (mUiccCard != NULL) {
                UiccCardApplication app = mUiccCard->GetApplication(family);
                If (app != NULL) {
                    return app->GetIccFileHandler();
                }
            }
            return NULL;
        }
    }
*/

    // Easy to use API
    public IccFileHandler GetIccFileHandler(Int32 slotId, Int32 family) {
        {    AutoLock syncLock(mLock);
            UiccCardApplication app = GetUiccCardApplication(slotId, family);
            If (app != NULL) {
                return app->GetIccFileHandler();
            }
            return NULL;
        }
    }


    public static Int32 GetFamilyFromRadioTechnology(Int32 radioTechnology) {
        If (ServiceState->IsGsm(radioTechnology) ||
                radioTechnology == ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD) {
            return  UiccController.APP_FAM_3GPP;
        } else If (ServiceState->IsCdma(radioTechnology)) {
            return  UiccController.APP_FAM_3GPP2;
        } else {
            // If it is UNKNOWN rat
            return UiccController.APP_FAM_UNKNOWN;
        }
    }

    //Notifies when card status changes
    CARAPI RegisterForIccChanged(Handler h, Int32 what, Object obj) {
        {    AutoLock syncLock(mLock);
            Registrant r = new Registrant (h, what, obj);
            mIccChangedRegistrants->Add(r);
            //Notify registrant right after registering, so that it will get the latest ICC status,
            //otherwise which may not happen until there is an actual change in ICC status.
            r->NotifyRegistrant();
        }
    }

    CARAPI UnregisterForIccChanged(Handler h) {
        {    AutoLock syncLock(mLock);
            mIccChangedRegistrants->Remove(h);
        }
    }

    //@Override
    CARAPI HandleMessage (Message msg) {
        {    AutoLock syncLock(mLock);
            Integer index = GetCiIndex(msg);

            If (index < 0 || index >= mCis.length) {
                Rlog->E(LOG_TAG, "Invalid index : " + index + " received with event " + msg.what);
                return;
            }

            Switch (msg.what) {
                case EVENT_ICC_STATUS_CHANGED:
                    If (DBG) Log("Received EVENT_ICC_STATUS_CHANGED, calling getIccCardStatus");
                    mCis[index].GetIccCardStatus(ObtainMessage(EVENT_GET_ICC_STATUS_DONE, index));
                    break;
                case EVENT_GET_ICC_STATUS_DONE:
                    If (DBG) Log("Received EVENT_GET_ICC_STATUS_DONE");
                    AsyncResult ar = (AsyncResult)msg.obj;
                    OnGetIccCardStatusDone(ar, index);
                    break;
                case EVENT_RADIO_UNAVAILABLE:
                    If (DBG) Log("EVENT_RADIO_UNAVAILABLE, dispose card");
                    If (mUiccCards[index] != NULL) {
                        mUiccCards[index].Dispose();
                    }
                    mUiccCards[index] = NULL;
                    mIccChangedRegistrants->NotifyRegistrants(new AsyncResult(NULL, index, NULL));
                    break;
                case EVENT_REFRESH:
                    ar = (AsyncResult)msg.obj;
                    If (DBG) Log("Sim REFRESH received");
                    If (ar.exception == NULL) {
                        HandleRefresh((IccRefreshResponse)ar.result, index);
                    } else {
                        Log ("Exception on refresh " + ar.exception);
                    }
                    break;
                case EVENT_REFRESH_OEM:
                    ar = (AsyncResult)msg.obj;
                    If (DBG) Log("Sim REFRESH OEM received");
                    If (ar.exception == NULL) {
                        ByteBuffer payload = ByteBuffer->Wrap((Byte[])ar.result);
                        HandleRefresh(ParseOemSimRefresh(payload), index);
                    } else {
                        Log ("Exception on refresh " + ar.exception);
                    }
                    break;
                default:
                    Rlog->E(LOG_TAG, " Unknown Event " + msg.what);
            }
        }
    }

    private Integer GetCiIndex(Message msg) {
        AsyncResult ar;
        Integer index = new Integer(PhoneConstants.DEFAULT_CARD_INDEX);

        /*
         * The events can be come in two ways. By explicitly sending it using
         * sendMessage, in this case the user object passed is msg.obj and from
         * the CommandsInterface, in this case the user object is msg.obj.userObj
         */
        If (msg != NULL) {
            If (msg.obj != NULL && msg.obj instanceof Integer) {
                index = (Integer)msg.obj;
            } else If(msg.obj != NULL && msg.obj instanceof AsyncResult) {
                ar = (AsyncResult)msg.obj;
                If (ar.userObj != NULL && ar.userObj instanceof Integer) {
                    index = (Integer)ar.userObj;
                }
            }
        }
        return index;
    }

    private void HandleRefresh(IccRefreshResponse refreshResponse, Int32 index){
        If (refreshResponse == NULL) {
            Log("handleRefresh received without input");
            return;
        }

        // Let the card know right away that a refresh has occurred
        If (mUiccCards[index] != NULL) {
            mUiccCards[index].OnRefresh(refreshResponse);
        }
        // The card status could have changed. Get the latest state
        mCis[index].GetIccCardStatus(ObtainMessage(EVENT_GET_ICC_STATUS_DONE));
    }

    public static IccRefreshResponse
    ParseOemSimRefresh(ByteBuffer payload) {
        IccRefreshResponse response = new IccRefreshResponse();
        /* AID maximum size */
        final Int32 QHOOK_MAX_AID_SIZE = 20*2+1+3;

        /* parse from payload */
        payload->Order(ByteOrder->NativeOrder());

        response.refreshResult = payload->GetInt();
        response.efId  = payload->GetInt();
        Int32 aidLen = payload->GetInt();
        Byte[] aid = new Byte[QHOOK_MAX_AID_SIZE];
        payload->Get(aid, 0, QHOOK_MAX_AID_SIZE);
        //Read the aid string with QHOOK_MAX_AID_SIZE from payload at first because need
        //corresponding to the aid array length sent from qcril and need parse the payload
        //to get app type and sub id in IccRecords.java after called this method.
        response.aid = (aidLen == 0) ? NULL : (new String(aid)).Substring(0, aidLen);

        If (DBG){
            Rlog->D(LOG_TAG, "refresh SIM card " + ", refresh result:" + response.refreshResult
                    + ", ef Id:" + response.efId + ", aid:" + response.aid);
        }
        return response;
    }
/*
    private UiccController(Context c, CommandsInterface ci) {
        If (DBG) Log("Creating UiccController");
        mContext = c;
        mCi = ci;
        mCi->RegisterForIccStatusChanged(this, EVENT_ICC_STATUS_CHANGED, NULL);
        // This is needed so that we query for sim status in the case when we boot in APM
        mCi->RegisterForAvailable(this, EVENT_ICC_STATUS_CHANGED, NULL);
    }
*/
    // Easy to use API
    public UiccCardApplication GetUiccCardApplication(Int32 slotId, Int32 family) {
        {    AutoLock syncLock(mLock);
            If (IsValidCardIndex(slotId)) {
                UiccCard c = mUiccCards[slotId];
                If (c != NULL) {
                    return mUiccCards[slotId].GetApplication(family);
                }
            }
            return NULL;
        }
    }

    private synchronized void OnGetIccCardStatusDone(AsyncResult ar, Integer index) {
        If (ar.exception != NULL) {
            Rlog->E(LOG_TAG,"Error getting ICC status. "
                    + "RIL_REQUEST_GET_ICC_STATUS should "
                    + "never return an error", ar.exception);
            return;
        }
        If (!IsValidCardIndex(index)) {
            Rlog->E(LOG_TAG,"onGetIccCardStatusDone: invalid index : " + index);
            return;
        }

        IccCardStatus status = (IccCardStatus)ar.result;

        If (mUiccCards[index] == NULL) {
            //Create new card
            mUiccCards[index] = new UiccCard(mContext, mCis[index], status, index);

/*
            // Update the UiccCard in base class, so that if someone calls
            // UiccManager->GetUiccCard(), it will return the default card.
            If (index == PhoneConstants.DEFAULT_CARD_INDEX) {
                mUiccCard = mUiccCards[index];
            }
*/
        } else {
            //Update already existing card
            mUiccCards[index].Update(mContext, mCis[index] , status);
        }

        If (DBG) Log("Notifying IccChangedRegistrants");
        mIccChangedRegistrants->NotifyRegistrants(new AsyncResult(NULL, index, NULL));

    }

    private Boolean IsValidCardIndex(Int32 index) {
        Return (index >= 0 && index < mUiccCards.length);
    }

    private void Log(String string) {
        Rlog->D(LOG_TAG, string);
    }


    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("UiccController: " + this);
        pw->Println(" mContext=" + mContext);
        pw->Println(" mInstance=" + mInstance);
//        pw->Println(" mCi=" + mCi);
//        pw->Println(" mUiccCard=" + mUiccCard);
        pw->Println(" mIccChangedRegistrants: size=" + mIccChangedRegistrants->Size());
        For (Int32 i = 0; i < mIccChangedRegistrants->Size(); i++) {
            pw->Println("  mIccChangedRegistrants[" + i + "]="
                    + ((Registrant)mIccChangedRegistrants->Get(i)).GetHandler());
        }
        pw->Println();
        pw->Flush();
//        For (Int32 i = 0; i < mUiccCards.length; i++) {
//            mUiccCards[i].Dump(fd, pw, args);
//        }
    }
}
