
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/imsphone/ImsPhoneCallTracker.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace ImsPhone {

//=====================================================================
//             ImsPhoneCallTracker::InnerBroadcastReceiver
//=====================================================================
ImsPhoneCallTracker::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ ImsPhoneCallTracker* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ImsPhoneCallTracker::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // ==================before translated======================
    // if (intent.getAction().equals(ImsManager.ACTION_IMS_INCOMING_CALL)) {
    //     if (DBG) log("onReceive : incoming call intent");
    //
    //     if (mImsManager == null) return;
    //
    //     if (mServiceId < 0) return;
    //
    //     try {
    //         // Network initiated USSD will be treated by mImsUssdListener
    //         boolean isUssd = intent.getBooleanExtra(ImsManager.EXTRA_USSD, false);
    //         if (isUssd) {
    //             if (DBG) log("onReceive : USSD");
    //             mUssdSession = mImsManager.takeCall(mServiceId, intent, mImsUssdListener);
    //             if (mUssdSession != null) {
    //                 mUssdSession.accept(ImsCallProfile.CALL_TYPE_VOICE);
    //             }
    //             return;
    //         }
    //
    //         // Normal MT call
    //         ImsCall imsCall = mImsManager.takeCall(mServiceId, intent, mImsCallListener);
    //         ImsPhoneConnection conn = new ImsPhoneConnection(mPhone.getContext(), imsCall,
    //                 ImsPhoneCallTracker.this, mRingingCall);
    //         addConnection(conn);
    //
    //         IImsVideoCallProvider imsVideoCallProvider =
    //                 imsCall.getCallSession().getVideoCallProvider();
    //         if (imsVideoCallProvider != null) {
    //             ImsVideoCallProviderWrapper imsVideoCallProviderWrapper =
    //                     new ImsVideoCallProviderWrapper(imsVideoCallProvider);
    //             conn.setVideoProvider(imsVideoCallProviderWrapper);
    //         }
    //
    //         if ((mForegroundCall.getState() != ImsPhoneCall.State.IDLE) ||
    //                 (mBackgroundCall.getState() != ImsPhoneCall.State.IDLE)) {
    //             conn.update(imsCall, ImsPhoneCall.State.WAITING);
    //         }
    //
    //         mPhone.notifyNewRingingConnection(conn);
    //         mPhone.notifyIncomingRing();
    //
    //         updatePhoneState();
    //         mPhone.notifyPreciseCallStateChanged();
    //     } catch (ImsException e) {
    //         loge("onReceive : exception " + e);
    //     } catch (RemoteException e) {
    //     }
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//              ImsPhoneCallTracker::InnerImsCallListener
//=====================================================================
ImsPhoneCallTracker::InnerImsCallListener::InnerImsCallListener(
    /* [in] */ ImsPhoneCallTracker* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallProgressing(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if (DBG) log("onCallProgressing");
    //
    // mPendingMO = null;
    // processCallStateChange(imsCall, ImsPhoneCall.State.ALERTING,
    //         DisconnectCause.NOT_DISCONNECTED);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallStarted(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if (DBG) log("onCallStarted");
    //
    // mPendingMO = null;
    // processCallStateChange(imsCall, ImsPhoneCall.State.ACTIVE,
    //         DisconnectCause.NOT_DISCONNECTED);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallUpdated(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if (DBG) log("onCallUpdated");
    // if (imsCall == null) {
    //     return;
    // }
    // ImsPhoneConnection conn = findConnection(imsCall);
    // if (conn != null) {
    //     processCallStateChange(imsCall, conn.getCall().mState,
    //             DisconnectCause.NOT_DISCONNECTED);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallStartFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // if (DBG) log("onCallStartFailed reasonCode=" + reasonInfo.getCode());
    //
    // if (mPendingMO != null) {
    //     // To initiate dialing circuit-switched call
    //     if (reasonInfo.getCode() == ImsReasonInfo.CODE_LOCAL_CALL_CS_RETRY_REQUIRED
    //             && mBackgroundCall.getState() == ImsPhoneCall.State.IDLE
    //             && mRingingCall.getState() == ImsPhoneCall.State.IDLE) {
    //         mForegroundCall.detach(mPendingMO);
    //         removeConnection(mPendingMO);
    //         mPendingMO.finalize();
    //         mPendingMO = null;
    //         mPhone.initiateSilentRedial();
    //         return;
    //     }
    //     mPendingMO.setDisconnectCause(DisconnectCause.ERROR_UNSPECIFIED);
    //     sendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallTerminated(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // if (DBG) log("onCallTerminated reasonCode=" + reasonInfo.getCode());
    //
    // ImsPhoneCall.State oldState = mForegroundCall.getState();
    // int cause = getDisconnectCauseFromReasonInfo(reasonInfo);
    // ImsPhoneConnection conn = findConnection(imsCall);
    // if (DBG) log("cause = " + cause + " conn = " + conn);
    //
    // if (conn != null && conn.isIncoming() && conn.getConnectTime() == 0) {
    //     // Missed or rejected call
    //     if (cause == DisconnectCause.LOCAL) {
    //         cause = DisconnectCause.INCOMING_REJECTED;
    //     } else {
    //         cause = DisconnectCause.INCOMING_MISSED;
    //     }
    // }
    //
    // if (cause == DisconnectCause.NORMAL && conn != null && conn.getImsCall().isMerged()) {
    //     // Call was terminated while it is merged instead of a remote disconnect.
    //     cause = DisconnectCause.IMS_MERGED_SUCCESSFULLY;
    // }
    //
    // processCallStateChange(imsCall, ImsPhoneCall.State.DISCONNECTED, cause);
    //
    // if (reasonInfo.getCode() == ImsReasonInfo.CODE_USER_TERMINATED) {
    //     if ((oldState == ImsPhoneCall.State.DISCONNECTING)
    //             && (mForegroundCall.getState() == ImsPhoneCall.State.DISCONNECTED)
    //             && (mBackgroundCall.getState() == ImsPhoneCall.State.HOLDING)) {
    //         sendEmptyMessage(EVENT_RESUME_BACKGROUND);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallHeld(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if (DBG) log("onCallHeld");
    //
    // synchronized (mSyncHold) {
    //     ImsPhoneCall.State oldState = mBackgroundCall.getState();
    //     processCallStateChange(imsCall, ImsPhoneCall.State.HOLDING,
    //             DisconnectCause.NOT_DISCONNECTED);
    //     if (oldState == ImsPhoneCall.State.ACTIVE) {
    //         if ((mForegroundCall.getState() == ImsPhoneCall.State.HOLDING)
    //                 || (mRingingCall.getState() == ImsPhoneCall.State.WAITING)) {
    //             boolean isOwner = true;
    //             CallGroup callGroup =  imsCall.getCallGroup();
    //             if (callGroup != null) {
    //                 isOwner = callGroup.isOwner(imsCall);
    //             }
    //             if (isOwner) {
    //                 sendEmptyMessage(EVENT_RESUME_BACKGROUND);
    //             }
    //         } else {
    //             //when multiple connections belong to background call,
    //             //only the first callback reaches here
    //             //otherwise the oldState is already HOLDING
    //             if (mPendingMO != null) {
    //                 sendEmptyMessage(EVENT_DIAL_PENDINGMO);
    //             }
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallHoldFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // if (DBG) log("onCallHoldFailed reasonCode=" + reasonInfo.getCode());
    //
    // synchronized (mSyncHold) {
    //     ImsPhoneCall.State bgState = mBackgroundCall.getState();
    //     if (reasonInfo.getCode() == ImsReasonInfo.CODE_LOCAL_CALL_TERMINATED) {
    //         // disconnected while processing hold
    //         if (mPendingMO != null) {
    //             sendEmptyMessage(EVENT_DIAL_PENDINGMO);
    //         }
    //     } else if (bgState == ImsPhoneCall.State.ACTIVE) {
    //         mForegroundCall.switchWith(mBackgroundCall);
    //
    //         if (mPendingMO != null) {
    //             mPendingMO.setDisconnectCause(DisconnectCause.ERROR_UNSPECIFIED);
    //             sendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallResumed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if (DBG) log("onCallResumed");
    //
    // processCallStateChange(imsCall, ImsPhoneCall.State.ACTIVE,
    //         DisconnectCause.NOT_DISCONNECTED);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallResumeFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // // TODO : What should be done?
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallResumeReceived(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if (DBG) log("onCallResumeReceived");
    //
    // if (mOnHoldToneStarted) {
    //     mPhone.stopOnHoldTone();
    //     mOnHoldToneStarted = false;
    // }
    //
    // SuppServiceNotification supp = new SuppServiceNotification();
    // // Type of notification: 0 = MO; 1 = MT
    // // Refer SuppServiceNotification class documentation.
    // supp.notificationType = 1;
    // supp.code = SuppServiceNotification.MT_CODE_CALL_RETRIEVED;
    // mPhone.notifySuppSvcNotification(supp);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallHoldReceived(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if (DBG) log("onCallHoldReceived");
    //
    // ImsPhoneConnection conn = findConnection(imsCall);
    // if (conn != null && conn.getState() == ImsPhoneCall.State.ACTIVE) {
    //     if (!mOnHoldToneStarted && ImsPhoneCall.isLocalTone(imsCall)) {
    //         mPhone.startOnHoldTone();
    //         mOnHoldToneStarted = true;
    //     }
    // }
    //
    // SuppServiceNotification supp = new SuppServiceNotification();
    // // Type of notification: 0 = MO; 1 = MT
    // // Refer SuppServiceNotification class documentation.
    // supp.notificationType = 1;
    // supp.code = SuppServiceNotification.MT_CODE_CALL_ON_HOLD;
    // mPhone.notifySuppSvcNotification(supp);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallMerged(
    /* [in] */ /*TODO IImsCall*/IInterface* call)
{
    // ==================before translated======================
    // if (DBG) log("onCallMerged");
    //
    // mForegroundCall.merge(mBackgroundCall, mForegroundCall.getState());
    // updatePhoneState();
    // mPhone.notifyPreciseCallStateChanged();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnCallMergeFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* call,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // if (DBG) log("onCallMergeFailed reasonCode=" + reasonInfo.getCode());
    // mPhone.notifySuppServiceFailed(Phone.SuppService.CONFERENCE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener::OnConferenceParticipantsStateChanged(
    /* [in] */ /*TODO IImsCall*/IInterface* call,
    /* [in] */ IList/*<IConferenceParticipant>*/* participants)
{
    // ==================before translated======================
    // if (DBG) log("onConferenceParticipantsStateChanged");
    //
    // ImsPhoneConnection conn = findConnection(call);
    // if (conn != null) {
    //     conn.updateConferenceParticipants(participants);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//              ImsPhoneCallTracker::InnerImsCallListener1
//=====================================================================
ImsPhoneCallTracker::InnerImsCallListener1::InnerImsCallListener1(
    /* [in] */ ImsPhoneCallTracker* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ImsPhoneCallTracker::InnerImsCallListener1::OnCallStarted(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // if (DBG) log("mImsUssdListener onCallStarted");
    //
    // if (imsCall == mUssdSession) {
    //     if (mPendingUssd != null) {
    //         AsyncResult.forMessage(mPendingUssd);
    //         mPendingUssd.sendToTarget();
    //         mPendingUssd = null;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener1::OnCallStartFailed(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // if (DBG) log("mImsUssdListener onCallStartFailed reasonCode=" + reasonInfo.getCode());
    //
    // onCallTerminated(imsCall, reasonInfo);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener1::OnCallTerminated(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // if (DBG) log("mImsUssdListener onCallTerminated reasonCode=" + reasonInfo.getCode());
    //
    // if (imsCall == mUssdSession) {
    //     mUssdSession = null;
    //     if (mPendingUssd != null) {
    //         CommandException ex =
    //                 new CommandException(CommandException.Error.GENERIC_FAILURE);
    //         AsyncResult.forMessage(mPendingUssd, null, ex);
    //         mPendingUssd.sendToTarget();
    //         mPendingUssd = null;
    //     }
    // }
    // imsCall.close();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsCallListener1::OnCallUssdMessageReceived(
    /* [in] */ /*TODO IImsCall*/IInterface* call,
    /* [in] */ Int32 mode,
    /* [in] */ const String& ussdMessage)
{
    // ==================before translated======================
    // if (DBG) log("mImsUssdListener onCallUssdMessageReceived mode=" + mode);
    //
    // int ussdMode = -1;
    //
    // switch(mode) {
    //     case ImsCall.USSD_MODE_REQUEST:
    //         ussdMode = CommandsInterface.USSD_MODE_REQUEST;
    //         break;
    //
    //     case ImsCall.USSD_MODE_NOTIFY:
    //         ussdMode = CommandsInterface.USSD_MODE_NOTIFY;
    //         break;
    // }
    //
    // mPhone.onIncomingUSSD(ussdMode, ussdMessage);
    assert(0);
    return NOERROR;
}

//=====================================================================
//         ImsPhoneCallTracker::InnerImsConnectionStateListener
//=====================================================================
ImsPhoneCallTracker::InnerImsConnectionStateListener::InnerImsConnectionStateListener(
    /* [in] */ ImsPhoneCallTracker* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnImsConnected()
{
    // ==================before translated======================
    // if (DBG) log("onImsConnected");
    // mPhone.setServiceState(ServiceState.STATE_IN_SERVICE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnImsDisconnected()
{
    // ==================before translated======================
    // if (DBG) log("onImsDisconnected");
    // mPhone.setServiceState(ServiceState.STATE_OUT_OF_SERVICE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnImsResumed()
{
    // ==================before translated======================
    // if (DBG) log("onImsResumed");
    // mPhone.setServiceState(ServiceState.STATE_IN_SERVICE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnImsSuspended()
{
    // ==================before translated======================
    // if (DBG) log("onImsSuspended");
    // mPhone.setServiceState(ServiceState.STATE_OUT_OF_SERVICE);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnFeatureCapabilityChanged(
    /* [in] */ Int32 serviceClass,
    /* [in] */ ArrayOf<Int32>* enabledFeatures,
    /* [in] */ ArrayOf<Int32>* disabledFeatures)
{
    // ==================before translated======================
    // if (serviceClass == ImsServiceClass.MMTEL) {
    //     boolean tmpIsVtEnabled = mIsVtEnabled;
    //
    //     if (disabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE] ==
    //             ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE ||
    //             disabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI] ==
    //             ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI) {
    //         mIsVolteEnabled = false;
    //     }
    //     if (disabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE] ==
    //             ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE ||
    //             disabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI] ==
    //             ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI) {
    //         mIsVtEnabled = false;
    //     }
    //     if (enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE] ==
    //             ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_LTE ||
    //             enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI] ==
    //             ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI) {
    //         mIsVolteEnabled = true;
    //     }
    //     if (enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE] ==
    //             ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_LTE ||
    //             enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI] ==
    //             ImsConfig.FeatureConstants.FEATURE_TYPE_VIDEO_OVER_WIFI) {
    //         mIsVtEnabled = true;
    //     }
    //
    //     if (tmpIsVtEnabled != mIsVtEnabled) {
    //         mPhone.notifyForVideoCapabilityChanged(mIsVtEnabled);
    //     }
    // }
    //
    // if (DBG) log("onFeatureCapabilityChanged, mIsVolteEnabled = " +  mIsVolteEnabled
    //         + " mIsVtEnabled = " + mIsVtEnabled);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::InnerImsConnectionStateListener::OnVoiceMessageCountChanged(
    /* [in] */ Int32 count)
{
    // ==================before translated======================
    // if (DBG) log("onVoiceMessageCountChanged :: count=" + count);
    // mPhone.mDefaultPhone.setVoiceMessageCount(count);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                         ImsPhoneCallTracker
//=====================================================================
CAR_INTERFACE_IMPL(ImsPhoneCallTracker, CallTracker, IImsPhoneCallTracker);

const String ImsPhoneCallTracker::LOGTAG("ImsPhoneCallTracker");
const Int32 ImsPhoneCallTracker::MAX_CONNECTIONS;
const Int32 ImsPhoneCallTracker::MAX_CONNECTIONS_PER_CALL;
const Boolean ImsPhoneCallTracker::DBG = TRUE;
const Int32 ImsPhoneCallTracker::EVENT_HANGUP_PENDINGMO;
const Int32 ImsPhoneCallTracker::EVENT_RESUME_BACKGROUND;
const Int32 ImsPhoneCallTracker::EVENT_DIAL_PENDINGMO;
const Int32 ImsPhoneCallTracker::TIMEOUT_HANGUP_PENDINGMO;

ECode ImsPhoneCallTracker::constructor(
    /* [in] */ IImsPhone* phone)
{
    // ==================before translated======================
    // this.mPhone = phone;
    //
    // IntentFilter intentfilter = new IntentFilter();
    // intentfilter.addAction(ImsManager.ACTION_IMS_INCOMING_CALL);
    // mPhone.getContext().registerReceiver(mReceiver, intentfilter);
    //
    // Thread t = new Thread() {
    //     public void run() {
    //         getImsService();
    //     }
    // };
    // t.start();
    return NOERROR;
}

ECode ImsPhoneCallTracker::Dispose()
{
    // ==================before translated======================
    // if (DBG) log("dispose");
    // mRingingCall.dispose();
    // mBackgroundCall.dispose();
    // mForegroundCall.dispose();
    // mHandoverCall.dispose();
    //
    // clearDisconnected();
    // mPhone.getContext().unregisterReceiver(mReceiver);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::RegisterForVoiceCallStarted(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant(h, what, obj);
    // mVoiceCallStartedRegistrants.add(r);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::UnregisterForVoiceCallStarted(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mVoiceCallStartedRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::RegisterForVoiceCallEnded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // Registrant r = new Registrant(h, what, obj);
    // mVoiceCallEndedRegistrants.add(r);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::UnregisterForVoiceCallEnded(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mVoiceCallEndedRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return dial(dialString, videoState, null);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mPhone.getContext());
    // int oirMode = sp.getInt(PhoneBase.CLIR_KEY, CommandsInterface.CLIR_DEFAULT);
    // return dial(dialString, oirMode, videoState, extras);
    assert(0);
    return NOERROR;
}

// synchronized
ECode ImsPhoneCallTracker::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 clirMode,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean isPhoneInEcmMode = SystemProperties.getBoolean(
    //         TelephonyProperties.PROPERTY_INECM_MODE, false);
    // boolean isEmergencyNumber = PhoneNumberUtils.isEmergencyNumber(dialString);
    //
    // if (DBG) log("dial clirMode=" + clirMode);
    //
    // // note that this triggers call state changed notif
    // clearDisconnected();
    //
    // if (mImsManager == null) {
    //     throw new CallStateException("service not available");
    // }
    //
    // if (!canDial()) {
    //     throw new CallStateException("cannot dial in current state");
    // }
    //
    // if (isPhoneInEcmMode && isEmergencyNumber) {
    //     handleEcmTimer(ImsPhone.CANCEL_ECM_TIMER);
    // }
    //
    // boolean holdBeforeDial = false;
    //
    // // The new call must be assigned to the foreground call.
    // // That call must be idle, so place anything that's
    // // there on hold
    // if (mForegroundCall.getState() == ImsPhoneCall.State.ACTIVE) {
    //     if (mBackgroundCall.getState() != ImsPhoneCall.State.IDLE) {
    //         //we should have failed in !canDial() above before we get here
    //         throw new CallStateException("cannot dial in current state");
    //     }
    //     // foreground call is empty for the newly dialed connection
    //     holdBeforeDial = true;
    //     // Cache the video state for pending MO call.
    //     mPendingCallVideoState = videoState;
    //     switchWaitingOrHoldingAndActive();
    // }
    //
    // ImsPhoneCall.State fgState = ImsPhoneCall.State.IDLE;
    // ImsPhoneCall.State bgState = ImsPhoneCall.State.IDLE;
    //
    // mClirMode = clirMode;
    //
    // synchronized (mSyncHold) {
    //     if (holdBeforeDial) {
    //         fgState = mForegroundCall.getState();
    //         bgState = mBackgroundCall.getState();
    //
    //         //holding foreground call failed
    //         if (fgState == ImsPhoneCall.State.ACTIVE) {
    //             throw new CallStateException("cannot dial in current state");
    //         }
    //
    //         //holding foreground call succeeded
    //         if (bgState == ImsPhoneCall.State.HOLDING) {
    //             holdBeforeDial = false;
    //         }
    //     }
    //
    //     mPendingMO = new ImsPhoneConnection(mPhone.getContext(),
    //             checkForTestEmergencyNumber(dialString), this,
    //             mForegroundCall, extras);
    // }
    // addConnection(mPendingMO);
    //
    // if (!holdBeforeDial) {
    //     if ((!isPhoneInEcmMode) || (isPhoneInEcmMode && isEmergencyNumber)) {
    //         dialInternal(mPendingMO, clirMode, videoState, extras);
    //     } else {
    //         try {
    //             getEcbmInterface().exitEmergencyCallbackMode();
    //         } catch (ImsException e) {
    //             e.printStackTrace();
    //             throw new CallStateException("service not available");
    //         }
    //         mPhone.setOnEcbModeExitResponse(this, EVENT_EXIT_ECM_RESPONSE_CDMA, null);
    //         pendingCallClirMode = clirMode;
    //         mPendingCallVideoState = videoState;
    //         pendingCallInEcm = true;
    //     }
    // }
    //
    // updatePhoneState();
    // mPhone.notifyPreciseCallStateChanged();
    //
    // return mPendingMO;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::AddParticipant(
    /* [in] */ const String& dialString)
{
    // ==================before translated======================
    // if (mForegroundCall != null) {
    //     ImsCall imsCall = mForegroundCall.getImsCall();
    //     if (imsCall == null) {
    //         loge("addParticipant : No foreground ims call");
    //     } else {
    //         ImsCallSession imsCallSession = imsCall.getCallSession();
    //         if (imsCallSession != null) {
    //             String[] callees = new String[] { dialString };
    //             imsCallSession.inviteParticipants(callees);
    //         } else {
    //             loge("addParticipant : ImsCallSession does not exist");
    //         }
    //     }
    // } else {
    //     loge("addParticipant : Foreground call does not exist");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::AcceptCall(
    /* [in] */ Int32 videoState)
{
    // ==================before translated======================
    // if (DBG) log("acceptCall");
    //
    // if (mForegroundCall.getState().isAlive()
    //         && mBackgroundCall.getState().isAlive()) {
    //     throw new CallStateException("cannot accept call");
    // }
    //
    // if ((mRingingCall.getState() == ImsPhoneCall.State.WAITING)
    //         && mForegroundCall.getState().isAlive()) {
    //     setMute(false);
    //     // Cache video state for pending MT call.
    //     mPendingCallVideoState = videoState;
    //     switchWaitingOrHoldingAndActive();
    // } else if (mRingingCall.getState().isRinging()) {
    //     if (DBG) log("acceptCall: incoming...");
    //     // Always unmute when answering a new call
    //     setMute(false);
    //     try {
    //         ImsCall imsCall = mRingingCall.getImsCall();
    //         if (imsCall != null) {
    //             imsCall.accept(ImsCallProfile.getCallTypeFromVideoState(videoState));
    //         } else {
    //             throw new CallStateException("no valid ims call");
    //         }
    //     } catch (ImsException e) {
    //         throw new CallStateException("cannot accept call");
    //     }
    // } else {
    //     throw new CallStateException("phone not ringing");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::DeflectCall(
    /* [in] */ const String& number)
{
    // ==================before translated======================
    // if (DBG) log("deflectCall");
    //
    // if (mRingingCall.getState().isRinging()) {
    //     try {
    //         ImsCall imsCall = mRingingCall.getImsCall();
    //         if (imsCall != null) {
    //             imsCall.deflect(number);
    //         } else {
    //             throw new CallStateException("no valid ims call to deflect");
    //         }
    //     } catch (ImsException e) {
    //         throw new CallStateException("cannot deflect call");
    //     }
    // } else {
    //     throw new CallStateException("phone not ringing");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::RejectCall()
{
    // ==================before translated======================
    // if (DBG) log("rejectCall");
    //
    // if (mRingingCall.getState().isRinging()) {
    //     hangup(mRingingCall);
    // } else {
    //     throw new CallStateException("phone not ringing");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::SwitchWaitingOrHoldingAndActive()
{
    // ==================before translated======================
    // if (DBG) log("switchWaitingOrHoldingAndActive");
    //
    // if (mRingingCall.getState() == ImsPhoneCall.State.INCOMING) {
    //     throw new CallStateException("cannot be in the incoming state");
    // }
    //
    // if (mForegroundCall.getState() == ImsPhoneCall.State.ACTIVE) {
    //     ImsCall imsCall = mForegroundCall.getImsCall();
    //     if (imsCall == null) {
    //         throw new CallStateException("no ims call");
    //     }
    //
    //     mForegroundCall.switchWith(mBackgroundCall);
    //
    //     try {
    //         imsCall.hold();
    //     } catch (ImsException e) {
    //         mForegroundCall.switchWith(mBackgroundCall);
    //         throw new CallStateException(e.getMessage());
    //     }
    // } else if (mBackgroundCall.getState() == ImsPhoneCall.State.HOLDING) {
    //     resumeWaitingOrHolding();
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::Conference()
{
    // ==================before translated======================
    // if (DBG) log("conference");
    //
    // ImsCall fgImsCall = mForegroundCall.getImsCall();
    // if (fgImsCall == null) {
    //     log("conference no foreground ims call");
    //     return;
    // }
    //
    // ImsCall bgImsCall = mBackgroundCall.getImsCall();
    // if (bgImsCall == null) {
    //     log("conference no background ims call");
    //     return;
    // }
    //
    // try {
    //     fgImsCall.merge(bgImsCall);
    // } catch (ImsException e) {
    //     log("conference " + e.getMessage());
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::ExplicitCallTransfer()
{
    // ==================before translated======================
    // //TODO : implement
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::ClearDisconnected()
{
    // ==================before translated======================
    // if (DBG) log("clearDisconnected");
    //
    // internalClearDisconnected();
    //
    // updatePhoneState();
    // mPhone.notifyPreciseCallStateChanged();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mForegroundCall.getState() == ImsPhoneCall.State.ACTIVE
    //     && mBackgroundCall.getState() == ImsPhoneCall.State.HOLDING
    //     && !mBackgroundCall.isFull()
    //     && !mForegroundCall.isFull();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::CanDial(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean ret;
    // int serviceState = mPhone.getServiceState().getState();
    // String disableCall = SystemProperties.get(
    //         TelephonyProperties.PROPERTY_DISABLE_CALL, "false");
    //
    // Rlog.d(LOGTAG, "canDial(): "
    //         + "\nserviceState = " + serviceState
    //         + "\npendingMO == null::=" + String.valueOf(mPendingMO == null)
    //         + "\nringingCall: " + mRingingCall.getState()
    //         + "\ndisableCall = " + disableCall
    //         + "\nforegndCall: " + mForegroundCall.getState()
    //         + "\nbackgndCall: " + mBackgroundCall.getState());
    //
    // ret = (serviceState != ServiceState.STATE_POWER_OFF)
    //     && mPendingMO == null
    //     && !mRingingCall.isRinging()
    //     && !disableCall.equals("true")
    //     && (!mForegroundCall.getState().isAlive()
    //             || !mBackgroundCall.getState().isAlive());
    //
    // return ret;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mForegroundCall.getState() == ImsPhoneCall.State.ACTIVE
    //     && mBackgroundCall.getState() == ImsPhoneCall.State.HOLDING;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // try {
    //     mImsManager.setUiTTYMode(mServiceId, uiTtyMode, onComplete);
    // } catch (ImsException e) {
    //     loge("setTTYMode : " + e);
    //     mPhone.sendErrorResponse(onComplete, e);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::SetMute(
    /* [in] */ Boolean mute)
{
    // ==================before translated======================
    // mDesiredMute = mute;
    // mForegroundCall.setMute(mute);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mDesiredMute;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::SendDtmf(
    /* [in] */ Char16 c)
{
    // ==================before translated======================
    // if (DBG) log("sendDtmf");
    //
    // ImsCall imscall = mForegroundCall.getImsCall();
    // if (imscall != null) {
    //     imscall.sendDtmf(c);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::StartDtmf(
    /* [in] */ Char16 c)
{
    // ==================before translated======================
    // if (DBG) log("startDtmf");
    //
    // ImsCall imscall = mForegroundCall.getImsCall();
    // if (imscall != null) {
    //     imscall.startDtmf(c);
    // } else {
    //     loge("startDtmf : no foreground call");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::StopDtmf()
{
    // ==================before translated======================
    // if (DBG) log("stopDtmf");
    //
    // ImsCall imscall = mForegroundCall.getImsCall();
    // if (imscall != null) {
    //     imscall.stopDtmf();
    // } else {
    //     loge("stopDtmf : no foreground call");
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::Hangup(
    /* [in] */ IImsPhoneConnection* conn)
{
    // ==================before translated======================
    // if (DBG) log("hangup connection");
    //
    // if (conn.getOwner() != this) {
    //     throw new CallStateException ("ImsPhoneConnection " + conn
    //             + "does not belong to ImsPhoneCallTracker " + this);
    // }
    //
    // hangup(conn.getCall());
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::Hangup(
    /* [in] */ IImsPhoneCall* call)
{
    // ==================before translated======================
    // if (DBG) log("hangup call");
    //
    // if (call.getConnections().size() == 0) {
    //     throw new CallStateException("no connections");
    // }
    //
    // ImsCall imsCall = call.getImsCall();
    // boolean rejectCall = false;
    //
    // if (call == mRingingCall) {
    //     if (Phone.DEBUG_PHONE) log("(ringing) hangup incoming");
    //     rejectCall = true;
    // } else if (call == mForegroundCall) {
    //     if (call.isDialingOrAlerting()) {
    //         if (Phone.DEBUG_PHONE) {
    //             log("(foregnd) hangup dialing or alerting...");
    //         }
    //     } else {
    //         if (Phone.DEBUG_PHONE) {
    //             log("(foregnd) hangup foreground");
    //         }
    //         //held call will be resumed by onCallTerminated
    //     }
    // } else if (call == mBackgroundCall) {
    //     if (Phone.DEBUG_PHONE) {
    //         log("(backgnd) hangup waiting or background");
    //     }
    // } else {
    //     throw new CallStateException ("ImsPhoneCall " + call +
    //             "does not belong to ImsPhoneCallTracker " + this);
    // }
    //
    // call.onHangupLocal();
    //
    // try {
    //     if (imsCall != null) {
    //         if (rejectCall) imsCall.reject(ImsReasonInfo.CODE_USER_DECLINE);
    //         else imsCall.terminate(ImsReasonInfo.CODE_USER_TERMINATED);
    //     } else if (mPendingMO != null && call == mForegroundCall) {
    //         // is holding a foreground call
    //         mPendingMO.update(null, ImsPhoneCall.State.DISCONNECTED);
    //         mPendingMO.onDisconnect();
    //         removeConnection(mPendingMO);
    //         mPendingMO = null;
    //         updatePhoneState();
    //         removeMessages(EVENT_DIAL_PENDINGMO);
    //     }
    // } catch (ImsException e) {
    //     throw new CallStateException(e.getMessage());
    // }
    //
    // mPhone.notifyPreciseCallStateChanged();
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::ResumeWaitingOrHolding()
{
    // ==================before translated======================
    // if (DBG) log("resumeWaitingOrHolding");
    //
    // try {
    //     if (mForegroundCall.getState().isAlive()) {
    //         //resume foreground call after holding background call
    //         //they were switched before holding
    //         ImsCall imsCall = mForegroundCall.getImsCall();
    //         if (imsCall != null) imsCall.resume();
    //     } else if (mRingingCall.getState() == ImsPhoneCall.State.WAITING) {
    //         //accept waiting call after holding background call
    //         ImsCall imsCall = mRingingCall.getImsCall();
    //         if (imsCall != null) {
    //             imsCall.accept(
    //                 ImsCallProfile.getCallTypeFromVideoState(mPendingCallVideoState));
    //         }
    //     } else {
    //         //Just resume background call.
    //         //To distinguish resuming call with swapping calls
    //         //we do not switch calls.here
    //         //ImsPhoneConnection.update will chnage the parent when completed
    //         ImsCall imsCall = mBackgroundCall.getImsCall();
    //         if (imsCall != null) imsCall.resume();
    //     }
    // } catch (ImsException e) {
    //     throw new CallStateException(e.getMessage());
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::SendUSSD(
    /* [in] */ const String& ussdString,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // if (DBG) log("sendUSSD");
    //
    // try {
    //     if (mUssdSession != null) {
    //         mUssdSession.sendUssd(ussdString);
    //         AsyncResult.forMessage(response, null, null);
    //         response.sendToTarget();
    //         return;
    //     }
    //
    //     String[] callees = new String[] { ussdString };
    //     ImsCallProfile profile = mImsManager.createCallProfile(mServiceId,
    //             ImsCallProfile.SERVICE_TYPE_NORMAL, ImsCallProfile.CALL_TYPE_VOICE);
    //     profile.setCallExtraInt(ImsCallProfile.EXTRA_DIALSTRING,
    //             ImsCallProfile.DIALSTRING_USSD);
    //
    //     mUssdSession = mImsManager.makeCall(mServiceId, profile,
    //             callees, mImsUssdListener);
    // } catch (ImsException e) {
    //     loge("sendUSSD : " + e);
    //     mPhone.sendErrorResponse(response, e);
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::CancelUSSD()
{
    // ==================before translated======================
    // if (mUssdSession == null) return;
    //
    // try {
    //     mUssdSession.terminate(ImsReasonInfo.CODE_USER_TERMINATED);
    // } catch (ImsException e) {
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::GetUtInterface(
    /* [out] */ /*TODO IImsUtInterface*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mImsManager == null) {
    //     throw new ImsException("no ims manager", ImsReasonInfo.CODE_UNSPECIFIED);
    // }
    //
    // ImsUtInterface ut = mImsManager.getSupplementaryServiceConfiguration(mServiceId);
    // return ut;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::NotifySrvccState(
    /* [in] */ ICallSrvccState state)
{
    // ==================before translated======================
    // if (DBG) log("notifySrvccState state=" + state);
    //
    // mSrvccState = state;
    //
    // if (mSrvccState == Call.SrvccState.COMPLETED) {
    //     transferHandoverConnections(mForegroundCall);
    //     transferHandoverConnections(mBackgroundCall);
    //     transferHandoverConnections(mRingingCall);
    //     // release wake lock hold
    //     ImsPhoneConnection con = mHandoverCall.getHandoverConnection();
    //     if (con != null) {
    //         con.releaseWakeLock();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // AsyncResult ar;
    // if (DBG) log("handleMessage what=" + msg.what);
    //
    // switch (msg.what) {
    //     case EVENT_HANGUP_PENDINGMO:
    //         if (mPendingMO != null) {
    //             mPendingMO.onDisconnect();
    //             removeConnection(mPendingMO);
    //             mPendingMO = null;
    //         }
    //
    //         updatePhoneState();
    //         mPhone.notifyPreciseCallStateChanged();
    //         break;
    //     case EVENT_RESUME_BACKGROUND:
    //         try {
    //             resumeWaitingOrHolding();
    //         } catch (CallStateException e) {
    //             if (Phone.DEBUG_PHONE) {
    //                 loge("handleMessage EVENT_RESUME_BACKGROUND exception=" + e);
    //             }
    //         }
    //         break;
    //     case EVENT_DIAL_PENDINGMO:
    //         dialInternal(mPendingMO, mClirMode, mPendingCallVideoState,
    //             mPendingMO.getCallExtras());
    //         break;
    //
    //     case EVENT_EXIT_ECM_RESPONSE_CDMA:
    //         // no matter the result, we still do the same here
    //         if (pendingCallInEcm) {
    //             dialInternal(mPendingMO, pendingCallClirMode,
    //                     mPendingCallVideoState, mPendingMO.getCallExtras());
    //             pendingCallInEcm = false;
    //         }
    //         mPhone.unsetOnEcbModeExitResponse(this);
    //         break;
    // }
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("ImsPhoneCallTracker extends:");
    // super.dump(fd, pw, args);
    // pw.println(" mVoiceCallEndedRegistrants=" + mVoiceCallEndedRegistrants);
    // pw.println(" mVoiceCallStartedRegistrants=" + mVoiceCallStartedRegistrants);
    // pw.println(" mRingingCall=" + mRingingCall);
    // pw.println(" mForegroundCall=" + mForegroundCall);
    // pw.println(" mBackgroundCall=" + mBackgroundCall);
    // pw.println(" mHandoverCall=" + mHandoverCall);
    // pw.println(" mPendingMO=" + mPendingMO);
    // //pw.println(" mHangupPendingMO=" + mHangupPendingMO);
    // pw.println(" mPhone=" + mPhone);
    // pw.println(" mDesiredMute=" + mDesiredMute);
    // pw.println(" mState=" + mState);
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::GetEcbmInterface(
    /* [out] */ /*TODO IImsEcbm*/IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mImsManager == null) {
    //     throw new ImsException("no ims manager", ImsReasonInfo.CODE_UNSPECIFIED);
    // }
    //
    // ImsEcbm ecbm = mImsManager.getEcbmInterface(mServiceId);
    // return ecbm;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::IsInEmergencyCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsInEmergencyCall;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::IsVolteEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsVolteEnabled;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::IsVtEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mIsVtEnabled;
    assert(0);
    return NOERROR;
}

ECode ImsPhoneCallTracker::Finalize()
{
    // ==================before translated======================
    // log("ImsPhoneCallTracker finalized");
    assert(0);
    return NOERROR;
}

void ImsPhoneCallTracker::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, "[ImsPhoneCallTracker] " + msg);
    assert(0);
}

void ImsPhoneCallTracker::Loge(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, "[ImsPhoneCallTracker] " + msg);
    assert(0);
}

void ImsPhoneCallTracker::HandlePollCalls(
    /* [in] */ AsyncResult* ar)
{
    assert(0);
}

AutoPtr<IPendingIntent> ImsPhoneCallTracker::CreateIncomingCallPendingIntent()
{
    // ==================before translated======================
    // Intent intent = new Intent(ImsManager.ACTION_IMS_INCOMING_CALL);
    // intent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
    // return PendingIntent.getBroadcast(mPhone.getContext(), 0, intent,
    //         PendingIntent.FLAG_UPDATE_CURRENT);
    assert(0);
    AutoPtr<IPendingIntent> empty;
    return empty;
}

void ImsPhoneCallTracker::GetImsService()
{
    // ==================before translated======================
    // if (DBG) log("getImsService");
    // mImsManager = ImsManager.getInstance(mPhone.getContext(), mPhone.getSubId());
    // try {
    //     mServiceId = mImsManager.open(ImsServiceClass.MMTEL,
    //             createIncomingCallPendingIntent(),
    //             mImsConnectionStateListener);
    //
    //     // Get the ECBM interface and set IMSPhone's listener object for notifications
    //     getEcbmInterface().setEcbmStateListener(mPhone.mImsEcbmStateListener);
    //     if (mPhone.isInEcm()) {
    //         // Call exit ECBM which will invoke onECBMExited
    //         mPhone.exitEmergencyCallbackMode();
    //     }
    //     int mPreferredTtyMode = Settings.Secure.getInt(
    //         mPhone.getContext().getContentResolver(),
    //         Settings.Secure.PREFERRED_TTY_MODE,
    //         Phone.TTY_MODE_OFF);
    //    mImsManager.setUiTTYMode(mServiceId, mPreferredTtyMode, null);
    //
    // } catch (ImsException e) {
    //     loge("getImsService: " + e);
    //     //Leave mImsManager as null, then CallStateException will be thrown when dialing
    //     mImsManager = null;
    // }
    assert(0);
}

void ImsPhoneCallTracker::HandleEcmTimer(
    /* [in] */ Int32 action)
{
    // ==================before translated======================
    // mPhone.handleTimerInEmergencyCallbackMode(action);
    // switch (action) {
    //     case ImsPhone.CANCEL_ECM_TIMER:
    //         break;
    //     case ImsPhone.RESTART_ECM_TIMER:
    //         break;
    //     default:
    //         log("handleEcmTimer, unsupported action " + action);
    // }
    assert(0);
}

void ImsPhoneCallTracker::DialInternal(
    /* [in] */ IImsPhoneConnection* conn,
    /* [in] */ Int32 clirMode,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras)
{
    // ==================before translated======================
    // if (conn == null) {
    //     return;
    // }
    //
    // boolean isConferenceUri = false;
    // boolean isSkipSchemaParsing = false;
    // if (extras != null) {
    //     isConferenceUri = extras.getBoolean(TelephonyProperties.EXTRA_DIAL_CONFERENCE_URI,
    //             false);
    //     isSkipSchemaParsing = extras.getBoolean(TelephonyProperties.EXTRA_SKIP_SCHEMA_PARSING,
    //             false);
    // }
    // if (!isConferenceUri && !isSkipSchemaParsing && (conn.getAddress()== null
    //         || conn.getAddress().length() == 0
    //         || conn.getAddress().indexOf(PhoneNumberUtils.WILD) >= 0)) {
    //     // Phone number is invalid
    //     conn.setDisconnectCause(DisconnectCause.INVALID_NUMBER);
    //     sendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
    //     return;
    // }
    //
    // // Always unmute when initiating a new call
    // setMute(false);
    // int serviceType = PhoneNumberUtils.isEmergencyNumber(conn.getAddress()) ?
    //         ImsCallProfile.SERVICE_TYPE_EMERGENCY : ImsCallProfile.SERVICE_TYPE_NORMAL;
    // int callType = ImsCallProfile.getCallTypeFromVideoState(videoState);
    // //TODO(vt): Is this sufficient?  At what point do we know the video state of the call?
    // conn.setVideoState(videoState);
    //
    // try {
    //     String[] callees = new String[] { conn.getAddress() };
    //     ImsCallProfile profile = mImsManager.createCallProfile(mServiceId,
    //             serviceType, callType);
    //     profile.setCallExtraInt(ImsCallProfile.EXTRA_OIR, clirMode);
    //     profile.setCallExtraBoolean(TelephonyProperties.EXTRAS_IS_CONFERENCE_URI,
    //             isConferenceUri);
    //
    //     if (extras != null) {
    //         // Pack the OEM-specific call extras.
    //         profile.mCallExtras.putBundle(ImsCallProfile.EXTRA_OEM_EXTRAS, extras);
    //         log("Packing OEM extras bundle in call profile.");
    //     } else {
    //         log("No dial extras packed in call profile.");
    //     }
    //
    //     ImsCall imsCall = mImsManager.makeCall(mServiceId, profile,
    //             callees, mImsCallListener);
    //     conn.setImsCall(imsCall);
    //
    //     IImsVideoCallProvider imsVideoCallProvider =
    //             imsCall.getCallSession().getVideoCallProvider();
    //     if (imsVideoCallProvider != null) {
    //         ImsVideoCallProviderWrapper imsVideoCallProviderWrapper =
    //                 new ImsVideoCallProviderWrapper(imsVideoCallProvider);
    //         conn.setVideoProvider(imsVideoCallProviderWrapper);
    //     }
    // } catch (ImsException e) {
    //     loge("dialInternal : " + e);
    //     conn.setDisconnectCause(DisconnectCause.ERROR_UNSPECIFIED);
    //     sendEmptyMessageDelayed(EVENT_HANGUP_PENDINGMO, TIMEOUT_HANGUP_PENDINGMO);
    // } catch (RemoteException e) {
    // }
    assert(0);
}

void ImsPhoneCallTracker::InternalClearDisconnected()
{
    // ==================before translated======================
    // mRingingCall.clearDisconnected();
    // mForegroundCall.clearDisconnected();
    // mBackgroundCall.clearDisconnected();
    // mHandoverCall.clearDisconnected();
    assert(0);
}

void ImsPhoneCallTracker::UpdatePhoneState()
{
    // ==================before translated======================
    // PhoneConstants.State oldState = mState;
    //
    // if (mRingingCall.isRinging()) {
    //     mState = PhoneConstants.State.RINGING;
    // } else if (mPendingMO != null ||
    //         !(mForegroundCall.isIdle() && mBackgroundCall.isIdle())) {
    //     mState = PhoneConstants.State.OFFHOOK;
    // } else {
    //     mState = PhoneConstants.State.IDLE;
    // }
    //
    // if (mState == PhoneConstants.State.IDLE && oldState != mState) {
    //     mVoiceCallEndedRegistrants.notifyRegistrants(
    //             new AsyncResult(null, null, null));
    // } else if (oldState == PhoneConstants.State.IDLE && oldState != mState) {
    //     mVoiceCallStartedRegistrants.notifyRegistrants (
    //             new AsyncResult(null, null, null));
    // }
    //
    // if (DBG) log("updatePhoneState oldState=" + oldState + ", newState=" + mState);
    //
    // if (mState != oldState) {
    //     mPhone.notifyPhoneStateChanged();
    // }
    assert(0);
}

void ImsPhoneCallTracker::HandleRadioNotAvailable()
{
    // ==================before translated======================
    // // handlePollCalls will clear out its
    // // call list when it gets the CommandException
    // // error result from this
    // pollCallsWhenSafe();
    assert(0);
}

void ImsPhoneCallTracker::DumpState()
{
    // ==================before translated======================
    // List l;
    //
    // log("Phone State:" + mState);
    //
    // log("Ringing call: " + mRingingCall.toString());
    //
    // l = mRingingCall.getConnections();
    // for (int i = 0, s = l.size(); i < s; i++) {
    //     log(l.get(i).toString());
    // }
    //
    // log("Foreground call: " + mForegroundCall.toString());
    //
    // l = mForegroundCall.getConnections();
    // for (int i = 0, s = l.size(); i < s; i++) {
    //     log(l.get(i).toString());
    // }
    //
    // log("Background call: " + mBackgroundCall.toString());
    //
    // l = mBackgroundCall.getConnections();
    // for (int i = 0, s = l.size(); i < s; i++) {
    //     log(l.get(i).toString());
    // }
    assert(0);
}

// synchronized
AutoPtr<IImsPhoneConnection> ImsPhoneCallTracker::FindConnection(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall)
{
    // ==================before translated======================
    // for (ImsPhoneConnection conn : mConnections) {
    //     if (conn.getImsCall() == imsCall) {
    //         return conn;
    //     }
    // }
    // return null;
    assert(0);
    AutoPtr<IImsPhoneConnection> empty;
    return empty;
}

// synchronized
void ImsPhoneCallTracker::RemoveConnection(
    /* [in] */ IImsPhoneConnection* conn)
{
    // ==================before translated======================
    // mConnections.remove(conn);
    assert(0);
}

// synchronized
void ImsPhoneCallTracker::AddConnection(
    /* [in] */ IImsPhoneConnection* conn)
{
    // ==================before translated======================
    // mConnections.add(conn);
    assert(0);
}

void ImsPhoneCallTracker::ProcessCallStateChange(
    /* [in] */ /*TODO IImsCall*/IInterface* imsCall,
    /* [in] */ ICallState state,
    /* [in] */ Int32 cause)
{
    // ==================before translated======================
    // if (DBG) log("processCallStateChange state=" + state + " cause=" + cause);
    //
    // if (imsCall == null) return;
    //
    // boolean changed = false;
    // ImsPhoneConnection conn = findConnection(imsCall);
    //
    // if (conn == null) {
    //     // TODO : what should be done?
    //     return;
    // }
    //
    // changed = conn.update(imsCall, state);
    //
    // if (state == ImsPhoneCall.State.DISCONNECTED) {
    //     changed = conn.onDisconnect(cause) || changed;
    //     removeConnection(conn);
    // }
    //
    // if (changed) {
    //     if (conn.getCall() == mHandoverCall) return;
    //     updatePhoneState();
    //     mPhone.notifyPreciseCallStateChanged();
    // }
    assert(0);
}

Int32 ImsPhoneCallTracker::GetDisconnectCauseFromReasonInfo(
    /* [in] */ IImsReasonInfo* reasonInfo)
{
    // ==================before translated======================
    // int cause = DisconnectCause.ERROR_UNSPECIFIED;
    //
    // //int type = reasonInfo.getReasonType();
    // int code = reasonInfo.getCode();
    // switch (code) {
    //     case ImsReasonInfo.CODE_SIP_BAD_ADDRESS:
    //     case ImsReasonInfo.CODE_SIP_NOT_REACHABLE:
    //         return DisconnectCause.NUMBER_UNREACHABLE;
    //
    //     case ImsReasonInfo.CODE_SIP_BUSY:
    //         return DisconnectCause.BUSY;
    //
    //     case ImsReasonInfo.CODE_USER_TERMINATED:
    //         return DisconnectCause.LOCAL;
    //
    //     case ImsReasonInfo.CODE_USER_TERMINATED_BY_REMOTE:
    //         return DisconnectCause.NORMAL;
    //
    //     case ImsReasonInfo.CODE_SIP_REDIRECTED:
    //     case ImsReasonInfo.CODE_SIP_BAD_REQUEST:
    //     case ImsReasonInfo.CODE_SIP_FORBIDDEN:
    //     case ImsReasonInfo.CODE_SIP_NOT_ACCEPTABLE:
    //     case ImsReasonInfo.CODE_SIP_USER_REJECTED:
    //     case ImsReasonInfo.CODE_SIP_GLOBAL_ERROR:
    //         return DisconnectCause.SERVER_ERROR;
    //
    //     case ImsReasonInfo.CODE_SIP_SERVICE_UNAVAILABLE:
    //     case ImsReasonInfo.CODE_SIP_NOT_FOUND:
    //     case ImsReasonInfo.CODE_SIP_SERVER_ERROR:
    //         return DisconnectCause.SERVER_UNREACHABLE;
    //
    //     case ImsReasonInfo.CODE_LOCAL_NETWORK_ROAMING:
    //     case ImsReasonInfo.CODE_LOCAL_NETWORK_IP_CHANGED:
    //     case ImsReasonInfo.CODE_LOCAL_IMS_SERVICE_DOWN:
    //     case ImsReasonInfo.CODE_LOCAL_SERVICE_UNAVAILABLE:
    //     case ImsReasonInfo.CODE_LOCAL_NOT_REGISTERED:
    //     case ImsReasonInfo.CODE_LOCAL_NETWORK_NO_LTE_COVERAGE:
    //     case ImsReasonInfo.CODE_LOCAL_NETWORK_NO_SERVICE:
    //     case ImsReasonInfo.CODE_LOCAL_CALL_VCC_ON_PROGRESSING:
    //         return DisconnectCause.OUT_OF_SERVICE;
    //
    //     case ImsReasonInfo.CODE_SIP_REQUEST_TIMEOUT:
    //     case ImsReasonInfo.CODE_TIMEOUT_1XX_WAITING:
    //     case ImsReasonInfo.CODE_TIMEOUT_NO_ANSWER:
    //     case ImsReasonInfo.CODE_TIMEOUT_NO_ANSWER_CALL_UPDATE:
    //         return DisconnectCause.TIMED_OUT;
    //
    //     case ImsReasonInfo.CODE_LOCAL_LOW_BATTERY:
    //     case ImsReasonInfo.CODE_LOCAL_POWER_OFF:
    //         return DisconnectCause.POWER_OFF;
    //
    //     default:
    // }
    //
    // return cause;
    assert(0);
    return 0;
}

void ImsPhoneCallTracker::TransferHandoverConnections(
    /* [in] */ IImsPhoneCall* call)
{
    // ==================before translated======================
    // if (call.mConnections != null) {
    //     for (Connection c : call.mConnections) {
    //         c.mPreHandoverState = call.mState;
    //         log ("Connection state before handover is " + c.getStateBeforeHandover());
    //     }
    // }
    // if (mHandoverCall.mConnections == null ) {
    //     mHandoverCall.mConnections = call.mConnections;
    // } else { // Multi-call SRVCC
    //     mHandoverCall.mConnections.addAll(call.mConnections);
    // }
    // if (mHandoverCall.mConnections != null) {
    //     if (call.getImsCall() != null) {
    //         call.getImsCall().close();
    //     }
    //     for (Connection c : mHandoverCall.mConnections) {
    //         ((ImsPhoneConnection)c).changeParent(mHandoverCall);
    //     }
    // }
    // if (call.getState().isAlive()) {
    //     log ("Call is alive and state is " + call.mState);
    //     mHandoverCall.mState = call.mState;
    // }
    // call.mConnections.clear();
    // call.mState = ImsPhoneCall.State.IDLE;
    assert(0);
}

} // namespace Imsphone
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
