/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
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


using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDataProfile;

/**
 * {@hide}
 */
public abstract class BaseCommands implements CommandsInterface {
    //***** Instance Variables
    protected Context mContext;
    protected RadioState mState = RadioState.RADIO_UNAVAILABLE;
    protected Object mStateMonitor = new Object();

    protected RegistrantList mRadioStateChangedRegistrants = new RegistrantList();
    protected RegistrantList mOnRegistrants = new RegistrantList();
    protected RegistrantList mAvailRegistrants = new RegistrantList();
    protected RegistrantList mOffOrNotAvailRegistrants = new RegistrantList();
    protected RegistrantList mNotAvailRegistrants = new RegistrantList();
    protected RegistrantList mCallStateRegistrants = new RegistrantList();
    protected RegistrantList mVoiceNetworkStateRegistrants = new RegistrantList();
    protected RegistrantList mDataNetworkStateRegistrants = new RegistrantList();
    protected RegistrantList mVoiceRadioTechChangedRegistrants = new RegistrantList();
    protected RegistrantList mImsNetworkStateChangedRegistrants = new RegistrantList();
    protected RegistrantList mIccStatusChangedRegistrants = new RegistrantList();
    protected RegistrantList mVoicePrivacyOnRegistrants = new RegistrantList();
    protected RegistrantList mVoicePrivacyOffRegistrants = new RegistrantList();
    protected Registrant mUnsolOemHookRawRegistrant;
    protected RegistrantList mOtaProvisionRegistrants = new RegistrantList();
    protected RegistrantList mCallWaitingInfoRegistrants = new RegistrantList();
    protected RegistrantList mDisplayInfoRegistrants = new RegistrantList();
    protected RegistrantList mSignalInfoRegistrants = new RegistrantList();
    protected RegistrantList mNumberInfoRegistrants = new RegistrantList();
    protected RegistrantList mRedirNumInfoRegistrants = new RegistrantList();
    protected RegistrantList mLineControlInfoRegistrants = new RegistrantList();
    protected RegistrantList mT53ClirInfoRegistrants = new RegistrantList();
    protected RegistrantList mT53AudCntrlInfoRegistrants = new RegistrantList();
    protected RegistrantList mRingbackToneRegistrants = new RegistrantList();
    protected RegistrantList mResendIncallMuteRegistrants = new RegistrantList();
    protected RegistrantList mCdmaSubscriptionChangedRegistrants = new RegistrantList();
    protected RegistrantList mCdmaPrlChangedRegistrants = new RegistrantList();
    protected RegistrantList mExitEmergencyCallbackModeRegistrants = new RegistrantList();
    protected RegistrantList mRilConnectedRegistrants = new RegistrantList();
    protected RegistrantList mIccRefreshRegistrants = new RegistrantList();
    protected RegistrantList mRilCellInfoListRegistrants = new RegistrantList();
    protected RegistrantList mSubscriptionStatusRegistrants = new RegistrantList();
    protected RegistrantList mSrvccStateRegistrants = new RegistrantList();
    protected RegistrantList mHardwareConfigChangeRegistrants = new RegistrantList();
    protected RegistrantList mWwanIwlanCoexistenceRegistrants = new RegistrantList();
    protected RegistrantList mSimRefreshRegistrants = new RegistrantList();
    protected RegistrantList mModemCapRegistrants = new RegistrantList();

    protected Registrant mGsmSmsRegistrant;
    protected Registrant mCdmaSmsRegistrant;
    protected Registrant mNITZTimeRegistrant;
    protected Registrant mSignalStrengthRegistrant;
    protected Registrant mUSSDRegistrant;
    protected Registrant mSmsOnSimRegistrant;
    protected Registrant mSmsStatusRegistrant;
    protected Registrant mSsnRegistrant;
    protected Registrant mCatSessionEndRegistrant;
    protected Registrant mCatProCmdRegistrant;
    protected Registrant mCatEventRegistrant;
    protected Registrant mCatCallSetUpRegistrant;
    protected Registrant mCatSendSmsResultRegistrant;
    protected Registrant mIccSmsFullRegistrant;
    protected Registrant mEmergencyCallbackModeRegistrant;
    protected Registrant mRingRegistrant;
    protected Registrant mRestrictedStateRegistrant;
    protected Registrant mGsmBroadcastSmsRegistrant;
    protected Registrant mCatCcAlphaRegistrant;
    protected Registrant mSsRegistrant;

    // Preferred network type received from PhoneFactory.
    // This is used when establishing a connection to the
    // vendor ril so it starts up in the correct mode.
    protected Int32 mPreferredNetworkType;
    // CDMA subscription received from PhoneFactory
    protected Int32 mCdmaSubscription;
    // Type of Phone, GSM or CDMA. Set by CDMAPhone or GSMPhone.
    protected Int32 mPhoneType;
    // RIL Version
    protected Int32 mRilVersion = -1;

    public BaseCommands(Context context) {
        mContext = context;  // May be NULL (if so we won't log statistics)
    }

    //***** CommandsInterface implementation

    //@Override
    public RadioState GetRadioState() {
        return mState;
    }

    //@Override
    CARAPI RegisterForRadioStateChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        Synchronized (mStateMonitor) {
            mRadioStateChangedRegistrants->Add(r);
            r->NotifyRegistrant();
        }
    }

    //@Override
    CARAPI UnregisterForRadioStateChanged(Handler h) {
        Synchronized (mStateMonitor) {
            mRadioStateChangedRegistrants->Remove(h);
        }
    }

    CARAPI RegisterForImsNetworkStateChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mImsNetworkStateChangedRegistrants->Add(r);
    }

    CARAPI UnregisterForImsNetworkStateChanged(Handler h) {
        mImsNetworkStateChangedRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForOn(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        Synchronized (mStateMonitor) {
            mOnRegistrants->Add(r);

            If (mState->IsOn()) {
                r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
            }
        }
    }
    //@Override
    CARAPI UnregisterForOn(Handler h) {
        Synchronized (mStateMonitor) {
            mOnRegistrants->Remove(h);
        }
    }


    //@Override
    CARAPI RegisterForAvailable(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        Synchronized (mStateMonitor) {
            mAvailRegistrants->Add(r);

            If (mState->IsAvailable()) {
                r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
            }
        }
    }

    //@Override
    CARAPI UnregisterForAvailable(Handler h) {
        Synchronized(mStateMonitor) {
            mAvailRegistrants->Remove(h);
        }
    }

    //@Override
    CARAPI RegisterForNotAvailable(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        Synchronized (mStateMonitor) {
            mNotAvailRegistrants->Add(r);

            If (!mState->IsAvailable()) {
                r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
            }
        }
    }

    //@Override
    CARAPI UnregisterForNotAvailable(Handler h) {
        Synchronized (mStateMonitor) {
            mNotAvailRegistrants->Remove(h);
        }
    }

    //@Override
    CARAPI RegisterForOffOrNotAvailable(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        Synchronized (mStateMonitor) {
            mOffOrNotAvailRegistrants->Add(r);

            If (mState == RadioState.RADIO_OFF || !mState->IsAvailable()) {
                r->NotifyRegistrant(new AsyncResult(NULL, NULL, NULL));
            }
        }
    }
    //@Override
    CARAPI UnregisterForOffOrNotAvailable(Handler h) {
        Synchronized(mStateMonitor) {
            mOffOrNotAvailRegistrants->Remove(h);
        }
    }

    //@Override
    CARAPI RegisterForCallStateChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mCallStateRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForCallStateChanged(Handler h) {
        mCallStateRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForVoiceNetworkStateChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mVoiceNetworkStateRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForVoiceNetworkStateChanged(Handler h) {
        mVoiceNetworkStateRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForDataNetworkStateChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mDataNetworkStateRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForDataNetworkStateChanged(Handler h) {
        mDataNetworkStateRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForVoiceRadioTechChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mVoiceRadioTechChangedRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForVoiceRadioTechChanged(Handler h) {
        mVoiceRadioTechChangedRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForIccStatusChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mIccStatusChangedRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForIccStatusChanged(Handler h) {
        mIccStatusChangedRegistrants->Remove(h);
    }

    //@Override
    CARAPI SetOnNewGsmSms(Handler h, Int32 what, Object obj) {
        mGsmSmsRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnNewGsmSms(Handler h) {
        If (mGsmSmsRegistrant != NULL && mGsmSmsRegistrant->GetHandler() == h) {
            mGsmSmsRegistrant->Clear();
            mGsmSmsRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnNewCdmaSms(Handler h, Int32 what, Object obj) {
        mCdmaSmsRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnNewCdmaSms(Handler h) {
        If (mCdmaSmsRegistrant != NULL && mCdmaSmsRegistrant->GetHandler() == h) {
            mCdmaSmsRegistrant->Clear();
            mCdmaSmsRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnNewGsmBroadcastSms(Handler h, Int32 what, Object obj) {
        mGsmBroadcastSmsRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnNewGsmBroadcastSms(Handler h) {
        If (mGsmBroadcastSmsRegistrant != NULL && mGsmBroadcastSmsRegistrant->GetHandler() == h) {
            mGsmBroadcastSmsRegistrant->Clear();
            mGsmBroadcastSmsRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnSmsOnSim(Handler h, Int32 what, Object obj) {
        mSmsOnSimRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnSmsOnSim(Handler h) {
        If (mSmsOnSimRegistrant != NULL && mSmsOnSimRegistrant->GetHandler() == h) {
            mSmsOnSimRegistrant->Clear();
            mSmsOnSimRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnSmsStatus(Handler h, Int32 what, Object obj) {
        mSmsStatusRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnSmsStatus(Handler h) {
        If (mSmsStatusRegistrant != NULL && mSmsStatusRegistrant->GetHandler() == h) {
            mSmsStatusRegistrant->Clear();
            mSmsStatusRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnSignalStrengthUpdate(Handler h, Int32 what, Object obj) {
        mSignalStrengthRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnSignalStrengthUpdate(Handler h) {
        If (mSignalStrengthRegistrant != NULL && mSignalStrengthRegistrant->GetHandler() == h) {
            mSignalStrengthRegistrant->Clear();
            mSignalStrengthRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnNITZTime(Handler h, Int32 what, Object obj) {
        mNITZTimeRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnNITZTime(Handler h) {
        If (mNITZTimeRegistrant != NULL && mNITZTimeRegistrant->GetHandler() == h) {
            mNITZTimeRegistrant->Clear();
            mNITZTimeRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnUSSD(Handler h, Int32 what, Object obj) {
        mUSSDRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnUSSD(Handler h) {
        If (mUSSDRegistrant != NULL && mUSSDRegistrant->GetHandler() == h) {
            mUSSDRegistrant->Clear();
            mUSSDRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnSuppServiceNotification(Handler h, Int32 what, Object obj) {
        mSsnRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnSuppServiceNotification(Handler h) {
        If (mSsnRegistrant != NULL && mSsnRegistrant->GetHandler() == h) {
            mSsnRegistrant->Clear();
            mSsnRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnCatSessionEnd(Handler h, Int32 what, Object obj) {
        mCatSessionEndRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnCatSessionEnd(Handler h) {
        If (mCatSessionEndRegistrant != NULL && mCatSessionEndRegistrant->GetHandler() == h) {
            mCatSessionEndRegistrant->Clear();
            mCatSessionEndRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnCatProactiveCmd(Handler h, Int32 what, Object obj) {
        mCatProCmdRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnCatProactiveCmd(Handler h) {
        If (mCatProCmdRegistrant != NULL && mCatProCmdRegistrant->GetHandler() == h) {
            mCatProCmdRegistrant->Clear();
            mCatProCmdRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnCatEvent(Handler h, Int32 what, Object obj) {
        mCatEventRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnCatEvent(Handler h) {
        If (mCatEventRegistrant != NULL && mCatEventRegistrant->GetHandler() == h) {
            mCatEventRegistrant->Clear();
            mCatEventRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnCatCallSetUp(Handler h, Int32 what, Object obj) {
        mCatCallSetUpRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnCatCallSetUp(Handler h) {
        If (mCatCallSetUpRegistrant != NULL && mCatCallSetUpRegistrant->GetHandler() == h) {
            mCatCallSetUpRegistrant->Clear();
            mCatCallSetUpRegistrant = NULL;
        }
    }

    // For Samsung STK
    CARAPI SetOnCatSendSmsResult(Handler h, Int32 what, Object obj) {
        mCatSendSmsResultRegistrant = new Registrant(h, what, obj);
    }

    CARAPI UnSetOnCatSendSmsResult(Handler h) {
        mCatSendSmsResultRegistrant->Clear();
    }

    //@Override
    CARAPI SetOnIccSmsFull(Handler h, Int32 what, Object obj) {
        mIccSmsFullRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnIccSmsFull(Handler h) {
        If (mIccSmsFullRegistrant != NULL && mIccSmsFullRegistrant->GetHandler() == h) {
            mIccSmsFullRegistrant->Clear();
            mIccSmsFullRegistrant = NULL;
        }
    }

    //@Override
    CARAPI RegisterForIccRefresh(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mIccRefreshRegistrants->Add(r);
    }
    //@Override
    CARAPI SetOnIccRefresh(Handler h, Int32 what, Object obj) {
        RegisterForIccRefresh(h, what, obj);
    }

    //@Override
    CARAPI SetEmergencyCallbackMode(Handler h, Int32 what, Object obj) {
        mEmergencyCallbackModeRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnregisterForIccRefresh(Handler h) {
        mIccRefreshRegistrants->Remove(h);
    }
    //@Override
    CARAPI UnsetOnIccRefresh(Handler h) {
        UnregisterForIccRefresh(h);
    }

    //@Override
    CARAPI SetOnCallRing(Handler h, Int32 what, Object obj) {
        mRingRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnCallRing(Handler h) {
        If (mRingRegistrant != NULL && mRingRegistrant->GetHandler() == h) {
            mRingRegistrant->Clear();
            mRingRegistrant = NULL;
        }
    }

    //@Override
    CARAPI SetOnSs(Handler h, Int32 what, Object obj) {
        mSsRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnSs(Handler h) {
        mSsRegistrant->Clear();
    }

    //@Override
    CARAPI SetOnCatCcAlphaNotify(Handler h, Int32 what, Object obj) {
        mCatCcAlphaRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnCatCcAlphaNotify(Handler h) {
        mCatCcAlphaRegistrant->Clear();
    }

    //@Override
    CARAPI RegisterForInCallVoicePrivacyOn(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mVoicePrivacyOnRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForInCallVoicePrivacyOn(Handler h){
        mVoicePrivacyOnRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForInCallVoicePrivacyOff(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mVoicePrivacyOffRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForInCallVoicePrivacyOff(Handler h){
        mVoicePrivacyOffRegistrants->Remove(h);
    }

    //@Override
    CARAPI SetOnRestrictedStateChanged(Handler h, Int32 what, Object obj) {
        mRestrictedStateRegistrant = new Registrant (h, what, obj);
    }

    //@Override
    CARAPI UnSetOnRestrictedStateChanged(Handler h) {
        If (mRestrictedStateRegistrant != NULL && mRestrictedStateRegistrant->GetHandler() != h) {
            mRestrictedStateRegistrant->Clear();
            mRestrictedStateRegistrant = NULL;
        }
    }

    //@Override
    CARAPI RegisterForDisplayInfo(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mDisplayInfoRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForDisplayInfo(Handler h) {
        mDisplayInfoRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForCallWaitingInfo(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mCallWaitingInfoRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForCallWaitingInfo(Handler h) {
        mCallWaitingInfoRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForSignalInfo(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mSignalInfoRegistrants->Add(r);
    }

    CARAPI SetOnUnsolOemHookRaw(Handler h, Int32 what, Object obj) {
        mUnsolOemHookRawRegistrant = new Registrant (h, what, obj);
    }

    CARAPI UnSetOnUnsolOemHookRaw(Handler h) {
        If (mUnsolOemHookRawRegistrant != NULL && mUnsolOemHookRawRegistrant->GetHandler() == h) {
            mUnsolOemHookRawRegistrant->Clear();
            mUnsolOemHookRawRegistrant = NULL;
        }
    }

    //@Override
    CARAPI UnregisterForSignalInfo(Handler h) {
        mSignalInfoRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForCdmaOtaProvision(Handler h,Int32 what, Object obj){
        Registrant r = new Registrant (h, what, obj);
        mOtaProvisionRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForCdmaOtaProvision(Handler h){
        mOtaProvisionRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForNumberInfo(Handler h,Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mNumberInfoRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForNumberInfo(Handler h){
        mNumberInfoRegistrants->Remove(h);
    }

     //@Override
    CARAPI RegisterForRedirectedNumberInfo(Handler h,Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mRedirNumInfoRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForRedirectedNumberInfo(Handler h) {
        mRedirNumInfoRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForLineControlInfo(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mLineControlInfoRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForLineControlInfo(Handler h) {
        mLineControlInfoRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterFoT53ClirlInfo(Handler h,Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mT53ClirInfoRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForT53ClirInfo(Handler h) {
        mT53ClirInfoRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForT53AudioControlInfo(Handler h,Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mT53AudCntrlInfoRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForT53AudioControlInfo(Handler h) {
        mT53AudCntrlInfoRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForRingbackTone(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mRingbackToneRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForRingbackTone(Handler h) {
        mRingbackToneRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForResendIncallMute(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mResendIncallMuteRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForResendIncallMute(Handler h) {
        mResendIncallMuteRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForCdmaSubscriptionChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mCdmaSubscriptionChangedRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForCdmaSubscriptionChanged(Handler h) {
        mCdmaSubscriptionChangedRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForCdmaPrlChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mCdmaPrlChangedRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForCdmaPrlChanged(Handler h) {
        mCdmaPrlChangedRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForExitEmergencyCallbackMode(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mExitEmergencyCallbackModeRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForExitEmergencyCallbackMode(Handler h) {
        mExitEmergencyCallbackModeRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForHardwareConfigChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mHardwareConfigChangeRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForHardwareConfigChanged(Handler h) {
        mHardwareConfigChangeRegistrants->Remove(h);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI RegisterForRilConnected(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mRilConnectedRegistrants->Add(r);
        If (mRilVersion != -1) {
            r->NotifyRegistrant(new AsyncResult(NULL, new Integer(mRilVersion), NULL));
        }
    }

    //@Override
    CARAPI UnregisterForRilConnected(Handler h) {
        mRilConnectedRegistrants->Remove(h);
    }

    CARAPI RegisterForSubscriptionStatusChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mSubscriptionStatusRegistrants->Add(r);
    }

    CARAPI UnregisterForSubscriptionStatusChanged(Handler h) {
        mSubscriptionStatusRegistrants->Remove(h);
    }

    CARAPI RegisterForWwanIwlanCoexistence(Handler h, Int32 what, Object obj) {
        mWwanIwlanCoexistenceRegistrants->AddUnique(h, what, obj);
    }

    CARAPI UnregisterForWwanIwlanCoexistence(Handler h) {
        mWwanIwlanCoexistenceRegistrants->Remove(h);
    }

    CARAPI RegisterForSimRefreshEvent(Handler h, Int32 what, Object obj) {
        mSimRefreshRegistrants->AddUnique(h, what, obj);
    }

    CARAPI UnregisterForSimRefreshEvent(Handler h) {
        mSimRefreshRegistrants->Remove(h);
    }

    CARAPI RegisterForModemCapEvent(Handler h, Int32 what, Object obj) {
        mModemCapRegistrants->AddUnique(h, what, obj);
    }

    CARAPI UnregisterForModemCapEvent(Handler h) {
        mModemCapRegistrants->Remove(h);
    }

    //@Override
    CARAPI GetDataCallProfile(Int32 appType, Message result) {
    }

    //***** Protected Methods
    /**
     * Store new RadioState and send notification based on the changes
     *
     * This function is called only by RIL.java when receiving unsolicited
     * RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED
     *
     * RadioState has 3 values : RADIO_OFF, RADIO_UNAVAILABLE, RADIO_ON.
     *
     * @param newState new RadioState decoded from RIL_UNSOL_RADIO_STATE_CHANGED
     */
    protected void SetRadioState(RadioState newState) {
        RadioState oldState;

        Synchronized (mStateMonitor) {
            oldState = mState;
            mState = newState;

            If (oldState == mState) {
                // no state transition
                return;
            }

            mRadioStateChangedRegistrants->NotifyRegistrants();

            If (mState->IsAvailable() && !oldState->IsAvailable()) {
                mAvailRegistrants->NotifyRegistrants();
                OnRadioAvailable();
            }

            If (!mState->IsAvailable() && oldState->IsAvailable()) {
                mNotAvailRegistrants->NotifyRegistrants();
            }

            If (mState->IsOn() && !oldState->IsOn()) {
                mOnRegistrants->NotifyRegistrants();
            }

            If ((!mState->IsOn() || !mState->IsAvailable())
                && !((!oldState->IsOn() || !oldState->IsAvailable()))
            ) {
                mOffOrNotAvailRegistrants->NotifyRegistrants();
            }
        }
    }

    CARAPI SendSMSExpectMore (String smscPDU, String pdu, Message result) {
    }

    protected void OnRadioAvailable() {
    }

    CARAPI GetModemCapability(Message response) {
    }

    CARAPI UpdateStackBinding(Int32 stackId, Int32 enable, Message response) {
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    public Int32 GetLteOnCdmaMode() {
        return TelephonyManager->GetLteOnCdmaModeStatic();
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI RegisterForCellInfoList(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        mRilCellInfoListRegistrants->Add(r);
    }
    //@Override
    CARAPI UnregisterForCellInfoList(Handler h) {
        mRilCellInfoListRegistrants->Remove(h);
    }

    //@Override
    CARAPI RegisterForSrvccStateChanged(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);

        mSrvccStateRegistrants->Add(r);
    }

    //@Override
    CARAPI UnregisterForSrvccStateChanged(Handler h) {
        mSrvccStateRegistrants->Remove(h);
    }

    //@Override
    CARAPI TestingEmergencyCall() {}

    //@Override
    public Int32 GetRilVersion() {
        return mRilVersion;
    }

    CARAPI SetUiccSubscription(Int32 slotId, Int32 appIndex, Int32 subId, Int32 subStatus,
            Message response) {
    }

    CARAPI SetDataProfile(DataProfile[] dps, Message result) {
    }

    CARAPI SetDataAllowed(Boolean allowed, Message response) {
    }

    //@Override
    CARAPI RequestShutdown(Message result) {
    }

    //@Override
    CARAPI IccOpenLogicalChannel(String AID, Message response) {}

    //@Override
    CARAPI IccCloseLogicalChannel(Int32 channel, Message response) {}

    //@Override
    CARAPI IccTransmitApduLogicalChannel(Int32 channel, Int32 cla, Int32 instruction,
                                              Int32 p1, Int32 p2, Int32 p3, String data,
                                              Message response) {}
    //@Override
    CARAPI IccTransmitApduBasicChannel(Int32 cla, Int32 instruction, Int32 p1, Int32 p2,
                                            Int32 p3, String data, Message response) {}

    //@Override
    CARAPI GetAtr(Message response) {}

    CARAPI SetLocalCallHold(Int32 lchStatus) {
    }

    /**
     * @hide
     */
    //@Override
    public Int32 GetLteOnGsmMode() {
        return TelephonyManager->GetLteOnGsmModeStatic();
    }

}
