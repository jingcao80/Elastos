/*
 * Copyright (C) 2010 The Android Open Source Project
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

using Elastos::Droid::Internal::Telephony::Imsphone::IImsPhone;
using Elastos::Droid::Internal::Telephony::Sip::ISipPhone;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Telecom::IVideoProfile;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;



/**
 * @hide
 *
 * CallManager class provides an abstract layer for PhoneApp to access
 * and control calls. It implements Phone interface.
 *
 * CallManager provides call and connection control as well as
 * channel capability.
 *
 * There are three categories of APIs CallManager provided
 *
 *  1. Call control and operation, such as Dial() and Hangup()
 *  2. Channel capabilities, such as CanConference()
 *  3. Register notification
 *
 *
 */
public class CallManager {

    private static const String LOG_TAG ="CallManager";
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE;

    private static const Int32 EVENT_DISCONNECT = 100;
    private static const Int32 EVENT_PRECISE_CALL_STATE_CHANGED = 101;
    private static const Int32 EVENT_NEW_RINGING_CONNECTION = 102;
    private static const Int32 EVENT_UNKNOWN_CONNECTION = 103;
    private static const Int32 EVENT_INCOMING_RING = 104;
    private static const Int32 EVENT_RINGBACK_TONE = 105;
    private static const Int32 EVENT_IN_CALL_VOICE_PRIVACY_ON = 106;
    private static const Int32 EVENT_IN_CALL_VOICE_PRIVACY_OFF = 107;
    private static const Int32 EVENT_CALL_WAITING = 108;
    private static const Int32 EVENT_DISPLAY_INFO = 109;
    private static const Int32 EVENT_SIGNAL_INFO = 110;
    private static const Int32 EVENT_CDMA_OTA_STATUS_CHANGE = 111;
    private static const Int32 EVENT_RESEND_INCALL_MUTE = 112;
    private static const Int32 EVENT_MMI_INITIATE = 113;
    private static const Int32 EVENT_MMI_COMPLETE = 114;
    private static const Int32 EVENT_ECM_TIMER_RESET = 115;
    private static const Int32 EVENT_SUBSCRIPTION_INFO_READY = 116;
    private static const Int32 EVENT_SUPP_SERVICE_FAILED = 117;
    private static const Int32 EVENT_SERVICE_STATE_CHANGED = 118;
    private static const Int32 EVENT_POST_DIAL_CHARACTER = 119;
    private static const Int32 EVENT_ONHOLD_TONE = 120;
    private static const Int32 EVENT_SUPP_SERVICE_NOTIFY = 121;

    // Singleton instance
    private static const CallManager INSTANCE = new CallManager();

    // list of registered phones, which are PhoneBase objs
    private final ArrayList<Phone> mPhones;

    // list of supported ringing calls
    private final ArrayList<Call> mRingingCalls;

    // list of supported background calls
    private final ArrayList<Call> mBackgroundCalls;

    // list of supported foreground calls
    private final ArrayList<Call> mForegroundCalls;

    // empty connection list
    private final ArrayList<Connection> mEmptyConnections = new ArrayList<Connection>();

    // mapping of phones to registered handler instances used for callbacks from RIL
    private final HashMap<Phone, CallManagerHandler> mHandlerMap = new HashMap<>();

    // default phone as the first phone registered, which is PhoneBase obj
    private Phone mDefaultPhone;

    private Boolean mSpeedUpAudioForMtCall = FALSE;

    // Holds the current active SUB, all actions would be
    // taken on this sub.
    private static Int64 mActiveSub = SubscriptionManager.INVALID_SUB_ID;

    // state registrants
    protected final RegistrantList mPreciseCallStateRegistrants
    = new RegistrantList();

    protected final RegistrantList mNewRingingConnectionRegistrants
    = new RegistrantList();

    protected final RegistrantList mIncomingRingRegistrants
    = new RegistrantList();

    protected final RegistrantList mDisconnectRegistrants
    = new RegistrantList();

    protected final RegistrantList mMmiRegistrants
    = new RegistrantList();

    protected final RegistrantList mUnknownConnectionRegistrants
    = new RegistrantList();

    protected final RegistrantList mRingbackToneRegistrants
    = new RegistrantList();

    protected final RegistrantList mOnHoldToneRegistrants
    = new RegistrantList();

    protected final RegistrantList mInCallVoicePrivacyOnRegistrants
    = new RegistrantList();

    protected final RegistrantList mInCallVoicePrivacyOffRegistrants
    = new RegistrantList();

    protected final RegistrantList mCallWaitingRegistrants
    = new RegistrantList();

    protected final RegistrantList mDisplayInfoRegistrants
    = new RegistrantList();

    protected final RegistrantList mSignalInfoRegistrants
    = new RegistrantList();

    protected final RegistrantList mCdmaOtaStatusChangeRegistrants
    = new RegistrantList();

    protected final RegistrantList mResendIncallMuteRegistrants
    = new RegistrantList();

    protected final RegistrantList mMmiInitiateRegistrants
    = new RegistrantList();

    protected final RegistrantList mMmiCompleteRegistrants
    = new RegistrantList();

    protected final RegistrantList mEcmTimerResetRegistrants
    = new RegistrantList();

    protected final RegistrantList mSubscriptionInfoReadyRegistrants
    = new RegistrantList();

    protected final RegistrantList mSuppServiceNotifyRegistrants
    = new RegistrantList();

    protected final RegistrantList mSuppServiceFailedRegistrants
    = new RegistrantList();

    protected final RegistrantList mServiceStateChangedRegistrants
    = new RegistrantList();

    protected final RegistrantList mPostDialCharacterRegistrants
    = new RegistrantList();

    protected final RegistrantList mActiveSubChangeRegistrants
    = new RegistrantList();

    private CallManager() {
        mPhones = new ArrayList<Phone>();
        mRingingCalls = new ArrayList<Call>();
        mBackgroundCalls = new ArrayList<Call>();
        mForegroundCalls = new ArrayList<Call>();
        mDefaultPhone = NULL;
    }

    /**
     * get singleton instance of CallManager
     * @return CallManager
     */
    public static CallManager GetInstance() {
        return INSTANCE;
    }

    /**
     * Get the corresponding PhoneBase obj
     *
     * @param phone a Phone object
     * @return the corresponding PhoneBase obj in Phone if Phone
     * is a PhoneProxy obj
     * or the Phone itself if Phone is not a PhoneProxy obj
     */
    private static Phone GetPhoneBase(Phone phone) {
        If (phone instanceof PhoneProxy) {
            return phone->GetForegroundCall()->GetPhone();
        }
        return phone;
    }

    /**
     * Check if two phones refer to the same PhoneBase obj
     *
     * Note: PhoneBase, not PhoneProxy, is to be used inside of CallManager
     *
     * Both PhoneBase and PhoneProxy implement Phone interface, so
     * they have same phone APIs, such as Dial(). The real implementation, for
     * example in GSM,  is in GSMPhone as extend from PhoneBase, so that
     * foregroundCall->GetPhone() returns GSMPhone obj. On the other hand,
     * PhoneFactory->GetDefaultPhone() returns PhoneProxy obj, which has a class
     * member of GSMPhone.
     *
     * So for phone returned by PhoneFacotry, which is used by PhoneApp,
     *        phone->GetForegroundCall()->GetPhone() != phone
     * but
     *        IsSamePhone(phone, phone->GetForegroundCall()->GetPhone()) == TRUE
     *
     * @param p1 is the first Phone obj
     * @param p2 is the second Phone obj
     * @return TRUE if p1 and p2 refer to the same phone
     */
    public static Boolean IsSamePhone(Phone p1, Phone p2) {
        Return (GetPhoneBase(p1) == GetPhoneBase(p2));
    }

    /**
     * Returns all the registered phone objects.
     * @return all the registered phone objects.
     */
    public List<Phone> GetAllPhones() {
        return Collections->UnmodifiableList(mPhones);
    }

    /**
     * get Phone object corresponds to subId
     * @return Phone
     */
    private Phone GetPhone(Int64 subId) {
        Phone p = NULL;
        For (Phone phone : mPhones) {
            If (phone->GetSubId() == subId && !(phone instanceof ImsPhone)) {
                p = phone;
                break;
            }
        }
        return p;
    }

    /**
     * Get current coarse-grained voice call state.
     * If the Call Manager has an active call and call waiting occurs,
     * then the phone state is RINGING not OFFHOOK
     *
     */
    public PhoneConstants.State GetState() {
        PhoneConstants.State s = PhoneConstants.State.IDLE;

        For (Phone phone : mPhones) {
            If (phone->GetState() == PhoneConstants.State.RINGING) {
                s = PhoneConstants.State.RINGING;
            } else If (phone->GetState() == PhoneConstants.State.OFFHOOK) {
                If (s == PhoneConstants.State.IDLE) s = PhoneConstants.State.OFFHOOK;
            }
        }
        return s;
    }

    /**
     * Get current coarse-grained voice call state on a subId.
     * If the Call Manager has an active call and call waiting occurs,
     * then the phone state is RINGING not OFFHOOK
     *
     */
    public PhoneConstants.State GetState(Int64 subId) {
        PhoneConstants.State s = PhoneConstants.State.IDLE;

        For (Phone phone : mPhones) {
            If (phone->GetSubId() == subId) {
                If (phone->GetState() == PhoneConstants.State.RINGING) {
                    s = PhoneConstants.State.RINGING;
                } else If (phone->GetState() == PhoneConstants.State.OFFHOOK) {
                    If (s == PhoneConstants.State.IDLE) s = PhoneConstants.State.OFFHOOK;
                }
            }
        }
        return s;
    }

    /**
     * @return the service state of CallManager, which represents the
     * highest priority state of all the service states of phones
     *
     * The priority is defined as
     *
     * STATE_IN_SERIVCE > STATE_OUT_OF_SERIVCE > STATE_EMERGENCY > STATE_POWER_OFF
     *
     */

    public Int32 GetServiceState() {
        Int32 resultState = ServiceState.STATE_OUT_OF_SERVICE;

        For (Phone phone : mPhones) {
            Int32 serviceState = phone->GetServiceState()->GetState();
            If (serviceState == ServiceState.STATE_IN_SERVICE) {
                // IN_SERVICE has the highest priority
                resultState = serviceState;
                break;
            } else If (serviceState == ServiceState.STATE_OUT_OF_SERVICE) {
                // OUT_OF_SERVICE replaces EMERGENCY_ONLY and POWER_OFF
                // Note: EMERGENCY_ONLY is not in use at this moment
                If ( resultState == ServiceState.STATE_EMERGENCY_ONLY ||
                        resultState == ServiceState.STATE_POWER_OFF) {
                    resultState = serviceState;
                }
            } else If (serviceState == ServiceState.STATE_EMERGENCY_ONLY) {
                If (resultState == ServiceState.STATE_POWER_OFF) {
                    resultState = serviceState;
                }
            }
        }
        return resultState;
    }

    /**
     * @return the Phone service state corresponds to subId
     */
    public Int32 GetServiceState(Int64 subId) {
        Int32 resultState = ServiceState.STATE_OUT_OF_SERVICE;

        For (Phone phone : mPhones) {
            If (phone->GetSubId() == subId) {
                Int32 serviceState = phone->GetServiceState()->GetState();
                If (serviceState == ServiceState.STATE_IN_SERVICE) {
                    // IN_SERVICE has the highest priority
                    resultState = serviceState;
                    break;
                } else If (serviceState == ServiceState.STATE_OUT_OF_SERVICE) {
                    // OUT_OF_SERVICE replaces EMERGENCY_ONLY and POWER_OFF
                    // Note: EMERGENCY_ONLY is not in use at this moment
                    If ( resultState == ServiceState.STATE_EMERGENCY_ONLY ||
                            resultState == ServiceState.STATE_POWER_OFF) {
                        resultState = serviceState;
                    }
                } else If (serviceState == ServiceState.STATE_EMERGENCY_ONLY) {
                    If (resultState == ServiceState.STATE_POWER_OFF) {
                        resultState = serviceState;
                    }
                }
            }
        }
        return resultState;
    }

    /**
     * @return the phone associated with any call
     */
    public Phone GetPhoneInCall() {
        Phone phone = NULL;
        If (!GetFirstActiveRingingCall()->IsIdle()) {
            phone = GetFirstActiveRingingCall()->GetPhone();
        } else If (!GetActiveFgCall()->IsIdle()) {
            phone = GetActiveFgCall()->GetPhone();
        } else {
            // If BG call is idle, we return default phone
            phone = GetFirstActiveBgCall()->GetPhone();
        }
        return phone;
    }

    public Phone GetPhoneInCall(Int64 subId) {
        Phone phone = NULL;
        If (!GetFirstActiveRingingCall(subId).IsIdle()) {
            phone = GetFirstActiveRingingCall(subId).GetPhone();
        } else If (!GetActiveFgCall(subId).IsIdle()) {
            phone = GetActiveFgCall(subId).GetPhone();
        } else {
            // If BG call is idle, we return default phone
            phone = GetFirstActiveBgCall(subId).GetPhone();
        }
        return phone;
    }

    /**
     * Register phone to CallManager
     * @param phone to be registered
     * @return TRUE if register successfully
     */
    public Boolean RegisterPhone(Phone phone) {
        Phone basePhone = GetPhoneBase(phone);

        If (basePhone != NULL && !mPhones->Contains(basePhone)) {

            If (DBG) {
                Rlog->D(LOG_TAG, "RegisterPhone(" +
                        phone->GetPhoneName() + " " + phone + ")");
            }

            If (mPhones->IsEmpty()) {
                mDefaultPhone = basePhone;
            }
            mPhones->Add(basePhone);
            mRingingCalls->Add(basePhone->GetRingingCall());
            mBackgroundCalls->Add(basePhone->GetBackgroundCall());
            mForegroundCalls->Add(basePhone->GetForegroundCall());
            RegisterForPhoneStates(basePhone);
            return TRUE;
        }
        return FALSE;
    }

    /**
     * unregister phone from CallManager
     * @param phone to be unregistered
     */
    CARAPI UnregisterPhone(Phone phone) {
        Phone basePhone = GetPhoneBase(phone);

        If (basePhone != NULL && mPhones->Contains(basePhone)) {

            If (DBG) {
                Rlog->D(LOG_TAG, "UnregisterPhone(" +
                        phone->GetPhoneName() + " " + phone + ")");
            }

            Phone vPhone = basePhone->GetImsPhone();
            If (vPhone != NULL) {
               UnregisterPhone(vPhone);
            }

            mPhones->Remove(basePhone);
            mRingingCalls->Remove(basePhone->GetRingingCall());
            mBackgroundCalls->Remove(basePhone->GetBackgroundCall());
            mForegroundCalls->Remove(basePhone->GetForegroundCall());
            UnregisterForPhoneStates(basePhone);
            If (basePhone == mDefaultPhone) {
                If (mPhones->IsEmpty()) {
                    mDefaultPhone = NULL;
                } else {
                    mDefaultPhone = mPhones->Get(0);
                }
            }
        }
    }

    /**
     * return the default phone or NULL if no phone available
     */
    public Phone GetDefaultPhone() {
        return mDefaultPhone;
    }

    /**
     * @return the phone associated with the foreground call
     */
    public Phone GetFgPhone() {
        return GetActiveFgCall()->GetPhone();
    }

    /**
     * @return the phone associated with the foreground call
     * of a particular subId
     */
    public Phone GetFgPhone(Int64 subId) {
        return GetActiveFgCall(subId).GetPhone();
    }

    /**
     * @return the phone associated with the background call
     */
    public Phone GetBgPhone() {
        return GetFirstActiveBgCall()->GetPhone();
    }

    /**
     * @return the phone associated with the background call
     * of a particular subId
     */
    public Phone GetBgPhone(Int64 subId) {
        return GetFirstActiveBgCall(subId).GetPhone();
    }

    /**
     * @return the phone associated with the ringing call
     */
    public Phone GetRingingPhone() {
        return GetFirstActiveRingingCall()->GetPhone();
    }

    /**
     * @return the phone associated with the ringing call
     * of a particular subId
     */
    public Phone GetRingingPhone(Int64 subId) {
        return GetFirstActiveRingingCall(subId).GetPhone();
    }

    private Context GetContext() {
        Phone defaultPhone = GetDefaultPhone();
        Return ((defaultPhone == NULL) ? NULL : defaultPhone->GetContext());
    }

    private void RegisterForPhoneStates(Phone phone) {
        // We need to keep a mapping of handler to Phone for proper unregistration.
        // TODO: Clean up this solution as it is just a work around for each Phone instance
        // using the same Handler to register with the RIL. When time permits, we should consider
        // moving the Handler (or the reference ot the handler) into the Phone object.
        // See b/17414427.
        CallManagerHandler handler = mHandlerMap->Get(phone);
        If (handler != NULL) {
            Rlog->D(LOG_TAG, "This phone has already been registered.");
            return;
        }

        // New registration, create a new handler instance and register the phone.
        handler = new CallManagerHandler();
        mHandlerMap->Put(phone, handler);

        // for common events supported by all phones
        phone->RegisterForPreciseCallStateChanged(handler, EVENT_PRECISE_CALL_STATE_CHANGED, NULL);
        phone->RegisterForDisconnect(handler, EVENT_DISCONNECT, NULL);
        phone->RegisterForNewRingingConnection(handler, EVENT_NEW_RINGING_CONNECTION, NULL);
        phone->RegisterForUnknownConnection(handler, EVENT_UNKNOWN_CONNECTION, NULL);
        phone->RegisterForIncomingRing(handler, EVENT_INCOMING_RING, NULL);
        phone->RegisterForRingbackTone(handler, EVENT_RINGBACK_TONE, NULL);
        phone->RegisterForInCallVoicePrivacyOn(handler, EVENT_IN_CALL_VOICE_PRIVACY_ON, NULL);
        phone->RegisterForInCallVoicePrivacyOff(handler, EVENT_IN_CALL_VOICE_PRIVACY_OFF, NULL);
        phone->RegisterForDisplayInfo(handler, EVENT_DISPLAY_INFO, NULL);
        phone->RegisterForSignalInfo(handler, EVENT_SIGNAL_INFO, NULL);
        phone->RegisterForResendIncallMute(handler, EVENT_RESEND_INCALL_MUTE, NULL);
        phone->RegisterForMmiInitiate(handler, EVENT_MMI_INITIATE, NULL);
        phone->RegisterForMmiComplete(handler, EVENT_MMI_COMPLETE, NULL);
        phone->RegisterForSuppServiceFailed(handler, EVENT_SUPP_SERVICE_FAILED, NULL);
        phone->RegisterForServiceStateChanged(handler, EVENT_SERVICE_STATE_CHANGED, NULL);

        // for events supported only by GSM phone
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
            phone->RegisterForSuppServiceNotification(handler, EVENT_SUPP_SERVICE_NOTIFY, NULL);
        }

        // for events supported only by GSM, CDMA and IMS phone
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM ||
                phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA ||
                phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_IMS) {
            phone->SetOnPostDialCharacter(handler, EVENT_POST_DIAL_CHARACTER, NULL);
        }

        // for events supported only by CDMA phone
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA ){
            phone->RegisterForCdmaOtaStatusChange(handler, EVENT_CDMA_OTA_STATUS_CHANGE, NULL);
            phone->RegisterForSubscriptionInfoReady(handler, EVENT_SUBSCRIPTION_INFO_READY, NULL);
            phone->RegisterForCallWaiting(handler, EVENT_CALL_WAITING, NULL);
            phone->RegisterForEcmTimerReset(handler, EVENT_ECM_TIMER_RESET, NULL);
        }

        // for events supported only by IMS phone
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_IMS) {
            phone->RegisterForOnHoldTone(handler, EVENT_ONHOLD_TONE, NULL);
        }
    }

    private void UnregisterForPhoneStates(Phone phone) {
        // Make sure that we clean up our map of handlers to Phones.
        CallManagerHandler handler = mHandlerMap->Get(phone);
        If (handler != NULL) {
            Rlog->E(LOG_TAG, "Could not find Phone handler for unregistration");
            return;
        }
        mHandlerMap->Remove(phone);

        //  for common events supported by all phones
        phone->UnregisterForPreciseCallStateChanged(handler);
        phone->UnregisterForDisconnect(handler);
        phone->UnregisterForNewRingingConnection(handler);
        phone->UnregisterForUnknownConnection(handler);
        phone->UnregisterForIncomingRing(handler);
        phone->UnregisterForRingbackTone(handler);
        phone->UnregisterForInCallVoicePrivacyOn(handler);
        phone->UnregisterForInCallVoicePrivacyOff(handler);
        phone->UnregisterForDisplayInfo(handler);
        phone->UnregisterForSignalInfo(handler);
        phone->UnregisterForResendIncallMute(handler);
        phone->UnregisterForMmiInitiate(handler);
        phone->UnregisterForMmiComplete(handler);
        phone->UnregisterForSuppServiceFailed(handler);
        phone->UnregisterForServiceStateChanged(handler);

        // for events supported only by GSM phone
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM) {
            phone->UnregisterForSuppServiceNotification(handler);
        }

        // for events supported only by GSM, CDMA and IMS phone
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_GSM ||
                phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA ||
                phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_IMS) {
            phone->SetOnPostDialCharacter(NULL, EVENT_POST_DIAL_CHARACTER, NULL);
        }

        // for events supported only by CDMA phone
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA ){
            phone->UnregisterForCdmaOtaStatusChange(handler);
            phone->UnregisterForSubscriptionInfoReady(handler);
            phone->UnregisterForCallWaiting(handler);
            phone->UnregisterForEcmTimerReset(handler);
        }

        // for events supported only by IMS phone
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_IMS) {
            phone->UnregisterForOnHoldTone(handler);
        }
    }

    /**
     * Answers a ringing or waiting call.
     *
     * Active call, if any, go on hold.
     * If active call can't be held, i.e., a background call of the same channel exists,
     * the active call will be hang up.
     *
     * Answering occurs asynchronously, and final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException when call is not ringing or waiting
     */
    CARAPI AcceptCall(Call ringingCall) throws CallStateException {
        Phone ringingPhone = ringingCall->GetPhone();

        If (VDBG) {
            Rlog->D(LOG_TAG, "AcceptCall(" +ringingCall + " from " + ringingCall->GetPhone() + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        If ( HasActiveFgCall() ) {
            Phone activePhone = GetActiveFgCall()->GetPhone();
            Boolean hasBgCall = ! (activePhone->GetBackgroundCall()->IsIdle());
            Boolean sameChannel = (activePhone == ringingPhone);

            If (VDBG) {
                Rlog->D(LOG_TAG, "hasBgCall: "+ hasBgCall + "sameChannel:" + sameChannel);
            }

            If (sameChannel && hasBgCall) {
                GetActiveFgCall()->Hangup();
            } else If (!sameChannel && !hasBgCall) {
                activePhone->SwitchHoldingAndActive();
            } else If (!sameChannel && hasBgCall) {
                GetActiveFgCall()->Hangup();
            }
        }

        // We only support the AUDIO_ONLY video state in this scenario.
        ringingPhone->AcceptCall(VideoProfile.VideoState.AUDIO_ONLY);

        If (VDBG) {
            Rlog->D(LOG_TAG, "End AcceptCall(" +ringingCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }
    }

    /**
     * Reject (ignore) a ringing call. In GSM, this means UDUB
     * (User Determined User Busy). Reject occurs asynchronously,
     * and final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException when no call is ringing or waiting
     */
    CARAPI RejectCall(Call ringingCall) throws CallStateException {
        If (VDBG) {
            Rlog->D(LOG_TAG, "RejectCall(" +ringingCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        Phone ringingPhone = ringingCall->GetPhone();

        ringingPhone->RejectCall();

        If (VDBG) {
            Rlog->D(LOG_TAG, "End RejectCall(" +ringingCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }
    }

    /**
     * Places active call on hold, and makes held call active.
     * Switch occurs asynchronously and may fail.
     *
     * There are 4 scenarios
     * 1. only active call but no held call, aka, hold
     * 2. no active call but only held call, aka, unhold
     * 3. both active and held calls from same phone, aka, swap
     * 4. active and held calls from different phones, aka, phone swap
     *
     * Final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if active call is ringing, waiting, or
     * dialing/alerting, or heldCall can't be active.
     * In these cases, this operation may not be performed.
     */
    CARAPI SwitchHoldingAndActive(Call heldCall) throws CallStateException {
        Phone activePhone = NULL;
        Phone heldPhone = NULL;

        If (VDBG) {
            Rlog->D(LOG_TAG, "SwitchHoldingAndActive(" +heldCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        If (HasActiveFgCall()) {
            activePhone = GetActiveFgCall()->GetPhone();
        }

        If (heldCall != NULL) {
            heldPhone = heldCall->GetPhone();
        }

        If (activePhone != NULL) {
            activePhone->SwitchHoldingAndActive();
        }

        If (heldPhone != NULL && heldPhone != activePhone) {
            heldPhone->SwitchHoldingAndActive();
        }

        If (VDBG) {
            Rlog->D(LOG_TAG, "End SwitchHoldingAndActive(" +heldCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }
    }

    /**
     * Hangup foreground call and resume the specific background call
     *
     * Note: this is noop if there is no foreground call or the heldCall is NULL
     *
     * @param heldCall to become foreground
     * @throws CallStateException
     */
    CARAPI HangupForegroundResumeBackground(Call heldCall) throws CallStateException {
        Phone foregroundPhone = NULL;
        Phone backgroundPhone = NULL;

        If (VDBG) {
            Rlog->D(LOG_TAG, "HangupForegroundResumeBackground(" +heldCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        If (HasActiveFgCall()) {
            foregroundPhone = GetFgPhone();
            If (heldCall != NULL) {
                backgroundPhone = heldCall->GetPhone();
                If (foregroundPhone == backgroundPhone) {
                    GetActiveFgCall()->Hangup();
                } else {
                // the call to be hangup and resumed belongs to different phones
                    GetActiveFgCall()->Hangup();
                    SwitchHoldingAndActive(heldCall);
                }
            }
        }

        If (VDBG) {
            Rlog->D(LOG_TAG, "End HangupForegroundResumeBackground(" +heldCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }
    }

    /**
     * Whether or not the phone can conference in the current phone
     * state--that is, one call holding and one call active.
     * @return TRUE if the phone can conference; FALSE otherwise.
     */
    public Boolean CanConference(Call heldCall) {
        Phone activePhone = NULL;
        Phone heldPhone = NULL;

        If (HasActiveFgCall()) {
            activePhone = GetActiveFgCall()->GetPhone();
        }

        If (heldCall != NULL) {
            heldPhone = heldCall->GetPhone();
        }

        return heldPhone->GetClass()->Equals(activePhone->GetClass());
    }

    /**
     * Whether or not the phone can conference in the current phone
     * state--that is, one call holding and one call active.
     * This method consider the phone object which is specific
     * to the provided subId.
     * @return TRUE if the phone can conference; FALSE otherwise.
     */
    public Boolean CanConference(Call heldCall, Int64 subId) {
        Phone activePhone = NULL;
        Phone heldPhone = NULL;

        If (HasActiveFgCall(subId)) {
            activePhone = GetActiveFgCall(subId).GetPhone();
        }

        If (heldCall != NULL) {
            heldPhone = heldCall->GetPhone();
        }

        return heldPhone->GetClass()->Equals(activePhone->GetClass());
    }

    /**
     * Conferences holding and active. Conference occurs asynchronously
     * and may fail. Final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if CanConference() would return FALSE.
     * In these cases, this operation may not be performed.
     */
    CARAPI Conference(Call heldCall) throws CallStateException {
        Int64 subId  = heldCall->GetPhone()->GetSubId();

        If (VDBG) {
            Rlog->D(LOG_TAG, "Conference(" +heldCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        Phone fgPhone = GetFgPhone(subId);
        If (fgPhone != NULL) {
            If (fgPhone instanceof SipPhone) {
                ((SipPhone) fgPhone).Conference(heldCall);
            } else If (CanConference(heldCall)) {
                fgPhone->Conference();
            } else {
                Throw(new CallStateException("Can't conference foreground and selected background call"));
            }
        } else {
            Rlog->D(LOG_TAG, "conference: fgPhone=NULL");
        }

        If (VDBG) {
            Rlog->D(LOG_TAG, "End Conference(" +heldCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }

    }

    /**
     * Initiate a new voice connection. This happens asynchronously, so you
     * cannot assume the audio path is Connected (or a call index has been
     * assigned) until PhoneStateChanged notification has occurred.
     *
     * @exception CallStateException if a new outgoing call is not currently
     * possible because no more call slots exist or a call exists that is
     * dialing, alerting, ringing, or waiting.  Other errors are
     * handled asynchronously.
     */
    public Connection Dial(Phone phone, String dialString, Int32 videoState)
            throws CallStateException {
        Phone basePhone = GetPhoneBase(phone);
        Int64 subId = phone->GetSubId();
        Connection result;

        If (VDBG) {
            Rlog->D(LOG_TAG, " Dial(" + basePhone + ", "+ dialString + ")" +
                    " subId = " + subId);
            Rlog->D(LOG_TAG, ToString());
        }

        If (!CanDial(phone)) {
            /*
             * canDial function only checks whether the phone can make a new call.
             * InCall MMI commmands are basically supplementary services
             * within a call eg: call hold, call deflection, explicit call transfer etc.
             */
            String newDialString = PhoneNumberUtils->StripSeparators(dialString);
            If (basePhone->HandleInCallMmiCommands(newDialString)) {
                return NULL;
            } else {
                throw new CallStateException("cannot dial in current state");
            }
        }

        If ( HasActiveFgCall(subId) ) {
            Phone activePhone = GetActiveFgCall(subId).GetPhone();
            Boolean hasBgCall = !(activePhone->GetBackgroundCall()->IsIdle());

            If (DBG) {
                Rlog->D(LOG_TAG, "hasBgCall: "+ hasBgCall + " sameChannel:" + (activePhone == basePhone));
            }

            // Manipulation between IMS phone and its owner
            // will be treated in GSM/CDMA phone.
            Phone vPhone = basePhone->GetImsPhone();
            If (activePhone != basePhone
                    && (vPhone == NULL || vPhone != activePhone)) {
                If (hasBgCall) {
                    Rlog->D(LOG_TAG, "Hangup");
                    GetActiveFgCall(subId).Hangup();
                } else {
                    Rlog->D(LOG_TAG, "Switch");
                    activePhone->SwitchHoldingAndActive();
                }
            }
        }

        result = basePhone->Dial(dialString, videoState);

        If (VDBG) {
            Rlog->D(LOG_TAG, "End Dial(" + basePhone + ", "+ dialString + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        return result;
    }

    /**
     * Initiate a new voice connection. This happens asynchronously, so you
     * cannot assume the audio path is Connected (or a call index has been
     * assigned) until PhoneStateChanged notification has occurred.
     *
     * @exception CallStateException if a new outgoing call is not currently
     * possible because no more call slots exist or a call exists that is
     * dialing, alerting, ringing, or waiting.  Other errors are
     * handled asynchronously.
     */
    public Connection Dial(Phone phone, String dialString, UUSInfo uusInfo, Int32 videoState)
            throws CallStateException {
        return phone->Dial(dialString, uusInfo, videoState);
    }

    /**
     * clear disconnect connection for each phone
     */
    CARAPI ClearDisconnected() {
        For(Phone phone : mPhones) {
            phone->ClearDisconnected();
        }
    }

    /**
     * clear disconnect connection for a phone specific
     * to the provided subId
     */
    CARAPI ClearDisconnected(Int64 subId) {
        For(Phone phone : mPhones) {
            If (phone->GetSubId() == subId) {
                phone->ClearDisconnected();
            }
        }
    }

    /**
     * Phone can make a call only if ALL of the following are TRUE:
     *        - Phone is not powered off
     *        - There's no incoming or waiting call
     *        - The foreground call is ACTIVE or IDLE or DISCONNECTED.
     *          (We mainly need to make sure it *isn't* DIALING or ALERTING.)
     * @param phone
     * @return TRUE if the phone can make a new call
     */
    private Boolean CanDial(Phone phone) {
        Int32 serviceState = phone->GetServiceState()->GetState();
        Int64 subId = phone->GetSubId();
        Boolean hasRingingCall = HasActiveRingingCall();
        Call.State fgCallState = GetActiveFgCallState(subId);

        Boolean result = (serviceState != ServiceState.STATE_POWER_OFF
                && !hasRingingCall
                && ((fgCallState == Call.State.ACTIVE)
                    || (fgCallState == Call.State.IDLE)
                    || (fgCallState == Call.State.DISCONNECTED)
                    /*As per 3GPP TS 51.010-1 section 31.13.1.4
                    call should be alowed when the foreground
                    call is in ALERTING state*/
                    || (fgCallState == Call.State.ALERTING)));

        If (result == FALSE) {
            Rlog->D(LOG_TAG, "canDial serviceState=" + serviceState
                            + " hasRingingCall=" + hasRingingCall
                            + " fgCallState=" + fgCallState);
        }
        return result;
    }

    /**
     * Whether or not the phone can do explicit call transfer in the current
     * phone state--that is, one call holding and one call active.
     * @return TRUE if the phone can do explicit call transfer; FALSE otherwise.
     */
    public Boolean CanTransfer(Call heldCall) {
        Phone activePhone = NULL;
        Phone heldPhone = NULL;

        If (HasActiveFgCall()) {
            activePhone = GetActiveFgCall()->GetPhone();
        }

        If (heldCall != NULL) {
            heldPhone = heldCall->GetPhone();
        }

        Return (heldPhone == activePhone && activePhone->CanTransfer());
    }

    /**
     * Whether or not the phone specific to subId can do explicit call transfer
     * in the current phone state--that is, one call holding and one call active.
     * @return TRUE if the phone can do explicit call transfer; FALSE otherwise.
     */
    public Boolean CanTransfer(Call heldCall, Int64 subId) {
        Phone activePhone = NULL;
        Phone heldPhone = NULL;

        If (HasActiveFgCall(subId)) {
            activePhone = GetActiveFgCall(subId).GetPhone();
        }

        If (heldCall != NULL) {
            heldPhone = heldCall->GetPhone();
        }

        Return (heldPhone == activePhone && activePhone->CanTransfer());
    }

    /**
     * Connects the held call and active call
     * Disconnects the subscriber from both calls
     *
     * Explicit Call Transfer occurs asynchronously
     * and may fail. Final notification occurs via
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}.
     *
     * @exception CallStateException if CanTransfer() would return FALSE.
     * In these cases, this operation may not be performed.
     */
    CARAPI ExplicitCallTransfer(Call heldCall) throws CallStateException {
        If (VDBG) {
            Rlog->D(LOG_TAG, " ExplicitCallTransfer(" + heldCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        If (CanTransfer(heldCall)) {
            heldCall->GetPhone()->ExplicitCallTransfer();
        }

        If (VDBG) {
            Rlog->D(LOG_TAG, "End ExplicitCallTransfer(" + heldCall + ")");
            Rlog->D(LOG_TAG, ToString());
        }

    }

    /**
     * Returns a list of MMI codes that are pending for a phone. (They have initiated
     * but have not yet completed).
     * Presently there is only ever one.
     *
     * Use <code>registerForMmiInitiate</code>
     * and <code>registerForMmiComplete</code> for change notification.
     * @return NULL if phone doesn't have or support mmi code
     */
    public List<? extends MmiCode> GetPendingMmiCodes(Phone phone) {
        Rlog->E(LOG_TAG, "getPendingMmiCodes not implemented");
        return NULL;
    }

    /**
     * Sends user response to a USSD REQUEST message.  An MmiCode instance
     * representing this response is sent to handlers registered with
     * registerForMmiInitiate.
     *
     * @param ussdMessge    Message to send in the response.
     * @return FALSE if phone doesn't support ussd service
     */
    public Boolean SendUssdResponse(Phone phone, String ussdMessge) {
        Rlog->E(LOG_TAG, "sendUssdResponse not implemented");
        return FALSE;
    }

    /**
     * Mutes or unmutes the microphone for the active call. The microphone
     * is automatically unmuted if a call is answered, dialed, or resumed
     * from a holding state.
     *
     * @param muted TRUE to mute the microphone,
     * FALSE to activate the microphone.
     */

    CARAPI SetMute(Boolean muted) {
        If (VDBG) {
            Rlog->D(LOG_TAG, " SetMute(" + muted + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        If (HasActiveFgCall()) {
            GetActiveFgCall()->GetPhone().SetMute(muted);
        }

        If (VDBG) {
            Rlog->D(LOG_TAG, "End SetMute(" + muted + ")");
            Rlog->D(LOG_TAG, ToString());
        }
    }

    /**
     * Gets current mute status. Use
     * {@link #RegisterForPreciseCallStateChanged(android.os.Handler, Int32,
     * java.lang.Object) RegisterForPreciseCallStateChanged()}
     * as a change notifcation, although presently phone state changed is not
     * fired when SetMute() is called.
     *
     * @return TRUE is muting, FALSE is unmuting
     */
    public Boolean GetMute() {
        If (HasActiveFgCall()) {
            return GetActiveFgCall()->GetPhone().GetMute();
        } else If (HasActiveBgCall()) {
            return GetFirstActiveBgCall()->GetPhone().GetMute();
        }
        return FALSE;
    }

    /**
     * Enables or disables echo suppression.
     */
    CARAPI SetEchoSuppressionEnabled() {
        If (VDBG) {
            Rlog->D(LOG_TAG, " SetEchoSuppression()");
            Rlog->D(LOG_TAG, ToString());
        }

        If (HasActiveFgCall()) {
            GetActiveFgCall()->GetPhone().SetEchoSuppressionEnabled();
        }

        If (VDBG) {
            Rlog->D(LOG_TAG, "End SetEchoSuppression()");
            Rlog->D(LOG_TAG, ToString());
        }
    }

    /**
     * Play a DTMF tone on the active call.
     *
     * @param c should be one of 0-9, '*' or '#'. Other values will be
     * silently ignored.
     * @return FALSE if no active call or the active call doesn't support
     *         dtmf tone
     */
    public Boolean SendDtmf(Char32 c) {
        Boolean result = FALSE;

        If (VDBG) {
            Rlog->D(LOG_TAG, " SendDtmf(" + c + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        If (HasActiveFgCall()) {
            GetActiveFgCall()->GetPhone().SendDtmf(c);
            result = TRUE;
        }

        If (VDBG) {
            Rlog->D(LOG_TAG, "End SendDtmf(" + c + ")");
            Rlog->D(LOG_TAG, ToString());
        }
        return result;
    }

    /**
     * Start to paly a DTMF tone on the active call.
     * or there is a playing DTMF tone.
     * @param c should be one of 0-9, '*' or '#'. Other values will be
     * silently ignored.
     *
     * @return FALSE if no active call or the active call doesn't support
     *         dtmf tone
     */
    public Boolean StartDtmf(Char32 c) {
        Boolean result = FALSE;

        If (VDBG) {
            Rlog->D(LOG_TAG, " StartDtmf(" + c + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        If (HasActiveFgCall()) {
            GetActiveFgCall()->GetPhone().StartDtmf(c);
            result = TRUE;
        }

        If (VDBG) {
            Rlog->D(LOG_TAG, "End StartDtmf(" + c + ")");
            Rlog->D(LOG_TAG, ToString());
        }

        return result;
    }

    /**
     * Stop the playing DTMF tone. Ignored if there is no playing DTMF
     * tone or no active call.
     */
    CARAPI StopDtmf() {
        If (VDBG) {
            Rlog->D(LOG_TAG, " StopDtmf()" );
            Rlog->D(LOG_TAG, ToString());
        }

        If (HasActiveFgCall()) GetFgPhone()->StopDtmf();

        If (VDBG) {
            Rlog->D(LOG_TAG, "End StopDtmf()");
            Rlog->D(LOG_TAG, ToString());
        }
    }

    /**
     * send burst DTMF tone, it can send the string as single character or multiple character
     * ignore if there is no active call or not valid digits string.
     * Valid digit means only includes characters ISO-LATIN characters 0-9, *, #
     * The difference between sendDtmf and sendBurstDtmf is sendDtmf only sends one character,
     * this api can send single character and multiple character, also, this api has response
     * back to caller.
     *
     * @param dtmfString is string representing the dialing Digit(s) in the active call
     * @param on the DTMF ON length in milliseconds, or 0 for default
     * @param off the DTMF OFF length in milliseconds, or 0 for default
     * @param onComplete is the callback message when the action is processed by BP
     *
     */
    public Boolean SendBurstDtmf(String dtmfString, Int32 on, Int32 off, Message onComplete) {
        If (HasActiveFgCall()) {
            GetActiveFgCall()->GetPhone().SendBurstDtmf(dtmfString, on, off, onComplete);
            return TRUE;
        }
        return FALSE;
    }

    /**
     * Notifies when a voice connection has disconnected, either due to local
     * or remote hangup or error.
     *
     *  Messages received from this will have the following members:<p>
     *  <ul><li>Message.obj will be an AsyncResult</li>
     *  <li>AsyncResult.userObj = obj</li>
     *  <li>AsyncResult.result = a Connection object that is
     *  no longer connected.</li></ul>
     */
    CARAPI RegisterForDisconnect(Handler h, Int32 what, Object obj) {
        mDisconnectRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for voice disconnection notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForDisconnect(Handler h){
        mDisconnectRegistrants->Remove(h);
    }

    /**
     * Register for getting notifications for change in the Call State {@link Call.State}
     * This is called PreciseCallState because the call state is more precise than what
     * can be obtained using the {@link PhoneStateListener}
     *
     * Resulting events will have an AsyncResult in <code>Message.obj</code>.
     * AsyncResult.userData will be set to the obj argument here.
     * The <em>h</em> parameter is held only by a weak reference.
     */
    CARAPI RegisterForPreciseCallStateChanged(Handler h, Int32 what, Object obj){
        mPreciseCallStateRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for voice call state change notifications.
     * Extraneous calls are tolerated silently.
     */
    CARAPI UnregisterForPreciseCallStateChanged(Handler h){
        mPreciseCallStateRegistrants->Remove(h);
    }

    /**
     * Notifies when a previously untracked non-ringing/waiting connection has appeared.
     * This is likely due to some other Entity (eg, SIM card application) initiating a call.
     */
    CARAPI RegisterForUnknownConnection(Handler h, Int32 what, Object obj){
        mUnknownConnectionRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for unknown connection notifications.
     */
    CARAPI UnregisterForUnknownConnection(Handler h){
        mUnknownConnectionRegistrants->Remove(h);
    }


    /**
     * Notifies when a new ringing or waiting connection has appeared.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = a Connection. <p>
     *  Please check Connection->IsRinging() to make sure the Connection
     *  has not dropped since this message was posted.
     *  If Connection->IsRinging() is TRUE, then
     *   Connection->GetCall() == Phone->GetRingingCall()
     */
    CARAPI RegisterForNewRingingConnection(Handler h, Int32 what, Object obj){
        mNewRingingConnectionRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for new ringing connection notification.
     * Extraneous calls are tolerated silently
     */

    CARAPI UnregisterForNewRingingConnection(Handler h){
        mNewRingingConnectionRegistrants->Remove(h);
    }

    /**
     * Notifies when an incoming call rings.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = a Connection. <p>
     */
    CARAPI RegisterForIncomingRing(Handler h, Int32 what, Object obj){
        mIncomingRingRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for ring notification.
     * Extraneous calls are tolerated silently
     */

    CARAPI UnregisterForIncomingRing(Handler h){
        mIncomingRingRegistrants->Remove(h);
    }

    /**
     * Notifies when out-band ringback tone is needed.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = Boolean, TRUE to start play ringback tone
     *                       and FALSE to stop. <p>
     */
    CARAPI RegisterForRingbackTone(Handler h, Int32 what, Object obj){
        mRingbackToneRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for ringback tone notification.
     */

    CARAPI UnregisterForRingbackTone(Handler h){
        mRingbackToneRegistrants->Remove(h);
    }

    /**
     * Notifies when out-band on-hold tone is needed.<p>
     *
     *  Messages received from this:
     *  Message.obj will be an AsyncResult
     *  AsyncResult.userObj = obj
     *  AsyncResult.result = Boolean, TRUE to start play on-hold tone
     *                       and FALSE to stop. <p>
     */
    CARAPI RegisterForOnHoldTone(Handler h, Int32 what, Object obj){
        mOnHoldToneRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for on-hold tone notification.
     */

    CARAPI UnregisterForOnHoldTone(Handler h){
        mOnHoldToneRegistrants->Remove(h);
    }

    /**
     * Registers the handler to reset the uplink mute state to get
     * uplink audio.
     */
    CARAPI RegisterForResendIncallMute(Handler h, Int32 what, Object obj){
        mResendIncallMuteRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for resend incall mute notifications.
     */
    CARAPI UnregisterForResendIncallMute(Handler h){
        mResendIncallMuteRegistrants->Remove(h);
    }

    /**
     * Register for notifications of initiation of a new MMI code request.
     * MMI codes for GSM are discussed in 3GPP TS 22.030.<p>
     *
     * Example: If Phone.dial is called with "*#31#", then the app will
     * be notified here.<p>
     *
     * The returned <code>Message.obj</code> will contain an AsyncResult.
     *
     * <code>obj.result</code> will be an "MmiCode" object.
     */
    CARAPI RegisterForMmiInitiate(Handler h, Int32 what, Object obj){
        mMmiInitiateRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for new MMI initiate notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForMmiInitiate(Handler h){
        mMmiInitiateRegistrants->Remove(h);
    }

    /**
     * Register for notifications that an MMI request has completed
     * its network activity and is in its final state. This may mean a state
     * of COMPLETE, FAILED, or CANCELLED.
     *
     * <code>Message.obj</code> will contain an AsyncResult.
     * <code>obj.result</code> will be an "MmiCode" object
     */
    CARAPI RegisterForMmiComplete(Handler h, Int32 what, Object obj){
        mMmiCompleteRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for MMI complete notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForMmiComplete(Handler h){
        mMmiCompleteRegistrants->Remove(h);
    }

    /**
     * Registration point for Ecm timer reset
     * @param h handler to notify
     * @param what user-defined message code
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForEcmTimerReset(Handler h, Int32 what, Object obj){
        mEcmTimerResetRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregister for notification for Ecm timer reset
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForEcmTimerReset(Handler h){
        mEcmTimerResetRegistrants->Remove(h);
    }

    /**
     * Register for ServiceState changed.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a ServiceState instance
     */
    CARAPI RegisterForServiceStateChanged(Handler h, Int32 what, Object obj){
        mServiceStateChangedRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for ServiceStateChange notification.
     * Extraneous calls are tolerated silently
     */
    CARAPI UnregisterForServiceStateChanged(Handler h){
        mServiceStateChangedRegistrants->Remove(h);
    }

    /**
     * Register for supplementary service notifications.
     * Message.obj will contain an AsyncResult.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForSuppServiceNotification(Handler h, Int32 what, Object obj) {
        mSuppServiceNotifyRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregister for supplementary service notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSuppServiceNotification(Handler h) {
        mSuppServiceNotifyRegistrants->Remove(h);
    }

    /**
     * Register for notifications when a supplementary service attempt fails.
     * Message.obj will contain an AsyncResult.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForSuppServiceFailed(Handler h, Int32 what, Object obj){
        mSuppServiceFailedRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregister for notifications when a supplementary service attempt fails.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSuppServiceFailed(Handler h){
        mSuppServiceFailedRegistrants->Remove(h);
    }

    /**
     * Register for notifications when a sInCall VoicePrivacy is enabled
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForInCallVoicePrivacyOn(Handler h, Int32 what, Object obj){
        mInCallVoicePrivacyOnRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregister for notifications when a sInCall VoicePrivacy is enabled
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForInCallVoicePrivacyOn(Handler h){
        mInCallVoicePrivacyOnRegistrants->Remove(h);
    }

    /**
     * Register for notifications when a sInCall VoicePrivacy is disabled
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForInCallVoicePrivacyOff(Handler h, Int32 what, Object obj){
        mInCallVoicePrivacyOffRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregister for notifications when a sInCall VoicePrivacy is disabled
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForInCallVoicePrivacyOff(Handler h){
        mInCallVoicePrivacyOffRegistrants->Remove(h);
    }

    /**
     * Register for notifications when CDMA call waiting comes
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForCallWaiting(Handler h, Int32 what, Object obj){
        mCallWaitingRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregister for notifications when CDMA Call waiting comes
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForCallWaiting(Handler h){
        mCallWaitingRegistrants->Remove(h);
    }


    /**
     * Register for signal information notifications from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a SuppServiceNotification instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */

    CARAPI RegisterForSignalInfo(Handler h, Int32 what, Object obj){
        mSignalInfoRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for signal information notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSignalInfo(Handler h){
        mSignalInfoRegistrants->Remove(h);
    }

    /**
     * Register for display information notifications from the network.
     * Message.obj will contain an AsyncResult.
     * AsyncResult.result will be a SuppServiceNotification instance.
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForDisplayInfo(Handler h, Int32 what, Object obj){
        mDisplayInfoRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregisters for display information notifications.
     * Extraneous calls are tolerated silently
     *
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForDisplayInfo(Handler h) {
        mDisplayInfoRegistrants->Remove(h);
    }

    /**
     * Register for notifications when CDMA OTA Provision status change
     *
     * @param h Handler that receives the notification message.
     * @param what User-defined message code.
     * @param obj User object.
     */
    CARAPI RegisterForCdmaOtaStatusChange(Handler h, Int32 what, Object obj){
        mCdmaOtaStatusChangeRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregister for notifications when CDMA OTA Provision status change
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForCdmaOtaStatusChange(Handler h){
        mCdmaOtaStatusChangeRegistrants->Remove(h);
    }

    /**
     * Registration point for subcription info ready
     * @param h handler to notify
     * @param what what code of message when delivered
     * @param obj placed in Message.obj
     */
    CARAPI RegisterForSubscriptionInfoReady(Handler h, Int32 what, Object obj){
        mSubscriptionInfoReadyRegistrants->AddUnique(h, what, obj);
    }

    /**
     * Unregister for notifications for subscription info
     * @param h Handler to be removed from the registrant list.
     */
    CARAPI UnregisterForSubscriptionInfoReady(Handler h){
        mSubscriptionInfoReadyRegistrants->Remove(h);
    }

    CARAPI RegisterForSubscriptionChange(Handler h, Int32 what, Object obj) {
        mActiveSubChangeRegistrants->AddUnique(h, what, obj);
    }

    CARAPI UnregisterForSubscriptionChange(Handler h) {
        mActiveSubChangeRegistrants->Remove(h);
    }

    CARAPI SetActiveSubscription(Int64 subscription) {
        Rlog->D(LOG_TAG, "setActiveSubscription existing:" + mActiveSub + "new = " + subscription);
        mActiveSub = subscription;
        mActiveSubChangeRegistrants->NotifyRegistrants(new AsyncResult (NULL, mActiveSub, NULL));
    }

    /**
     * Sets an event to be fired when the telephony system processes
     * a post-dial character on an outgoing call.<p>
     *
     * Messages of type <code>what</code> will be sent to <code>h</code>.
     * The <code>obj</code> field of these Message's will be instances of
     * <code>AsyncResult</code>. <code>Message.obj.result</code> will be
     * a Connection object.<p>
     *
     * Message.arg1 will be the post dial character being processed,
     * or 0 ('\0') if end of string.<p>
     *
     * If Connection->GetPostDialState() == WAIT,
     * the application must call
     * {@link com.android.internal.telephony.Connection#ProceedAfterWaitChar()
     * Connection->ProceedAfterWaitChar()} or
     * {@link com.android.internal.telephony.Connection#CancelPostDial()
     * Connection->CancelPostDial()}
     * for the telephony system to continue playing the post-dial
     * DTMF sequence.<p>
     *
     * If Connection->GetPostDialState() == WILD,
     * the application must call
     * {@link com.android.internal.telephony.Connection#proceedAfterWildChar
     * Connection->ProceedAfterWildChar()}
     * or
     * {@link com.android.internal.telephony.Connection#CancelPostDial()
     * Connection->CancelPostDial()}
     * for the telephony system to continue playing the
     * post-dial DTMF sequence.<p>
     *
     */
    CARAPI RegisterForPostDialCharacter(Handler h, Int32 what, Object obj){
        mPostDialCharacterRegistrants->AddUnique(h, what, obj);
    }

    CARAPI UnregisterForPostDialCharacter(Handler h){
        mPostDialCharacterRegistrants->Remove(h);
    }

    /* APIs to access foregroudCalls, backgroudCalls, and ringingCalls
     * 1. APIs to access list of calls
     * 2. APIs to check if any active call, which has connection other than
     * disconnected ones, pleaser refer to Call->IsIdle()
     * 3. APIs to return first active call
     * 4. APIs to return the connections of first active call
     * 5. APIs to return other property of first active call
     */

    /**
     * @return list of all ringing calls
     */
    public List<Call> GetRingingCalls() {
        return Collections->UnmodifiableList(mRingingCalls);
    }

    /**
     * @return list of all foreground calls
     */
    public List<Call> GetForegroundCalls() {
        return Collections->UnmodifiableList(mForegroundCalls);
    }

    /**
     * @return list of all background calls
     */
    public List<Call> GetBackgroundCalls() {
        return Collections->UnmodifiableList(mBackgroundCalls);
    }

    /**
     * Return TRUE if there is at least one active foreground call
     */
    public Boolean HasActiveFgCall() {
        Return (GetFirstActiveCall(mForegroundCalls) != NULL);
    }

    /**
     * Return TRUE if there is at least one active foreground call
     * on a particular subId or an active sip call
     */
    public Boolean HasActiveFgCall(Int64 subId) {
        Return (GetFirstActiveCall(mForegroundCalls, subId) != NULL);
    }

    /**
     * Return TRUE if there is at least one active background call
     */
    public Boolean HasActiveBgCall() {
        // TODO since hasActiveBgCall may get called often
        // better to cache it to improve performance
        Return (GetFirstActiveCall(mBackgroundCalls) != NULL);
    }

    /**
     * Return TRUE if there is at least one active background call
     * on a particular subId or an active sip call
     */
    public Boolean HasActiveBgCall(Int64 subId) {
        // TODO since hasActiveBgCall may get called often
        // better to cache it to improve performance
        Return (GetFirstActiveCall(mBackgroundCalls, subId) != NULL);
    }

    /**
     * Return TRUE if there is at least one active ringing call
     *
     */
    public Boolean HasActiveRingingCall() {
        Return (GetFirstActiveCall(mRingingCalls) != NULL);
    }

    /**
     * Return TRUE if there is at least one active ringing call
     */
    public Boolean HasActiveRingingCall(Int64 subId) {
        Return (GetFirstActiveCall(mRingingCalls, subId) != NULL);
    }

    /**
     * return the active foreground call from foreground calls
     *
     * Active call means the call is NOT in Call.State.IDLE
     *
     * 1. If there is active foreground call, return it
     * 2. If there is no active foreground call, return the
     *    foreground call associated with default phone, which state is IDLE.
     * 3. If there is no phone registered at all, return NULL.
     *
     */
    public Call GetActiveFgCall() {
        Call call = GetFirstNonIdleCall(mForegroundCalls);
        If (call == NULL) {
            call = (mDefaultPhone == NULL)
                    ? NULL
                    : mDefaultPhone->GetForegroundCall();
        }
        return call;
    }

    public Call GetActiveFgCall(Int64 subId) {
        Call call = GetFirstNonIdleCall(mForegroundCalls, subId);
        If (call == NULL) {
            Phone phone = GetPhone(subId);
            call = (phone == NULL)
                    ? NULL
                    : phone->GetForegroundCall();
        }
        return call;
    }

    // Returns the first call that is not in IDLE state. If both active calls
    // and disconnecting/disconnected calls exist, return the first active call.
    private Call GetFirstNonIdleCall(List<Call> calls) {
        Call result = NULL;
        For (Call call : calls) {
            If (!call->IsIdle()) {
                return call;
            } else If (call->GetState() != Call.State.IDLE) {
                If (result == NULL) result = call;
            }
        }
        return result;
    }

    // Returns the first call that is not in IDLE state. If both active calls
    // and disconnecting/disconnected calls exist, return the first active call.
    private Call GetFirstNonIdleCall(List<Call> calls, Int64 subId) {
        Call result = NULL;
        For (Call call : calls) {
            If ((call->GetPhone()->GetSubId() == subId) ||
                    (call->GetPhone() instanceof SipPhone)) {
                If (!call->IsIdle()) {
                    return call;
                } else If (call->GetState() != Call.State.IDLE) {
                    If (result == NULL) result = call;
                }
            }
        }
        return result;
    }

    /**
     * return one active background call from background calls
     *
     * Active call means the call is NOT idle defined by Call->IsIdle()
     *
     * 1. If there is only one active background call, return it
     * 2. If there is more than one active background call, return the first one
     * 3. If there is no active background call, return the background call
     *    associated with default phone, which state is IDLE.
     * 4. If there is no background call at all, return NULL.
     *
     * Complete background calls list can be get by GetBackgroundCalls()
     */
    public Call GetFirstActiveBgCall() {
        Call call = GetFirstNonIdleCall(mBackgroundCalls);
        If (call == NULL) {
            call = (mDefaultPhone == NULL)
                    ? NULL
                    : mDefaultPhone->GetBackgroundCall();
        }
        return call;
    }

    /**
     * return one active background call from background calls of the
     * requested subId.
     *
     * Active call means the call is NOT idle defined by Call->IsIdle()
     *
     * 1. If there is only one active background call on given sub or
     *    on SIP Phone, return it
     * 2. If there is more than one active background call, return the background call
     *    associated with the active sub.
     * 3. If there is no background call at all, return NULL.
     *
     * Complete background calls list can be get by GetBackgroundCalls()
     */
    public Call GetFirstActiveBgCall(Int64 subId) {
        Phone phone = GetPhone(subId);
        If (HasMoreThanOneHoldingCall(subId)) {
            return phone->GetBackgroundCall();
        } else {
            Call call = GetFirstNonIdleCall(mBackgroundCalls, subId);
            If (call == NULL) {
                call = (phone == NULL)
                        ? NULL
                        : phone->GetBackgroundCall();
            }
            return call;
        }
    }

    /**
     * return one active ringing call from ringing calls
     *
     * Active call means the call is NOT idle defined by Call->IsIdle()
     *
     * 1. If there is only one active ringing call, return it
     * 2. If there is more than one active ringing call, return the first one
     * 3. If there is no active ringing call, return the ringing call
     *    associated with default phone, which state is IDLE.
     * 4. If there is no ringing call at all, return NULL.
     *
     * Complete ringing calls list can be get by GetRingingCalls()
     */
    public Call GetFirstActiveRingingCall() {
        Call call = GetFirstNonIdleCall(mRingingCalls);
        If (call == NULL) {
            call = (mDefaultPhone == NULL)
                    ? NULL
                    : mDefaultPhone->GetRingingCall();
        }
        return call;
    }

    public Call GetFirstActiveRingingCall(Int64 subId) {
        Phone phone = GetPhone(subId);
        Call call = GetFirstNonIdleCall(mRingingCalls, subId);
        If (call == NULL) {
            call = (phone == NULL)
                    ? NULL
                    : phone->GetRingingCall();
        }
        return call;
    }

    /**
     * @return the state of active foreground call
     * return IDLE if there is no active foreground call
     */
    public Call.State GetActiveFgCallState() {
        Call fgCall = GetActiveFgCall();

        If (fgCall != NULL) {
            return fgCall->GetState();
        }

        return Call.State.IDLE;
    }

    public Call.State GetActiveFgCallState(Int64 subId) {
        Call fgCall = GetActiveFgCall(subId);

        If (fgCall != NULL) {
            return fgCall->GetState();
        }

        return Call.State.IDLE;
    }

    /**
     * @return the connections of active foreground call
     * return empty list if there is no active foreground call
     */
    public List<Connection> GetFgCallConnections() {
        Call fgCall = GetActiveFgCall();
        If ( fgCall != NULL) {
            return fgCall->GetConnections();
        }
        return mEmptyConnections;
    }

    /**
     * @return the connections of active foreground call
     * return empty list if there is no active foreground call
     */
    public List<Connection> GetFgCallConnections(Int64 subId) {
        Call fgCall = GetActiveFgCall(subId);
        If ( fgCall != NULL) {
            return fgCall->GetConnections();
        }
        return mEmptyConnections;
    }

    /**
     * @return the connections of active background call
     * return empty list if there is no active background call
     */
    public List<Connection> GetBgCallConnections() {
        Call bgCall = GetFirstActiveBgCall();
        If ( bgCall != NULL) {
            return bgCall->GetConnections();
        }
        return mEmptyConnections;
    }

    /**
     * @return the connections of active background call
     * return empty list if there is no active background call
     */
    public List<Connection> GetBgCallConnections(Int64 subId) {
        Call bgCall = GetFirstActiveBgCall(subId);
        If ( bgCall != NULL) {
            return bgCall->GetConnections();
        }
        return mEmptyConnections;
    }

    /**
     * @return the latest connection of active foreground call
     * return NULL if there is no active foreground call
     */
    public Connection GetFgCallLatestConnection() {
        Call fgCall = GetActiveFgCall();
        If ( fgCall != NULL) {
            return fgCall->GetLatestConnection();
        }
        return NULL;
    }

    /**
     * @return the latest connection of active foreground call
     * return NULL if there is no active foreground call
     */
    public Connection GetFgCallLatestConnection(Int64 subId) {
        Call fgCall = GetActiveFgCall(subId);
        If ( fgCall != NULL) {
            return fgCall->GetLatestConnection();
        }
        return NULL;
    }

    /**
     * @return TRUE if there is at least one Foreground call in disconnected state
     */
    public Boolean HasDisconnectedFgCall() {
        Return (GetFirstCallOfState(mForegroundCalls, Call.State.DISCONNECTED) != NULL);
    }

    /**
     * @return TRUE if there is at least one Foreground call in disconnected state
     */
    public Boolean HasDisconnectedFgCall(Int64 subId) {
        Return (GetFirstCallOfState(mForegroundCalls, Call.State.DISCONNECTED,
                subId) != NULL);
    }

    /**
     * @return TRUE if there is at least one background call in disconnected state
     */
    public Boolean HasDisconnectedBgCall() {
        Return (GetFirstCallOfState(mBackgroundCalls, Call.State.DISCONNECTED) != NULL);
    }

    /**
     * @return TRUE if there is at least one background call in disconnected state
     */
    public Boolean HasDisconnectedBgCall(Int64 subId) {
        Return (GetFirstCallOfState(mBackgroundCalls, Call.State.DISCONNECTED,
                subId) != NULL);
    }


    /**
     * @return the first active call from a call list
     */
    private  Call GetFirstActiveCall(ArrayList<Call> calls) {
        For (Call call : calls) {
            If (!call->IsIdle()) {
                return call;
            }
        }
        return NULL;
    }

    /**
     * @return the first active call from a call list
     */
    private  Call GetFirstActiveCall(ArrayList<Call> calls, Int64 subId) {
        For (Call call : calls) {
            If ((!call->IsIdle()) && ((call->GetPhone()->GetSubId() == subId) ||
                    (call->GetPhone() instanceof SipPhone))) {
                return call;
            }
        }
        return NULL;
    }

    /**
     * @return the first call in a the Call.state from a call list
     */
    private Call GetFirstCallOfState(ArrayList<Call> calls, Call.State state) {
        For (Call call : calls) {
            If (call->GetState() == state) {
                return call;
            }
        }
        return NULL;
    }

    /**
     * @return the first call in a the Call.state from a call list
     */
    private Call GetFirstCallOfState(ArrayList<Call> calls, Call.State state,
            Int64 subId) {
        For (Call call : calls) {
            If ((call->GetState() == state) ||
                ((call->GetPhone()->GetSubId() == subId) ||
                (call->GetPhone() instanceof SipPhone))) {
                return call;
            }
        }
        return NULL;
    }

    private Boolean HasMoreThanOneRingingCall() {
        Int32 count = 0;
        For (Call call : mRingingCalls) {
            If (call->GetState()->IsRinging()) {
                If (++count > 1) return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * @return TRUE if more than one active ringing call exists on
     * the active subId.
     * This checks for the active calls on provided
     * subId and also active calls on SIP Phone.
     *
     */
    private Boolean HasMoreThanOneRingingCall(Int64 subId) {
        Int32 count = 0;
        For (Call call : mRingingCalls) {
            If ((call->GetState()->IsRinging()) &&
                ((call->GetPhone()->GetSubId() == subId) ||
                (call->GetPhone() instanceof SipPhone))) {
                If (++count > 1) return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * @return TRUE if more than one active background call exists on
     * the provided subId.
     * This checks for the background calls on provided
     * subId and also background calls on SIP Phone.
     *
     */
    private Boolean HasMoreThanOneHoldingCall(Int64 subId) {
        Int32 count = 0;
        For (Call call : mBackgroundCalls) {
            If ((call->GetState() == Call.State.HOLDING) &&
                ((call->GetPhone()->GetSubId() == subId) ||
                (call->GetPhone() instanceof SipPhone))) {
                If (++count > 1) return TRUE;
            }
        }
        return FALSE;
    }

    private class CallManagerHandler extends Handler {
        //@Override
        CARAPI HandleMessage(Message msg) {

            Switch (msg.what) {
                case EVENT_DISCONNECT:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_DISCONNECT)");
                    mDisconnectRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_PRECISE_CALL_STATE_CHANGED:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_PRECISE_CALL_STATE_CHANGED)");
                    mPreciseCallStateRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_NEW_RINGING_CONNECTION:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_NEW_RINGING_CONNECTION)");
                    Connection c = (Connection) ((AsyncResult) msg.obj).result;
                    Int64 subId = c->GetCall()->GetPhone().GetSubId();
                    If (GetActiveFgCallState(subId).IsDialing() ||
                            HasMoreThanOneRingingCall(subId)) {
                        try {
                            Rlog->D(LOG_TAG, "silently drop incoming call: " + c->GetCall());
                            c->GetCall()->Hangup();
                        } Catch (CallStateException e) {
                            Rlog->W(LOG_TAG, "new ringing connection", e);
                        }
                    } else {
                        mNewRingingConnectionRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    }
                    break;
                case EVENT_UNKNOWN_CONNECTION:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_UNKNOWN_CONNECTION)");
                    mUnknownConnectionRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_INCOMING_RING:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_INCOMING_RING)");
                    // The event may come from RIL who's not aware of an ongoing fg call
                    If (!HasActiveFgCall()) {
                        mIncomingRingRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    }
                    break;
                case EVENT_RINGBACK_TONE:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_RINGBACK_TONE)");
                    mRingbackToneRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_IN_CALL_VOICE_PRIVACY_ON:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_IN_CALL_VOICE_PRIVACY_ON)");
                    mInCallVoicePrivacyOnRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_IN_CALL_VOICE_PRIVACY_OFF:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_IN_CALL_VOICE_PRIVACY_OFF)");
                    mInCallVoicePrivacyOffRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_CALL_WAITING:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_CALL_WAITING)");
                    mCallWaitingRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_DISPLAY_INFO:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_DISPLAY_INFO)");
                    mDisplayInfoRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_SIGNAL_INFO:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_SIGNAL_INFO)");
                    mSignalInfoRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_CDMA_OTA_STATUS_CHANGE:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_CDMA_OTA_STATUS_CHANGE)");
                    mCdmaOtaStatusChangeRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_RESEND_INCALL_MUTE:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_RESEND_INCALL_MUTE)");
                    mResendIncallMuteRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_MMI_INITIATE:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_MMI_INITIATE)");
                    mMmiInitiateRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_MMI_COMPLETE:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_MMI_COMPLETE)");
                    mMmiCompleteRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_ECM_TIMER_RESET:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_ECM_TIMER_RESET)");
                    mEcmTimerResetRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_SUBSCRIPTION_INFO_READY:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_SUBSCRIPTION_INFO_READY)");
                    mSubscriptionInfoReadyRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_SUPP_SERVICE_NOTIFY:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_SUPP_SERVICE_NOTIFY)");
                    mSuppServiceNotifyRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_SUPP_SERVICE_FAILED:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_SUPP_SERVICE_FAILED)");
                    mSuppServiceFailedRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_SERVICE_STATE_CHANGED:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_SERVICE_STATE_CHANGED)");
                    mServiceStateChangedRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
                case EVENT_POST_DIAL_CHARACTER:
                    // we need send the character that is being processed in msg.arg1
                    // so can't use NotifyRegistrants()
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_POST_DIAL_CHARACTER)");
                    For(Int32 i=0; i < mPostDialCharacterRegistrants->Size(); i++) {
                        Message notifyMsg;
                        notifyMsg = ((Registrant)mPostDialCharacterRegistrants->Get(i)).MessageForRegistrant();
                        notifyMsg.obj = msg.obj;
                        notifyMsg.arg1 = msg.arg1;
                        notifyMsg->SendToTarget();
                    }
                    break;
                case EVENT_ONHOLD_TONE:
                    If (VDBG) Rlog->D(LOG_TAG, " HandleMessage (EVENT_ONHOLD_TONE)");
                    mOnHoldToneRegistrants->NotifyRegistrants((AsyncResult) msg.obj);
                    break;
            }
        }
    };

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        Call call;
        StringBuilder b = new StringBuilder();
        For (Int32 i = 0; i < TelephonyManager->GetDefault()->GetPhoneCount(); i++) {
            b->Append("CallManager {");
            b->Append("\nstate = " + GetState(i));
            call = GetActiveFgCall(i);
            b->Append("\n- Foreground: " + GetActiveFgCallState(i));
            b->Append(" from " + call->GetPhone());
            b->Append("\n  Conn: ").Append(GetFgCallConnections(i));
            call = GetFirstActiveBgCall(i);
            b->Append("\n- Background: " + call->GetState());
            b->Append(" from " + call->GetPhone());
            b->Append("\n  Conn: ").Append(GetBgCallConnections(i));
            call = GetFirstActiveRingingCall(i);
            b->Append("\n- Ringing: " +call->GetState());
            b->Append(" from " + call->GetPhone());
        }

        For (Phone phone : GetAllPhones()) {
            If (phone != NULL) {
                b->Append("\nPhone: " + phone + ", name = " + phone->GetPhoneName()
                        + ", state = " + phone->GetState());
                call = phone->GetForegroundCall();
                b->Append("\n- Foreground: ").Append(call);
                call = phone->GetBackgroundCall();
                b->Append(" Background: ").Append(call);
                call = phone->GetRingingCall();
                b->Append(" Ringing: ").Append(call);
            }
        }
        b->Append("\n}");
        return b->ToString();
    }
}
