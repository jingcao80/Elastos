/*
 * Copyright (C) 2006, 2012 The Android Open Source Project
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

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppType;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IPersoSubState;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardStatus::IPinState;
using Elastos::Droid::Internal::Telephony::Uicc::IUICCConfig;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

/**
 * {@hide}
 */
public class UiccCardApplication {
    private static const String LOG_TAG = "UiccCardApplication";
    private static const Boolean DBG = TRUE;

    private static const Int32 EVENT_PIN1_PUK1_DONE = 1;
    private static const Int32 EVENT_CHANGE_PIN1_DONE = 2;
    private static const Int32 EVENT_CHANGE_PIN2_DONE = 3;
    private static const Int32 EVENT_QUERY_FACILITY_FDN_DONE = 4;
    private static const Int32 EVENT_CHANGE_FACILITY_FDN_DONE = 5;
    private static const Int32 EVENT_QUERY_FACILITY_LOCK_DONE = 6;
    private static const Int32 EVENT_CHANGE_FACILITY_LOCK_DONE = 7;
    private static const Int32 EVENT_PIN2_PUK2_DONE = 8;

    /**
     * These values are for AuthContext (parameter P2) per 3GPP TS 31.102 (Section 7.1.2)
     */
    public static const Int32 AUTH_CONTEXT_EAP_SIM = 128;
    public static const Int32 AUTH_CONTEXT_EAP_AKA = 129;
    public static const Int32 AUTH_CONTEXT_UNDEFINED = -1;

    private final Object  mLock = new Object();
    private UiccCard      mUiccCard; //parent
    private AppState      mAppState;
    private AppType       mAppType;
    private Int32           mAuthContext;
    private PersoSubState mPersoSubState;
    private String        mAid;
    private String        mAppLabel;
    private Boolean       mPin1Replaced;
    private PinState      mPin1State;
    private PinState      mPin2State;
    private Boolean       mIccFdnEnabled;
    private Boolean       mDesiredFdnEnabled;
    private Boolean       mIccLockEnabled;
    private Boolean       mDesiredPinLocked;
    private Boolean       mIccFdnAvailable = TRUE; // Default is enabled.

    private CommandsInterface mCi;
    private Context mContext;
    private IccRecords mIccRecords;
    private IccFileHandler mIccFh;

    private Boolean mDestroyed;//set to TRUE once this App is commanded to be disposed of.

    private RegistrantList mReadyRegistrants = new RegistrantList();
    private RegistrantList mPinLockedRegistrants = new RegistrantList();
    private RegistrantList mPersoLockedRegistrants = new RegistrantList();

    UiccCardApplication(UiccCard uiccCard,
                        IccCardApplicationStatus as,
                        Context c,
                        CommandsInterface ci) {
        If (DBG) Log("Creating UiccApp: " + as);
        mUiccCard = uiccCard;
        mAppState = as.app_state;
        mAppType = as.app_type;
        mAuthContext = GetAuthContext(mAppType);
        mPersoSubState = as.perso_substate;
        mAid = as.aid;
        mAppLabel = as.app_label;
        mPin1Replaced = (as.pin1_replaced != 0);
        mPin1State = as.pin1;
        mPin2State = as.pin2;

        mContext = c;
        mCi = ci;

        mIccFh = CreateIccFileHandler(as.app_type);
        mIccRecords = CreateIccRecords(as.app_type, mContext, mCi);
        If (mAppState == AppState.APPSTATE_READY) {
            QueryFdn();
            QueryPin1State();
        }
    }

    void Update (IccCardApplicationStatus as, Context c, CommandsInterface ci) {
        Synchronized (mLock) {
            If (mDestroyed) {
                Loge("Application updated after destroyed! Fix me!");
                return;
            }

            If (DBG) Log(mAppType + " update. New " + as);
            mContext = c;
            mCi = ci;
            AppType oldAppType = mAppType;
            AppState oldAppState = mAppState;
            PersoSubState oldPersoSubState = mPersoSubState;
            mAppType = as.app_type;
            mAuthContext = GetAuthContext(mAppType);
            mAppState = as.app_state;
            mPersoSubState = as.perso_substate;
            mAid = as.aid;
            mAppLabel = as.app_label;
            mPin1Replaced = (as.pin1_replaced != 0);
            mPin1State = as.pin1;
            mPin2State = as.pin2;

            If (mAppType != oldAppType) {
                If (mIccFh != NULL) { mIccFh->Dispose();}
                If (mIccRecords != NULL) { mIccRecords->Dispose();}
                mIccFh = CreateIccFileHandler(as.app_type);
                mIccRecords = CreateIccRecords(as.app_type, c, ci);
            }

            If (mPersoSubState != oldPersoSubState &&
                    IsPersoLocked()) {
                NotifyPersoLockedRegistrantsIfNeeded(NULL);
            }

            If (mAppState != oldAppState) {
                If (DBG) Log(oldAppType + " changed state: " + oldAppState + " -> " + mAppState);
                // If the app state turns to APPSTATE_READY, then query FDN status,
                //as it might have failed in earlier attempt.
                If (mAppState == AppState.APPSTATE_READY) {
                    QueryFdn();
                    QueryPin1State();
                }
                NotifyPinLockedRegistrantsIfNeeded(NULL);
                NotifyReadyRegistrantsIfNeeded(NULL);
            }
        }
    }

    void Dispose() {
        Synchronized (mLock) {
            If (DBG) Log(mAppType + " being Disposed");
            mDestroyed = TRUE;
            If (mIccRecords != NULL) { mIccRecords->Dispose();}
            If (mIccFh != NULL) { mIccFh->Dispose();}
            mIccRecords = NULL;
            mIccFh = NULL;
        }
    }

    private IccRecords CreateIccRecords(AppType type, Context c, CommandsInterface ci) {
        If (type == AppType.APPTYPE_USIM || type == AppType.APPTYPE_SIM) {
            return new SIMRecords(this, c, ci);
        } else If (type == AppType.APPTYPE_RUIM || type == AppType.APPTYPE_CSIM){
            return new RuimRecords(this, c, ci);
        } else If (type == AppType.APPTYPE_ISIM) {
            return new IsimUiccRecords(this, c, ci);
        } else {
            // Unknown app Type (maybe detection is still in progress)
            return NULL;
        }
    }

    private IccFileHandler CreateIccFileHandler(AppType type) {
        Switch (type) {
            case APPTYPE_SIM:
                return new SIMFileHandler(this, mAid, mCi);
            case APPTYPE_RUIM:
                return new RuimFileHandler(this, mAid, mCi);
            case APPTYPE_USIM:
                return new UsimFileHandler(this, mAid, mCi);
            case APPTYPE_CSIM:
                return new CsimFileHandler(this, mAid, mCi);
            case APPTYPE_ISIM:
                return new IsimFileHandler(this, mAid, mCi);
            default:
                return NULL;
        }
    }

    /** Assumes mLock is held. */
    void QueryFdn() {
        //This shouldn't change run-time. So needs to be called only once.
        Int32 serviceClassX;

        serviceClassX = CommandsInterface.SERVICE_CLASS_VOICE +
                        CommandsInterface.SERVICE_CLASS_DATA +
                        CommandsInterface.SERVICE_CLASS_FAX;
        mCi.QueryFacilityLockForApp (
                CommandsInterface.CB_FACILITY_BA_FD, "", serviceClassX,
                mAid, mHandler->ObtainMessage(EVENT_QUERY_FACILITY_FDN_DONE));
    }
    /**
     * Interpret EVENT_QUERY_FACILITY_LOCK_DONE
     * @param ar is asyncResult of Query_Facility_Locked
     */
    private void OnQueryFdnEnabled(AsyncResult ar) {
        Synchronized (mLock) {
            If (ar.exception != NULL) {
                If (DBG) Log("Error in querying facility lock:" + ar.exception);
                return;
            }

            Int32[] result = (Int32[])ar.result;
            If(result.length != 0) {
                //0 - Available & Disabled, 1-Available & Enabled, 2-Unavailable.
                If (result[0] == 2) {
                    mIccFdnEnabled = FALSE;
                    mIccFdnAvailable = FALSE;
                } else {
                    mIccFdnEnabled = (result[0] == 1) ? TRUE : FALSE;
                    mIccFdnAvailable = TRUE;
                }
                Log("Query facility FDN : FDN service available: "+ mIccFdnAvailable
                        +" enabled: "  + mIccFdnEnabled);
            } else {
                Loge("Bogus facility lock response");
            }
        }
    }

    private void OnChangeFdnDone(AsyncResult ar) {
        Synchronized (mLock) {
            Int32 attemptsRemaining = -1;

            If (ar.exception == NULL) {
                mIccFdnEnabled = mDesiredFdnEnabled;
                If (DBG) Log("EVENT_CHANGE_FACILITY_FDN_DONE: " +
                        "mIccFdnEnabled=" + mIccFdnEnabled);
            } else {
                attemptsRemaining = ParsePinPukErrorResult(ar);
                Loge("Error change facility fdn with exception " + ar.exception);
            }
            Message response = (Message)ar.userObj;
            response.arg1 = attemptsRemaining;
            AsyncResult->ForMessage(response).exception = ar.exception;
            response->SendToTarget();
        }
    }

    /** REMOVE when mIccLockEnabled is not needed, assumes mLock is held */
    private void QueryPin1State() {
        Int32 serviceClassX = CommandsInterface.SERVICE_CLASS_VOICE +
                CommandsInterface.SERVICE_CLASS_DATA +
                CommandsInterface.SERVICE_CLASS_FAX;
        mCi.QueryFacilityLockForApp (
            CommandsInterface.CB_FACILITY_BA_SIM, "", serviceClassX,
            mAid, mHandler->ObtainMessage(EVENT_QUERY_FACILITY_LOCK_DONE));
    }

    /** REMOVE when mIccLockEnabled is not needed*/
    private void OnQueryFacilityLock(AsyncResult ar) {
        Synchronized (mLock) {
            If(ar.exception != NULL) {
                If (DBG) Log("Error in querying facility lock:" + ar.exception);
                return;
            }

            Int32[] ints = (Int32[])ar.result;
            If(ints.length != 0) {
                If (DBG) Log("Query facility lock : "  + ints[0]);

                mIccLockEnabled = (ints[0] != 0);

                If (mIccLockEnabled) {
                    mPinLockedRegistrants->NotifyRegistrants();
                }

                // Sanity check: we expect mPin1State to match mIccLockEnabled.
                // When mPin1State is DISABLED mIccLockEanbled should be FALSE.
                // When mPin1State is ENABLED mIccLockEnabled should be TRUE.
                //
                // Here we validate these assumptions to assist in identifying which ril/radio's
                // have not correctly implemented GET_SIM_STATUS
                Switch (mPin1State) {
                    case PINSTATE_DISABLED:
                        If (mIccLockEnabled) {
                            Loge("QUERY_FACILITY_LOCK:enabled GET_SIM_STATUS.Pin1:disabled."
                                    + " Fixme");
                        }
                        break;
                    case PINSTATE_ENABLED_NOT_VERIFIED:
                    case PINSTATE_ENABLED_VERIFIED:
                    case PINSTATE_ENABLED_BLOCKED:
                    case PINSTATE_ENABLED_PERM_BLOCKED:
                        If (!mIccLockEnabled) {
                            Loge("QUERY_FACILITY_LOCK:disabled GET_SIM_STATUS.Pin1:enabled."
                                    + " Fixme");
                        }
                    case PINSTATE_UNKNOWN:
                    default:
                        If (DBG) Log("Ignoring: pin1state=" + mPin1State);
                        break;
                }
            } else {
                Loge("Bogus facility lock response");
            }
        }
    }

    /** REMOVE when mIccLockEnabled is not needed */
    private void OnChangeFacilityLock(AsyncResult ar) {
        Synchronized (mLock) {
            Int32 attemptsRemaining = -1;

            If (ar.exception == NULL) {
                mIccLockEnabled = mDesiredPinLocked;
                If (DBG) Log( "EVENT_CHANGE_FACILITY_LOCK_DONE: mIccLockEnabled= "
                        + mIccLockEnabled);
            } else {
                attemptsRemaining = ParsePinPukErrorResult(ar);
                Loge("Error change facility lock with exception " + ar.exception);
            }
            Message response = (Message)ar.userObj;
            AsyncResult->ForMessage(response).exception = ar.exception;
            response.arg1 = attemptsRemaining;
            response->SendToTarget();
        }
    }

    /**
     * Parse the error response to obtain number of attempts remaining
     */
    private Int32 ParsePinPukErrorResult(AsyncResult ar) {
        Int32[] result = (Int32[]) ar.result;
        If (result == NULL) {
            return -1;
        } else {
            Int32 length = result.length;
            Int32 attemptsRemaining = -1;
            If (length > 0) {
                attemptsRemaining = result[0];
            }
            Log("parsePinPukErrorResult: attemptsRemaining=" + attemptsRemaining);
            return attemptsRemaining;
        }
    }

    private Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg){
            AsyncResult ar;

            If (mDestroyed) {
                Loge("Received message " + msg + "[" + msg.what
                        + "] while being destroyed. Ignoring.");
                return;
            }

            Switch (msg.what) {
                case EVENT_PIN1_PUK1_DONE:
                case EVENT_PIN2_PUK2_DONE:
                case EVENT_CHANGE_PIN1_DONE:
                case EVENT_CHANGE_PIN2_DONE:
                    // a PIN/PUK/PIN2/PUK2 complete
                    // request has completed. ar.userObj is the response Message
                    Int32 attemptsRemaining = -1;
                    ar = (AsyncResult)msg.obj;
                    If (ar.result != NULL) {
                        attemptsRemaining = ParsePinPukErrorResult(ar);
                    }
                    Message response = (Message)ar.userObj;
                    AsyncResult->ForMessage(response).exception = ar.exception;
                    response.arg1 = attemptsRemaining;
                    response->SendToTarget();
                    break;
                case EVENT_QUERY_FACILITY_FDN_DONE:
                    ar = (AsyncResult)msg.obj;
                    OnQueryFdnEnabled(ar);
                    break;
                case EVENT_CHANGE_FACILITY_FDN_DONE:
                    ar = (AsyncResult)msg.obj;
                    OnChangeFdnDone(ar);
                    break;
                case EVENT_QUERY_FACILITY_LOCK_DONE:
                    ar = (AsyncResult)msg.obj;
                    OnQueryFacilityLock(ar);
                    break;
                case EVENT_CHANGE_FACILITY_LOCK_DONE:
                    ar = (AsyncResult)msg.obj;
                    OnChangeFacilityLock(ar);
                    break;
                default:
                    Loge("Unknown Event " + msg.what);
            }
        }
    };

    void OnRefresh(IccRefreshResponse refreshResponse){
        If (refreshResponse == NULL) {
            Loge("onRefresh received without input");
            return;
        }

        If (refreshResponse.aid == NULL ||
                refreshResponse.aid->Equals(mAid)) {
            Log("refresh for app " + refreshResponse.aid);
        } else {
         // This is for a different app. Ignore.
            return;
        }

        Switch (refreshResponse.refreshResult) {
            case IccRefreshResponse.REFRESH_RESULT_INIT:
            case IccRefreshResponse.REFRESH_RESULT_RESET:
                Log("onRefresh: Setting app state to unknown");
                // Move our state to Unknown as soon as we know about a refresh
                // so that anyone interested does not get a stale state.
                mAppState = AppState.APPSTATE_UNKNOWN;
                break;
        }
    }

    CARAPI RegisterForReady(Handler h, Int32 what, Object obj) {
        Synchronized (mLock) {
            For (Int32 i = mReadyRegistrants->Size() - 1; i >= 0 ; i--) {
                Registrant  r = (Registrant) mReadyRegistrants->Get(i);
                Handler rH = r->GetHandler();

                If (rH != NULL && rH == h) {
                    return;
                }
            }
            Registrant r = new Registrant (h, what, obj);
            mReadyRegistrants->Add(r);
            NotifyReadyRegistrantsIfNeeded(r);
        }
    }

    CARAPI UnregisterForReady(Handler h) {
        Synchronized (mLock) {
            mReadyRegistrants->Remove(h);
        }
    }

    /**
     * Notifies handler of any transition into State->IsPinLocked()
     */
    CARAPI RegisterForLocked(Handler h, Int32 what, Object obj) {
        Synchronized (mLock) {
            Registrant r = new Registrant (h, what, obj);
            mPinLockedRegistrants->Add(r);
            NotifyPinLockedRegistrantsIfNeeded(r);
        }
    }

    CARAPI UnregisterForLocked(Handler h) {
        Synchronized (mLock) {
            mPinLockedRegistrants->Remove(h);
        }
    }

    /**
     * Notifies handler of any transition into State.PERSO_LOCKED
     */
    CARAPI RegisterForPersoLocked(Handler h, Int32 what, Object obj) {
        Synchronized (mLock) {
            Registrant r = new Registrant (h, what, obj);
            mPersoLockedRegistrants->Add(r);
            NotifyPersoLockedRegistrantsIfNeeded(r);
        }
    }

    CARAPI UnregisterForPersoLocked(Handler h) {
        Synchronized (mLock) {
            mPersoLockedRegistrants->Remove(h);
        }
    }

    /**
     * Notifies specified registrant, assume mLock is held.
     *
     * @param r Registrant to be notified. If NULL - all registrants will be notified
     */
    private void NotifyReadyRegistrantsIfNeeded(Registrant r) {
        If (mDestroyed) {
            return;
        }
        If (mAppState == AppState.APPSTATE_READY) {
            If (mPin1State == PinState.PINSTATE_ENABLED_NOT_VERIFIED ||
                    mPin1State == PinState.PINSTATE_ENABLED_BLOCKED ||
                    mPin1State == PinState.PINSTATE_ENABLED_PERM_BLOCKED) {
                Loge("Sanity check failed! APPSTATE is ready while PIN1 is not verified!!!");
                // Don't notify if application is in insane state
                return;
            }
            If (r == NULL) {
                If (DBG) Log("Notifying registrants: READY");
                mReadyRegistrants->NotifyRegistrants();
            } else {
                If (DBG) Log("Notifying 1 registrant: READY");
                r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
            }
        }
    }

    /**
     * Notifies specified registrant, assume mLock is held.
     *
     * @param r Registrant to be notified. If NULL - all registrants will be notified
     */
    private void NotifyPinLockedRegistrantsIfNeeded(Registrant r) {
        If (mDestroyed) {
            return;
        }

        If (mAppState == AppState.APPSTATE_PIN ||
                mAppState == AppState.APPSTATE_PUK) {
            If (mPin1State == PinState.PINSTATE_ENABLED_VERIFIED ||
                    mPin1State == PinState.PINSTATE_DISABLED) {
                Loge("Sanity check failed! APPSTATE is locked while PIN1 is not!!!");
                //Don't notify if application is in insane state
                return;
            }
            If (r == NULL) {
                If (DBG) Log("Notifying registrants: LOCKED");
                mPinLockedRegistrants->NotifyRegistrants();
            } else {
                If (DBG) Log("Notifying 1 registrant: LOCKED");
                r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
            }
        }
    }

    /**
     * Notifies specified registrant, assume mLock is held.
     *
     * @param r Registrant to be notified. If NULL - all registrants will be notified
     */
    private void NotifyPersoLockedRegistrantsIfNeeded(Registrant r) {
        If (mDestroyed) {
            return;
        }

        If (mAppState == AppState.APPSTATE_SUBSCRIPTION_PERSO &&
                IsPersoLocked()) {
            AsyncResult ar = new AsyncResult(NULL, mPersoSubState->Ordinal(), NULL);
            If (r == NULL) {
                If (DBG) Log("Notifying registrants: PERSO_LOCKED");
                mPersoLockedRegistrants->NotifyRegistrants(ar);
            } else {
                If (DBG) Log("Notifying 1 registrant: PERSO_LOCKED");
                r->NotifyRegistrant(ar);
            }
        }
    }

    public AppState GetState() {
        Synchronized (mLock) {
            return mAppState;
        }
    }

    public AppType GetType() {
        Synchronized (mLock) {
            return mAppType;
        }
    }

    public Int32 GetAuthContext() {
        Synchronized (mLock) {
            return mAuthContext;
        }
    }

    /**
     * Returns the authContext based on the type of UiccCard.
     *
     * @param appType the app type
     * @return authContext corresponding to the type or AUTH_CONTEXT_UNDEFINED if appType not
     * supported
     */
    private static Int32 GetAuthContext(AppType appType) {
        Int32 authContext;

        Switch (appType) {
            case APPTYPE_SIM:
                authContext = AUTH_CONTEXT_EAP_SIM;
                break;

            case APPTYPE_USIM:
                authContext = AUTH_CONTEXT_EAP_AKA;
                break;

            default:
                authContext = AUTH_CONTEXT_UNDEFINED;
                break;
        }

        return authContext;
    }

    public PersoSubState GetPersoSubState() {
        Synchronized (mLock) {
            return mPersoSubState;
        }
    }

    public String GetAid() {
        Synchronized (mLock) {
            return mAid;
        }
    }

    public String GetAppLabel() {
        return mAppLabel;
    }

    public PinState GetPin1State() {
        Synchronized (mLock) {
            If (mPin1Replaced) {
                return mUiccCard->GetUniversalPinState();
            }
            return mPin1State;
        }
    }

    public IccFileHandler GetIccFileHandler() {
        Synchronized (mLock) {
            return mIccFh;
        }
    }

    public IccRecords GetIccRecords() {
        Synchronized (mLock) {
            return mIccRecords;
        }
    }

    public Boolean IsPersoLocked() {
        Switch (mPersoSubState) {
            case PERSOSUBSTATE_UNKNOWN:
            case PERSOSUBSTATE_IN_PROGRESS:
            case PERSOSUBSTATE_READY:
                return FALSE;
            default:
                return TRUE;
        }
    }

    /**
     * Supply the ICC PIN to the ICC
     *
     * When the operation is complete, onComplete will be sent to its
     * Handler.
     *
     * onComplete.obj will be an AsyncResult
     * onComplete.arg1 = remaining attempts before puk locked or -1 if unknown
     *
     * ((AsyncResult)onComplete.obj).exception == NULL on success
     * ((AsyncResult)onComplete.obj).exception != NULL on fail
     *
     * If the supplied PIN is incorrect:
     * ((AsyncResult)onComplete.obj).exception != NULL
     * && ((AsyncResult)onComplete.obj).exception
     *       instanceof com.android.internal.telephony.gsm.CommandException)
     * && ((CommandException)(((AsyncResult)onComplete.obj).exception))
     *          .GetCommandError() == CommandException.Error.PASSWORD_INCORRECT
     */
    CARAPI SupplyPin (String pin, Message onComplete) {
        Synchronized (mLock) {
            mCi->SupplyIccPinForApp(pin, mAid, mHandler->ObtainMessage(EVENT_PIN1_PUK1_DONE,
                    onComplete));
        }
    }

    /**
     * Supply the ICC PUK to the ICC
     *
     * When the operation is complete, onComplete will be sent to its
     * Handler.
     *
     * onComplete.obj will be an AsyncResult
     * onComplete.arg1 = remaining attempts before Icc will be permanently unusable
     * or -1 if unknown
     *
     * ((AsyncResult)onComplete.obj).exception == NULL on success
     * ((AsyncResult)onComplete.obj).exception != NULL on fail
     *
     * If the supplied PIN is incorrect:
     * ((AsyncResult)onComplete.obj).exception != NULL
     * && ((AsyncResult)onComplete.obj).exception
     *       instanceof com.android.internal.telephony.gsm.CommandException)
     * && ((CommandException)(((AsyncResult)onComplete.obj).exception))
     *          .GetCommandError() == CommandException.Error.PASSWORD_INCORRECT
     *
     *
     */
    CARAPI SupplyPuk (String puk, String newPin, Message onComplete) {
        Synchronized (mLock) {
        mCi->SupplyIccPukForApp(puk, newPin, mAid,
                mHandler->ObtainMessage(EVENT_PIN1_PUK1_DONE, onComplete));
        }
    }

    CARAPI SupplyPin2 (String pin2, Message onComplete) {
        Synchronized (mLock) {
            mCi->SupplyIccPin2ForApp(pin2, mAid,
                    mHandler->ObtainMessage(EVENT_PIN2_PUK2_DONE, onComplete));
        }
    }

    CARAPI SupplyPuk2 (String puk2, String newPin2, Message onComplete) {
        Synchronized (mLock) {
            mCi->SupplyIccPuk2ForApp(puk2, newPin2, mAid,
                    mHandler->ObtainMessage(EVENT_PIN2_PUK2_DONE, onComplete));
        }
    }

    CARAPI SupplyDepersonalization (String pin, String type, Message onComplete) {
        Synchronized (mLock) {
            If (DBG) Log("Network Despersonalization: pin = **** , type = " + type);
            mCi->SupplyDepersonalization(pin, type, onComplete);
        }
    }

    /**
     * Check whether ICC pin lock is enabled
     * This is a sync call which returns the cached pin enabled state
     *
     * @return TRUE for ICC locked enabled
     *         FALSE for ICC locked disabled
     */
    public Boolean GetIccLockEnabled() {
        return mIccLockEnabled;
        /* STOPSHIP: Remove line above and all code associated with setting
           mIccLockEanbled once all RIL correctly sends the pin1 state.
        // Use getPin1State to take into account pin1Replaced flag
        PinState pinState = GetPin1State();
        return pinState == PinState.PINSTATE_ENABLED_NOT_VERIFIED ||
               pinState == PinState.PINSTATE_ENABLED_VERIFIED ||
               pinState == PinState.PINSTATE_ENABLED_BLOCKED ||
               pinState == PinState.PINSTATE_ENABLED_PERM_BLOCKED;*/
     }

    /**
     * Check whether ICC Fdn (fixed dialing number) is enabled
     * This is a sync call which returns the cached pin enabled state
     *
     * @return TRUE for ICC fdn enabled
     *         FALSE for ICC fdn disabled
     */
    public Boolean GetIccFdnEnabled() {
        Synchronized (mLock) {
            return mIccFdnEnabled;
        }
    }

    /**
     * Check whether Fdn (fixed dialing number) service is available.
     * @return TRUE if ICC fdn service available
     *         FALSE if ICC fdn service not available
     */
    public Boolean GetIccFdnAvailable() {
        return mIccFdnAvailable;
    }

    /**
     * Set the ICC pin lock enabled or disabled
     * When the operation is complete, onComplete will be sent to its handler
     *
     * @param enabled "TRUE" for locked "FALSE" for unlocked.
     * @param password needed to change the ICC pin state, aka. Pin1
     * @param onComplete
     *        onComplete.obj will be an AsyncResult
     *        ((AsyncResult)onComplete.obj).exception == NULL on success
     *        ((AsyncResult)onComplete.obj).exception != NULL on fail
     */
    CARAPI SetIccLockEnabled (Boolean enabled,
            String password, Message onComplete) {
        Synchronized (mLock) {
            Int32 serviceClassX;
            serviceClassX = CommandsInterface.SERVICE_CLASS_VOICE +
                    CommandsInterface.SERVICE_CLASS_DATA +
                    CommandsInterface.SERVICE_CLASS_FAX;

            mDesiredPinLocked = enabled;

            mCi->SetFacilityLockForApp(CommandsInterface.CB_FACILITY_BA_SIM,
                    enabled, password, serviceClassX, mAid,
                    mHandler->ObtainMessage(EVENT_CHANGE_FACILITY_LOCK_DONE, onComplete));
        }
    }

    /**
     * Set the ICC fdn enabled or disabled
     * When the operation is complete, onComplete will be sent to its handler
     *
     * @param enabled "TRUE" for locked "FALSE" for unlocked.
     * @param password needed to change the ICC fdn enable, aka Pin2
     * @param onComplete
     *        onComplete.obj will be an AsyncResult
     *        ((AsyncResult)onComplete.obj).exception == NULL on success
     *        ((AsyncResult)onComplete.obj).exception != NULL on fail
     */
    CARAPI SetIccFdnEnabled (Boolean enabled,
            String password, Message onComplete) {
        Synchronized (mLock) {
            Int32 serviceClassX;
            serviceClassX = CommandsInterface.SERVICE_CLASS_VOICE +
                    CommandsInterface.SERVICE_CLASS_DATA +
                    CommandsInterface.SERVICE_CLASS_FAX +
                    CommandsInterface.SERVICE_CLASS_SMS;

            mDesiredFdnEnabled = enabled;

            mCi->SetFacilityLockForApp(CommandsInterface.CB_FACILITY_BA_FD,
                    enabled, password, serviceClassX, mAid,
                    mHandler->ObtainMessage(EVENT_CHANGE_FACILITY_FDN_DONE, onComplete));
        }
    }

    /**
     * Change the ICC password used in ICC pin lock
     * When the operation is complete, onComplete will be sent to its handler
     *
     * @param oldPassword is the old password
     * @param newPassword is the new password
     * @param onComplete
     *        onComplete.obj will be an AsyncResult
     *        onComplete.arg1 = attempts remaining or -1 if unknown
     *        ((AsyncResult)onComplete.obj).exception == NULL on success
     *        ((AsyncResult)onComplete.obj).exception != NULL on fail
     */
    CARAPI ChangeIccLockPassword(String oldPassword, String newPassword,
            Message onComplete) {
        Synchronized (mLock) {
            If (DBG) Log("changeIccLockPassword");
            mCi->ChangeIccPinForApp(oldPassword, newPassword, mAid,
                    mHandler->ObtainMessage(EVENT_CHANGE_PIN1_DONE, onComplete));
        }
    }

    /**
     * Change the ICC password used in ICC fdn enable
     * When the operation is complete, onComplete will be sent to its handler
     *
     * @param oldPassword is the old password
     * @param newPassword is the new password
     * @param onComplete
     *        onComplete.obj will be an AsyncResult
     *        ((AsyncResult)onComplete.obj).exception == NULL on success
     *        ((AsyncResult)onComplete.obj).exception != NULL on fail
     */
    CARAPI ChangeIccFdnPassword(String oldPassword, String newPassword,
            Message onComplete) {
        Synchronized (mLock) {
            If (DBG) Log("changeIccFdnPassword");
            mCi->ChangeIccPin2ForApp(oldPassword, newPassword, mAid,
                    mHandler->ObtainMessage(EVENT_CHANGE_PIN2_DONE, onComplete));
        }
    }

    /**
     * @return TRUE if ICC card is PIN2 blocked
     */
    public Boolean GetIccPin2Blocked() {
        Synchronized (mLock) {
            return mPin2State == PinState.PINSTATE_ENABLED_BLOCKED;
        }
    }

    /**
     * @return TRUE if ICC card is PUK2 blocked
     */
    public Boolean GetIccPuk2Blocked() {
        Synchronized (mLock) {
            return mPin2State == PinState.PINSTATE_ENABLED_PERM_BLOCKED;
        }
    }

    protected UiccCard GetUiccCard() {
        return mUiccCard;
    }

    public UICCConfig GetUICCConfig() {
        return mUiccCard->GetUICCConfig();
    }

    private void Log(String msg) {
        Rlog->D(LOG_TAG, msg);
    }

    private void Loge(String msg) {
        Rlog->E(LOG_TAG, msg);
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("UiccCardApplication: " + this);
        pw->Println(" mUiccCard=" + mUiccCard);
        pw->Println(" mAppState=" + mAppState);
        pw->Println(" mAppType=" + mAppType);
        pw->Println(" mPersoSubState=" + mPersoSubState);
        pw->Println(" mAid=" + mAid);
        pw->Println(" mAppLabel=" + mAppLabel);
        pw->Println(" mPin1Replaced=" + mPin1Replaced);
        pw->Println(" mPin1State=" + mPin1State);
        pw->Println(" mPin2State=" + mPin2State);
        pw->Println(" mIccFdnEnabled=" + mIccFdnEnabled);
        pw->Println(" mDesiredFdnEnabled=" + mDesiredFdnEnabled);
        pw->Println(" mIccLockEnabled=" + mIccLockEnabled);
        pw->Println(" mDesiredPinLocked=" + mDesiredPinLocked);
        pw->Println(" mCi=" + mCi);
        pw->Println(" mIccRecords=" + mIccRecords);
        pw->Println(" mIccFh=" + mIccFh);
        pw->Println(" mDestroyed=" + mDestroyed);
        pw->Println(" mReadyRegistrants: size=" + mReadyRegistrants->Size());
        For (Int32 i = 0; i < mReadyRegistrants->Size(); i++) {
            pw->Println("  mReadyRegistrants[" + i + "]="
                    + ((Registrant)mReadyRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mPinLockedRegistrants: size=" + mPinLockedRegistrants->Size());
        For (Int32 i = 0; i < mPinLockedRegistrants->Size(); i++) {
            pw->Println("  mPinLockedRegistrants[" + i + "]="
                    + ((Registrant)mPinLockedRegistrants->Get(i)).GetHandler());
        }
        pw->Println(" mPersoLockedRegistrants: size=" + mPersoLockedRegistrants->Size());
        For (Int32 i = 0; i < mPersoLockedRegistrants->Size(); i++) {
            pw->Println("  mPersoLockedRegistrants[" + i + "]="
                    + ((Registrant)mPersoLockedRegistrants->Get(i)).GetHandler());
        }
        pw->Flush();
    }
}
