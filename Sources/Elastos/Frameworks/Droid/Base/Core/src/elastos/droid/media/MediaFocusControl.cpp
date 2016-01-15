#include "elastos/droid/media/MediaFocusControl.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

Boolean MediaFocusControl::DEBUG_RC = FALSE;
/** Debug volumes */
Boolean MediaFocusControl::DEBUG_VOL = FALSE;

const String MediaFocusControl::IN_VOICE_COMM_FOCUS_ID("AudioFocus_For_Phone_Ring_And_Calls");
const String MediaFocusControl::TAG("MediaFocusControl");

AutoPtr<IUri> MediaFocusControl::ENABLED_NOTIFICATION_LISTENERS_URI = Settings.Secure.getUriFor(ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS);

const Int32 MediaFocusControl::RCD_REG_FAILURE = 0;
const Int32 MediaFocusControl::RCD_REG_SUCCESS_PERMISSION = 1;
const Int32 MediaFocusControl::RCD_REG_SUCCESS_ENABLED_NOTIF = 2;

// event handler messages
const Int32 MediaFocusControl::MSG_RCDISPLAY_CLEAR = 1;
const Int32 MediaFocusControl::MSG_RCDISPLAY_UPDATE = 2;
const Int32 MediaFocusControl::MSG_REEVALUATE_REMOTE = 3;
const Int32 MediaFocusControl::MSG_RCC_NEW_PLAYBACK_INFO = 4;
const Int32 MediaFocusControl::MSG_RCC_NEW_VOLUME_OBS = 5;
const Int32 MediaFocusControl::MSG_RCC_NEW_PLAYBACK_STATE = 6;
const Int32 MediaFocusControl::MSG_RCC_SEEK_REQUEST = 7;
const Int32 MediaFocusControl::MSG_RCC_UPDATE_METADATA = 8;
const Int32 MediaFocusControl::MSG_RCDISPLAY_INIT_INFO = 9;
const Int32 MediaFocusControl::MSG_REEVALUATE_RCD = 10;
const Int32 MediaFocusControl::MSG_UNREGISTER_MEDIABUTTONINTENT = 11;

// sendMsg() flags
/** If the msg is already queued, replace it with this one. */
const Int32 MediaFocusControl::SENDMSG_REPLACE = 0;
/** If the msg is already queued, ignore this one and leave the old. */
const Int32 MediaFocusControl::SENDMSG_NOOP = 1;
/** If the msg is already queued, queue this one and leave the old. */
const Int32 MediaFocusControl::SENDMSG_QUEUE = 2;

Object MediaFocusControl::mAudioFocusLock;
Object MediaFocusControl::mRingingLock;

const Int32 MediaFocusControl::VOICEBUTTON_ACTION_DISCARD_CURRENT_KEY_PRESS = 1;
const Int32 MediaFocusControl::VOICEBUTTON_ACTION_START_VOICE_INPUT = 2;
const Int32 MediaFocusControl::VOICEBUTTON_ACTION_SIMULATE_KEY_PRESS = 3;
const Int32 MediaFocusControl::WAKELOCK_RELEASE_ON_FINISHED = 1980; //magic number

// only set when wakelock was acquired, no need to check value when received
const String MediaFocusControl::EXTRA_WAKELOCK_ACQUIRED(
        "android.media.AudioService.WAKELOCK_ACQUIRED");

const Int32 MediaFocusControl::RC_INFO_NONE = 0;
const Int32 MediaFocusControl::RC_INFO_ALL =
        IRemoteControlClient::FLAG_INFORMATION_REQUEST_ALBUM_ART |
        IRemoteControlClient::FLAG_INFORMATION_REQUEST_KEY_MEDIA |
        IRemoteControlClient::FLAG_INFORMATION_REQUEST_METADATA |
        IRemoteControlClient::FLAG_INFORMATION_REQUEST_PLAYSTATE;

//==============================================================================
//  MediaFocusControl::NotificationListenerObserver
//==============================================================================

MediaFocusControl::NotificationListenerObserver::NotificationListenerObserver(
    /* [in] */ MediaFocusControl* host)
{
    constructor(mEventHandler);
    mContentResolver->RegisterContentObserver(Settings.Secure.getUriFor(
            ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS), FALSE, this);
}

ECode MediaFocusControl::NotificationListenerObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    if (!ENABLED_NOTIFICATION_LISTENERS_URI->Equals(uri) || selfChange) {
        return NOERROR;
    }
    if (DEBUG_RC) { Logger::D(TAG, "NotificationListenerObserver.onChange()"); }
    PostReevaluateRemoteControlDisplays();
    return NOERROR;
}

//==============================================================================
//  MediaFocusControl::MediaEventHandler
//==============================================================================

ECode MediaFocusControl::MediaEventHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case MSG_RCDISPLAY_CLEAR: {
            OnRcDisplayClear();
            break;
        }
        case MSG_RCDISPLAY_UPDATE: {
            // msg.obj is guaranteed to be non NULL
            Int32 arg1;
            msg->GetArg1(&arg1);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            OnRcDisplayUpdate(IPlayerRecord::Probe(obj), arg1);
            break;
        }
        case MSG_REEVALUATE_REMOTE: {
            OnReevaluateRemote();
            break;
        }
        case MSG_RCC_NEW_VOLUME_OBS: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            OnRegisterVolumeObserverForRcc(arg1 /* rccId */,
                    IIRemoteVolumeObserver::Porbe(obj) /* rvo */);
            break;
        }
        case MSG_RCDISPLAY_INIT_INFO: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            Int32 arg2;
            msg->GetArg2(&arg2);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            // msg.obj is guaranteed to be non NULL
            OnRcDisplayInitInfo(IIRemoteControlDisplay::Porbe(obj) /*newRcd*/,
                    arg1/*w*/, arg2/*h*/);
            break;
        }
        case MSG_REEVALUATE_RCD: {
            OnReevaluateRemoteControlDisplays();
            break;
        }
        case MSG_UNREGISTER_MEDIABUTTONINTENT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            UnregisterMediaButtonIntent(IPendingIntent::Probe(obj));
            break;
        }
    }
}

//==============================================================================
//  MediaFocusControl::MyPhoneStateListener
//==============================================================================

ECode MediaFocusControl::MyPhoneStateListener::OnCallStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ const String& incomingNumber)
{
    if (state == ITelephonyManager::CALL_STATE_RINGING) {
        //Log.v(TAG, " CALL_STATE_RINGING");
        synchronized(mRingingLock) {
            mIsRinging = TRUE;
        }
    }
    else if ((state == ITelephonyManager::CALL_STATE_OFFHOOK)
            || (state == ITelephonyManager::CALL_STATE_IDLE)) {
        synchronized(mRingingLock) {
            mIsRinging = FALSE;
        }
    }
    return NOERROR;
}

//==============================================================================
//  MediaFocusControl::AudioFocusDeathHandler
//==============================================================================

CAR_INTERFACE_IMPL(MediaFocusControl::AudioFocusDeathHandler, Object, IProxyDeathRecipient)

ECode MediaFocusControl::AudioFocusDeathHandler::BinderDied()
{
    synchronized(mAudioFocusLock) {
        Logger::W(TAG, "  AudioFocus   audio focus client died");
        RemoveFocusStackEntryForClient(mCb);
    }
    return NOERROR;
}

ECode MediaFocusControl::AudioFocusDeathHandler::GetBinder(
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCb;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//==============================================================================
//  MediaFocusControl::MyBroadcastReceiver
//==============================================================================

ECode MediaFocusControl::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (intent == NULL) {
        return NOERROR;
    }
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);
    if (extras == NULL) {
        return NOERROR;
    }
    Boolean b;
    if (extras->ContainsKey(EXTRA_WAKELOCK_ACQUIRED, &b), b) {
        mMediaEventWakeLock->ReleaseResources();
    }
    return NOERROR;
}

//==============================================================================
//  MediaFocusControl::DisplayInfoForServer
//==============================================================================

CAR_INTERFACE_IMPL(MediaFocusControl::DisplayInfoForServer, Object, IProxyDeathRecipient)

MediaFocusControl::DisplayInfoForServer::DisplayInfoForServer(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ MediaFocusControl* host)
    : mArtworkExpectedWidth(-1)
    , mArtworkExpectedHeight(-1)
    , mWantsPositionSync(FALSE)
    , mEnabled(TRUE)
{
    if (DEBUG_RC) Logger::I(TAG, "new DisplayInfoForServer for %p w=%d h=%d", rcd, w, h);
    mRcDisplay = rcd;
    mRcDisplayBinder = IBinder::Probe(rcd);
    mArtworkExpectedWidth = w;
    mArtworkExpectedHeight = h;
}

ECode MediaFocusControl::DisplayInfoForServer::Init(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    AutoPtr<IProxy> proxy = IProxy::Probe(mRcDisplayBinder);
    if (proxy != NULL) proxy->LinkToDeath(this, 0);
    // } catch (RemoteException e) {
    //     // remote control display is DOA, disqualify it
    //     Logger::W(TAG, "registerRemoteControlDisplay() has a dead client " + mRcDisplayBinder);
    //     return FALSE;
    // }
    *result = TRUE;
    return NOERROR;
}

ECode MediaFocusControl::DisplayInfoForServer::ReleaseResources()
{
    // try {
    AutoPtr<IProxy> proxy = IProxy::Probe(mRcDisplayBinder);
    if (proxy != NULL) proxy->UnLinkToDeath(this, 0);
    // } catch (java.util.NoSuchElementException e) {
    //     // not much we can do here, the display should have been unregistered anyway
    //     Logger::E(TAG, "Error in DisplaInfoForServer.relase()", e);
    // }
    return NOERROR;
}

ECode MediaFocusControl::DisplayInfoForServer::BinderDied()
{
    synchronized(mPRStack) {
        Logger::W(TAG, "RemoteControl: display %p died", mRcDisplay.Get());
        // remove the display from the list
        AutoPtr<ArrayOf<IInterface*> > array;
        mHost->mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<DisplayInfoForServer> di = (*array)[i];
            if (di->mRcDisplay == mRcDisplay) {
                if (DEBUG_RC) Logger::W(TAG, " RCD removed from list");
                DisplayIterator->Remove(i);
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

//==============================================================================
//  MediaFocusControl
//==============================================================================

CAR_INTERFACE_IMPL(MediaFocusControl, Object, IMediaFocusControl)

MediaFocusControl::MediaFocusControl()
    : mRole(0)
    , mIsRinging(FALSE)
    , mVoiceButtonDown(FALSE)
    , mVoiceButtonHandled(FALSE)
    , mCurrentRcClientGen(0)
    , mMainRemoteIsActive(FALSE)
    , mHasRemotePlayback(FALSE)
{
    mKeyEventDone = new MyBroadcastReceiver(this);
    CStack::New((IStack**)&mPRStack);
    CArrayList::New((IArrayList**)&mRcDisplays);
    mPhoneStateListener = new MyPhoneStateListener(this);
}

MediaFocusControl::~MediaFocusControl()
{
}

ECode MediaFocusControl::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ IContext* cntxt,
    /* [in] */ IAudioServiceVolumeController* volumeCtrl,
    /* [in] */ IAudioService* as)
{
    mEventHandler = new MediaEventHandler(this, looper);
    mContext = cntxt;
    mContext->GetContentResolver((IContentResolver**)&mContentResolver);
    mVolumeController = volumeCtrl;
    mAudioService = as;

    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::POWER_SERVICE, ((IInterface**)service));
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);

    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            String("handleMediaEvent"), (IPowerManagerWakeLock**)&mMediaEventWakeLock);
// TODO: Need CAudioService
    // mMainRemote = new CAudioService::RemotePlaybackState(-1,
    //         AudioService::GetMaxStreamVolume(IAudioManager::STREAM_MUSIC),
    //         AudioService::GetMaxStreamVolume(IAudioManager::STREAM_MUSIC));

    // Register for phone state monitoring

    service = NULL;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, ((IInterface**)service));
    AutoPtr<ITelephonyManager> tmgr = ITelephonyManager::Probe(service);
    tmgr->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_CALL_STATE);

    service = NULL;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, ((IInterface**)service));
    mAppOps = IAppOpsManager::Probe(service);

    service = NULL;
    mContext->GetSystemService(IContext::KEYGUARD_SERVICE, ((IInterface**)service));
    mKeyguardManager = IKeyguardManager::Probe(service);
    mNotifListenerObserver = new NotificationListenerObserver(this);

    mHasRemotePlayback = FALSE;
    mMainRemoteIsActive = FALSE;

    PlayerRecord->SetMediaFocusControl(this);

    PostReevaluateRemote();
    return NOERROR;
}

ECode MediaFocusControl::OnSendFinished(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras)
{
    if (resultCode == WAKELOCK_RELEASE_ON_FINISHED) {
        mMediaEventWakeLock->ReleaseResources();
    }
    return NOERROR;
}

void MediaFocusControl::Dump(
    /* [in] */ IPrintWriter* pw)
{
    DumpFocusStack(pw);
    DumpRCStack(pw);
    DumpRCCStack(pw);
    DumpRCDList(pw);
}

Boolean MediaFocusControl::RegisterRemoteController(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IComponentName* listenerComp)
{
    Int32 reg = CheckRcdRegistrationAuthorization(listenerComp);
    if (reg != RCD_REG_FAILURE) {
        RegisterRemoteControlDisplay_int(rcd, w, h, listenerComp);
        return TRUE;
    }
    else {
        // Slogger::W(TAG, "Access denied to process: " + Binder.getCallingPid() +
        //         ", must have permission " + android.Manifest.permission.MEDIA_CONTENT_CONTROL +
        //         " or be an enabled NotificationListenerService for registerRemoteController");
        return FALSE;
    }
}

Boolean MediaFocusControl::RegisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    Int32 reg = CheckRcdRegistrationAuthorization(NULL);
    if (reg != RCD_REG_FAILURE) {
        RegisterRemoteControlDisplay_int(rcd, w, h, NULL);
        return TRUE;
    }
    else {
        // Slogger::W(TAG, "Access denied to process: " + Binder.getCallingPid() +
        //         ", must have permission " + android.Manifest.permission.MEDIA_CONTENT_CONTROL +
        //         " to register IRemoteControlDisplay");
        return FALSE;
    }
}

void MediaFocusControl::DiscardAudioFocusOwner()
{
    synchronized(mAudioFocusLock) {
        Boolean b;
        if (!(mFocusStack->IsEmpty(&b), b)) {
            // notify the current focus owner it lost focus after removing it from stack
            AutoPtr<IInterface> obj;
            mFocusStack->Pop(((IInterface**)&obj));
            AutoPtr<IFocusRequester> exFocusOwner = IFocusRequester::Probe(obj);
            exFocusOwner->HandleFocusLoss(IAudioManager::AUDIOFOCUS_LOSS);
            exFocusOwner->ReleaseResources();
        }
    }
}

Int32 MediaFocusControl::GetCurrentAudioFocus()
{
    synchronized(mAudioFocusLock) {
        Boolean b;
        if ((mFocusStack->IsEmpty(&b), b)) {
            return IAudioManager::AUDIOFOCUS_NONE;
        }
        else {
            AutoPtr<IInterface> obj;
            mFocusStack->Peek(((IInterface**)&obj));
            AutoPtr<IFocusRequester> exFocusOwner = IFocusRequester::Probe(obj);
            Int32 val;
            exFocusOwner->GetGainRequest(&val);
            return val;
        }
    }
}

Int32 MediaFocusControl::RequestAudioFocus(
    /* [in] */ Int32 mainStreamType,
    /* [in] */ Int32 focusChangeHint,
    /* [in] */ IBinder* cb,
    /* [in] */ IIAudioFocusDispatcher* fd,
    /* [in] */ const String& clientId,
    /* [in] */ const String& callingPackageName)
{
    Logger::I(TAG, " AudioFocus  requestAudioFocus() from %s", clientId.string());
    // we need a valid binder callback for clients
    if (!cb->PingBinder()) {
        Logger::E(TAG, " AudioFocus DOA client for requestAudioFocus(), aborting.");
        return IAudioManager::AUDIOFOCUS_REQUEST_FAILED;
    }

    if (mAppOps->NoteOp(IAppOpsManager::OP_TAKE_AUDIO_FOCUS, Binder::GetCallingUid(),
            callingPackageName) != IAppOpsManager::MODE_ALLOWED) {
        return IAudioManager::AUDIOFOCUS_REQUEST_FAILED;
    }

    synchronized(mAudioFocusLock) {
        if (!CanReassignAudioFocus()) {
            return IAudioManager::AUDIOFOCUS_REQUEST_FAILED;
        }

        // handle the potential premature death of the new holder of the focus
        // (premature death == death before abandoning focus)
        // Register for client death notification
        AutoPtr<AudioFocusDeathHandler> afdh = new AudioFocusDeathHandler(this, cb);
        // try {
        AutoPtr<IProxy> proxy = IProxy::Probe(cb);
        if (proxy != NULL) proxy->LinkToDeath(afdh, 0);
        // } catch (RemoteException e) {
        //     // client has already died!
        //     Logger::W(TAG, "AudioFocus  requestAudioFocus() could not link to "+cb+" binder death");
        //     return AudioManager.AUDIOFOCUS_REQUEST_FAILED;
        // }

        Boolean b1, b2;
        mFocusStack->IsEmpty(&b1);
        AutoPtr<IInterface> obj;
        mFocusStack->Peek((IInterface**)&obj);
        AutoPtr<IFocusRequester> exFocusOwner = IFocusRequester::Probe(obj);
        exFocusOwner->HasSameClient(clientId, &b2);
        if (!b1 && b2) {
            // if focus is already owned by this client and the reason for acquiring the focus
            // hasn't changed, don't do anything
            Int32 val;
            exFocusOwner->GetGainRequest(&val);
            if (val == focusChangeHint) {
                // unlink death handler so it can be gc'ed.
                // linkToDeath() creates a JNI global reference preventing collection.
                proxy->UnlinkToDeath(afdh, 0);
                return IAudioManager::AUDIOFOCUS_REQUEST_GRANTED;
            }
            // the reason for the audio focus request has changed: remove the current top of
            // stack and respond as if we had a new focus owner
            obj = NULL;
            mFocusStack->Pop((IInterface**)&obj);
            AutoPtr<IFocusRequester> fr = IFocusRequester::Probe(obj);
            fr->ReleaseResources();
        }

        // focus requester might already be somewhere below in the stack, remove it
        RemoveFocusStackEntry(clientId, FALSE /* signal */);

        // propagate the focus change through the stack
        Boolean b;
        if (!(mFocusStack->IsEmpty(&b), b)) {
            PropagateFocusLossFromGain_syncAf(focusChangeHint);
        }

        // push focus requester at the top of the audio focus stack
        AutoPtr<IFocusRequester> req;
        CFocusRequester::New(mainStreamType, focusChangeHint, fd, cb,
                clientId, afdh, callingPackageName, Binder::GetCallingUid(),
                (IFocusRequester**)&req);

        mFocusStack->Push(req);

    }//synchronized(mAudioFocusLock)

    return IAudioManager::AUDIOFOCUS_REQUEST_GRANTED;
}

Int32 MediaFocusControl::AbandonAudioFocus(
    /* [in] */ IIAudioFocusDispatcher* fl,
    /* [in] */ const String* clientId)
{
    Logger::I(TAG, " AudioFocus  abandonAudioFocus() from %s", clientId.string());
    // try {
    // this will take care of notifying the new focus owner if needed
    synchronized(mAudioFocusLock) {
        RemoveFocusStackEntry(clientId, TRUE /*signal*/);
    }
    // } catch (java.util.ConcurrentModificationException cme) {
    //     // Catching this exception here is temporary. It is here just to prevent
    //     // a crash seen when the "Silent" notification is played. This is believed to be fixed
    //     // but this try catch block is left just to be safe.
    //     Logger::E(TAG, "FATAL EXCEPTION AudioFocus  abandonAudioFocus() caused " + cme);
    //     cme.printStackTrace();
    // }

    return IAudioManager::AUDIOFOCUS_REQUEST_GRANTED;
}

void MediaFocusControl::UnregisterAudioFocusClient(
    /* [in] */ const String& clientId)
{
    synchronized(mAudioFocusLock) {
        RemoveFocusStackEntry(clientId, FALSE);
    }
}

void MediaFocusControl::DispatchMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent)
{
    FilterMediaKeyEvent(keyEvent, FALSE /*needWakeLock*/);
}

void MediaFocusControl::DispatchMediaKeyEventUnderWakelock(
    /* [in] */ IKeyEvent* keyEvent)
{
    FilterMediaKeyEvent(keyEvent, TRUE /*needWakeLock*/);
}

void MediaFocusControl::RegisterMediaButtonIntent(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IComponentName* eventReceiver,
    /* [in] */ IBinder* token)
{
    Logger::I(TAG, "  Remote Control   registerMediaButtonIntent() for %p", mediaIntent);

    synchronized(mPRStack) {
        if (PushMediaButtonReceiver_syncPrs(mediaIntent, eventReceiver, token)) {
            // new RC client, assume every type of information shall be queried
            CheckUpdateRemoteControlDisplay_syncPrs(RC_INFO_ALL);
        }
    }
}

void MediaFocusControl::UnregisterMediaButtonIntent(
    /* [in] */ IPendingIntent* mediaIntent)
{
    Logger::I(TAG, "  Remote Control   unregisterMediaButtonIntent() for %p", mediaIntent);

    synchronized(mPRStack) {
        Boolean topOfStackWillChange = IsCurrentRcController(mediaIntent);
        RemoveMediaButtonReceiver_syncPrs(mediaIntent);
        if (topOfStackWillChange) {
            // current RC client will change, assume every type of info needs to be queried
            CheckUpdateRemoteControlDisplay_syncPrs(RC_INFO_ALL);
        }
    }
}

void MediaFocusControl::UnregisterMediaButtonIntentAsync(
    /* [in] */ IPendingIntent* mediaIntent)
{
    AutoPtr<IMessage> msg;
    mEventHandler->ObtainMessage(MSG_UNREGISTER_MEDIABUTTONINTENT, 0, 0,
                    mediaIntent, (IMessage**)&msg);
    Boolean b;
    mEventHandler->SendMessage(msg, &b);
}

void MediaFocusControl::RegisterMediaButtonEventReceiverForCalls(
    /* [in] */ IComponentName* c)
{
    Int32 val;
    mContext->CheckCallingPermission(String("android.permission.MODIFY_PHONE_STATE"), &val);
    if (val != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "Invalid permissions to register media button receiver for calls");
        return;
    }
    synchronized(mPRStack) {
        mMediaReceiverForCalls = c;
    }
}

void MediaFocusControl::UnregisterMediaButtonEventReceiverForCalls()
{
    Int32 val;
    mContext->CheckCallingPermission(String("android.permission.MODIFY_PHONE_STATE"), &val);
    if (val != IPackageManager::PERMISSION_GRANTED) {
        Logger::E(TAG, "Invalid permissions to unregister media button receiver for calls");
        return;
    }
    synchronized(mPRStack) {
        mMediaReceiverForCalls = NULL;
    }
}

Int32 MediaFocusControl::RegisterRemoteControlClient(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IIRemoteControlClient* rcClient,
    /* [in] */ const String& callingPackageName)
{
    if (DEBUG_RC) Logger::I(TAG, "Register remote control client rcClient=%p", rcClient);
    Int32 rccId = IRemoteControlClient::RCSE_ID_UNREGISTERED;
    synchronized(mPRStack) {
        // store the new display information
        // try {
        for (Int32 index = mPRStack->GetSize() - 1; index >= 0; index--) {
            AutoPtr<IInterface> obj;
            mPRStack->Get(index, (IInterface**)&obj);
            AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);
            if(prse->HasMatchingMediaButtonIntent(mediaIntent)) {
                prse->ResetControllerInfoForRcc(rcClient, callingPackageName,
                        Binder::GetCallingUid());

                if (rcClient == NULL) {
                    break;
                }

                rccId = prse->GetRccId();

                // there is a new (non-NULL) client:
                //     give the new client the displays (if any)
                Int32 size;
                mRcDisplays->GetSize(&size);
                if (size > 0) {
                    PlugRemoteControlDisplaysIntoClient_syncPrs(prse->GetRcc());
                }
                break;
            }
        }//for
        // } catch (ArrayIndexOutOfBoundsException e) {
        //     // not expected to happen, indicates improper concurrent modification
        //     Logger::E(TAG, "Wrong index accessing RC stack, lock error? ", e);
        // }

        // if the eventReceiver is at the top of the stack
        // then check for potential refresh of the remote controls
        if (IsCurrentRcController(mediaIntent)) {
            CheckUpdateRemoteControlDisplay_syncPrs(RC_INFO_ALL);
        }
    }//synchronized(mPRStack)
    return rccId;
}

void MediaFocusControl::UnregisterRemoteControlClient(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IIRemoteControlClient* rcClient)
{
    if (DEBUG_RC) Logger::I(TAG, "Unregister remote control client rcClient=%p", rcClient);
    synchronized(mPRStack) {
        Boolean topRccChange = FALSE;
        // try {
        for (Int32 index = mPRStack->GetSize() - 1; index >= 0; index--) {
            AutoPtr<IInterface> obj;
            mPRStack->Get(index, (IInterface**)&obj);
            AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);
            if ((prse->HasMatchingMediaButtonIntent(mediaIntent))
                    && rcClient->Equals(prse->GetRcc())) {
                // we found the IRemoteControlClient to unregister
                prse->ResetControllerInfoForNoRcc();
                topRccChange = (index == mPRStack->GetSize()-1);
                // there can only be one matching RCC in the RC stack, we're done
                break;
            }
        }
        // } catch (ArrayIndexOutOfBoundsException e) {
        //     // not expected to happen, indicates improper concurrent modification
        //     Logger::E(TAG, "Wrong index accessing RC stack, lock error? ", e);
        // }
        if (topRccChange) {
            // no more RCC for the RCD, check for potential refresh of the remote controls
            CheckUpdateRemoteControlDisplay_syncPrs(RC_INFO_ALL);
        }
    }
}

void MediaFocusControl::UnregisterRemoteControlDisplay(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    if (DEBUG_RC) Logger::D(TAG, "<<< unregisterRemoteControlDisplay(%p)", rcd);
    synchronized(mPRStack) {
        if (rcd == NULL) {
            return;
        }

        Boolean displayWasPluggedIn = FALSE;
        AutoPtr<ArrayOf<IInterface*> > array;
        mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength() && !displayWasPluggedIn; i++) {
            AutoPtr<DisplayInfoForServer> di = (*array)[i];
            if (IBinder::Probe(di->mRcDisplay)->Equals(IBinder::Probe(rcd))) {
                displayWasPluggedIn = TRUE;
                di->ReleaseResources();
                displayIterator->Remove(i);
            }
        }

        if (displayWasPluggedIn) {
            // disconnect this remote control display from all the clients, so the remote
            //   control stack traversal order doesn't matter
            array = NULL;
            mPRStack->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                AutoPtr<IPlayerRecord> prse = (*array)[i];
                if(prse->GetRcc() != NULL) {
                    // try {
                    prse->GetRcc()->UnplugRemoteControlDisplay(rcd);
                    // } catch (RemoteException e) {
                    //     Logger::E(TAG, "Error disconnecting remote control display to client: ", e);
                    // }
                }
            }
        }
        else {
            if (DEBUG_RC) Logger::W(TAG, "  trying to unregister unregistered RCD");
        }
    }
}

void MediaFocusControl::RemoteControlDisplayUsesBitmapSize(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    synchronized(mPRStack) {
        AutoPtr<ArrayOf<IInterface*> > array;
        mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
        Boolean artworkSizeUpdate = FALSE;
        for (Int32 i = 0; i < array->GetLength() && !artworkSizeUpdate; i++) {
            AutoPtr<DisplayInfoForServer> di = (*array)[i];
            if (IBinder::Probe(di->mRcDisplay)->Equals(IBinder::Probe(rcd))) {
                if ((di->mArtworkExpectedWidth != w) || (di->mArtworkExpectedHeight != h)) {
                    di->mArtworkExpectedWidth = w;
                    di->mArtworkExpectedHeight = h;
                    artworkSizeUpdate = TRUE;
                }
            }
        }
        if (artworkSizeUpdate) {
            // RCD is currently plugged in and its artwork size has changed, notify all RCCs,
            // stack traversal order doesn't matter
            array = NULL;
            mPRStack->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                AutoPtr<IPlayerRecord> prse = (*array)[i];
                if(prse->GetRcc() != NULL) {
                    // try {
                    prse->GetRcc()->SetBitmapSizeForDisplay(rcd, w, h);
                    // } catch (RemoteException e) {
                    //     Logger::E(TAG, "Error setting bitmap size for RCD on RCC: ", e);
                    // }
                }
            }
        }
    }
}

void MediaFocusControl::RemoteControlDisplayWantsPlaybackPositionSync(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Boolean wantsSync)
{
    synchronized(mPRStack) {
        Boolean rcdRegistered = FALSE;
        // store the information about this display
        // (display stack traversal order doesn't matter).
        AutoPtr<ArrayOf<IInterface*> > array;
        mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<DisplayInfoForServer> di = (*array)[i];
            if (IBinder::Probe(di->mRcDisplay)->Equals(IBinder::Probe(rcd))) {
                di->mWantsPositionSync = wantsSync;
                rcdRegistered = TRUE;
                break;
            }
        }
        if (!rcdRegistered) {
            return;
        }

        // notify all current RemoteControlClients
        // (stack traversal order doesn't matter as we notify all RCCs)
        array = NULL;
        mPRStack->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IPlayerRecord> prse = (*array)[i];
            if (prse->GetRcc() != NULL) {
                // try {
                prse->GetRcc()->SetWantsSyncForDisplay(rcd, wantsSync);
                // } catch (RemoteException e) {
                //     Logger::E(TAG, "Error setting position sync flag for RCD on RCC: ", e);
                // }
            }
        }
    }
}

Boolean MediaFocusControl::CheckUpdateRemoteStateIfActive(
    /* [in] */ Int32 streamType)
{
    synchronized(mPRStack) {
        // iterating from top of stack as active playback is more likely on entries at the top
        // try {
        Int32 size;
        mPRStack->GetSize(&size);
        for (Int32 index = size-1; index >= 0; index--) {
            AutoPtr<IInterface> obj;
            mPRStack->Get(index, (IInterface**)&obj);
            AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);

            if ((prse->mPlaybackType == IRemoteControlClient::PLAYBACK_TYPE_REMOTE)
                    && IsPlaystateActive(prse->mPlaybackState->mState)
                    && (prse->mPlaybackStream == streamType)) {
                if (DEBUG_RC) Logger::D(TAG, "remote playback active on stream %d, vol =%d",
                    streamType, prse->mPlaybackVolume);
                synchronized(mMainRemote) {
                    mMainRemote->mRccId = prse->GetRccId();
                    mMainRemote->mVolume = prse->mPlaybackVolume;
                    mMainRemote->mVolumeMax = prse->mPlaybackVolumeMax;
                    mMainRemote->mVolumeHandling = prse->mPlaybackVolumeHandling;
                    mMainRemoteIsActive = TRUE;
                }
                return TRUE;
            }
        }
        // } catch (ArrayIndexOutOfBoundsException e) {
        //     // not expected to happen, indicates improper concurrent modification
        //     Logger::E(TAG, "Wrong index accessing RC stack, lock error? ", e);
        // }
    }
    synchronized(mMainRemote) {
        mMainRemoteIsActive = FALSE;
    }
    return FALSE;
}

Boolean MediaFocusControl::IsPlaystateActive(
    /* [in] */ Int32 playState)
{
    switch (playState) {
        case IRemoteControlClient::PLAYSTATE_PLAYING:
        case IRemoteControlClient::PLAYSTATE_BUFFERING:
        case IRemoteControlClient::PLAYSTATE_FAST_FORWARDING:
        case IRemoteControlClient::PLAYSTATE_REWINDING:
        case IRemoteControlClient::PLAYSTATE_SKIPPING_BACKWARDS:
        case IRemoteControlClient::PLAYSTATE_SKIPPING_FORWARDS:
            return TRUE;
        default:
            return FALSE;
    }
}

Int32 MediaFocusControl::GetRemoteStreamMaxVolume()
{
    synchronized(mMainRemote) {
        if (mMainRemote->mRccId == IRemoteControlClient::RCSE_ID_UNREGISTERED) {
            return 0;
        }
        return mMainRemote->mVolumeMax;
    }
}

Int32 MediaFocusControl::GetRemoteStreamVolume()
{
    synchronized(mMainRemote) {
        if (mMainRemote.mRccId == RemoteControlClient.RCSE_ID_UNREGISTERED) {
            return 0;
        }
        return mMainRemote.mVolume;
    }
}

void MediaFocusControl::SetRemoteStreamVolume(
    /* [in] */ Int32 vol)
{
    if (DEBUG_VOL) { Logger::D(TAG, "setRemoteStreamVolume(vol=%d)", vol); }
    Int32 rccId = IRemoteControlClient::RCSE_ID_UNREGISTERED;
    synchronized(mMainRemote) {
        if (mMainRemote->mRccId == IRemoteControlClient::RCSE_ID_UNREGISTERED) {
            return;
        }
        rccId = mMainRemote->mRccId;
    }
    AutoPtr<IIRemoteVolumeObserver> rvo;
    synchronized(mPRStack) {
        // The stack traversal order doesn't matter because there is only one stack entry
        //  with this RCC ID, but the matching ID is more likely at the top of the stack, so
        //  start iterating from the top.
        // try {
        Int32 size;
        mPRStack->GetSize(&size);
        for (Int32 index = size-1; index >= 0; index--) {
            AutoPtr<IInterface> obj;
            mPRStack->Get(index, (IInterface**)&obj);
            AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);

            //FIXME OPTIMIZE store this info in mMainRemote so we don't have to iterate?
            if (prse->GetRccId() == rccId) {
                rvo = prse->mRemoteVolumeObs;
                break;
            }
        }
        // } catch (ArrayIndexOutOfBoundsException e) {
        //     // not expected to happen, indicates improper concurrent modification
        //     Logger::E(TAG, "Wrong index accessing media button stack, lock error? ", e);
        // }
    }
    if (rvo != NULL) {
        // try {
        rvo->DispatchRemoteVolumeUpdate(0, vol);
        // } catch (RemoteException e) {
        //     Logger::E(TAG, "Error dispatching absolute volume update", e);
        // }
    }
}

void MediaFocusControl::PostReevaluateRemote()
{
    SendMsg(mEventHandler, MSG_REEVALUATE_REMOTE, SENDMSG_QUEUE, 0, 0, NULL, 0);
}

Int32 MediaFocusControl::CheckRcdRegistrationAuthorization(
    /* [in] */ IComponentName* listenerComp)
{
    // MEDIA_CONTENT_CONTROL permission check
    Int32 p;
    mContext->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::MEDIA_CONTENT_CONTROL, &p);
    if (IPackageManager::PERMISSION_GRANTED == p) {
        if (DEBUG_RC) { Logger::D(TAG, "ok to register Rcd: has MEDIA_CONTENT_CONTROL permission");}
        return RCD_REG_SUCCESS_PERMISSION;
    }

    // ENABLED_NOTIFICATION_LISTENERS settings check
    if (listenerComp != NULL) {
        // this call is coming from an app, can't use its identity to read secure settings
        Int64 ident = Binder::ClearCallingIdentity();
        // try {
        Int32 currentUser;
        CActivityManager::GetCurrentUser(&currentUser);
        AutoPtr<ContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        String enabledNotifListeners;
        SettingsSecure::GetStringForUser(cr,
                ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS,
                currentUser, &enabledNotifListeners);
        if (enabledNotifListeners != NULL) {
            AutoPtr<ArrayOf<String> > components;
            StringUtils::Split(enabledNotifListeners, String(":"), (ArrayOf<String>**)&components);

            for (Int32 i=0; i<components->GetLength(); i++) {
                AutoPtr<IComponentName> component;
                CComponentName::UnflattenFromString((*components)[i], (IComponentName**)&component);
                if (component != NULL) {
                    if (listenerComp->Equals(component)) {
                        if (DEBUG_RC) { Log.d(TAG, "ok to register RCC: %p
                                is authorized notification listener", component); }
                        return RCD_REG_SUCCESS_ENABLED_NOTIF;
                    }
                }
            }
        }
        if (DEBUG_RC) { Logger::D(TAG, "not ok to register RCD, %p
                is not in list of ENABLED_NOTIFICATION_LISTENERS", listenerComp); }
        // } finally {
        //     Binder.restoreCallingIdentity(ident);
        // }
    }

    return RCD_REG_FAILURE;
}

void MediaFocusControl::PostReevaluateRemoteControlDisplays()
{
    SendMsg(mEventHandler, MSG_REEVALUATE_RCD, SENDMSG_QUEUE, 0, 0, NULL, 0);
}

void MediaFocusControl::OnReevaluateRemoteControlDisplays()
{
    if (DEBUG_RC) { Logger::D(TAG, "onReevaluateRemoteControlDisplays()"); }
    // read which components are enabled notification listeners
    Int32 currentUser;
    CActivityManager::GetCurrentUser(&currentUser);
    AutoPtr<ContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String enabledNotifListeners;
    SettingsSecure::GetStringForUser(cr,
            ISettingsSecure::ENABLED_NOTIFICATION_LISTENERS,
            currentUser, &enabledNotifListeners);
    if (DEBUG_RC) { Logger::D(TAG, " > enabled list: %s", enabledNotifListeners.string()); }
    synchronized(mAudioFocusLock) {
        synchronized(mPRStack) {
            // check whether the "enable" status of each RCD with a notification listener
            // has changed
            AutoPtr<ArrayOf<String> > enabledComponents;
            if (enabledNotifListeners == NULL) {
                enabledComponents = NULL;
            }
            else {
                StringUtils::Split(enabledNotifListeners, String(":"),
                        (ArrayOf<String>**)&enabledComponents);
            }
            AutoPtr<ArrayOf<IInterface*> > array;
            mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                AutoPtr<DisplayInfoForServer> di = (*array)[i];

                if (di->mClientNotifListComp != NULL) {
                    Boolean wasEnabled = di->mEnabled;
                    di->mEnabled = IsComponentInStringArray(di->mClientNotifListComp,
                            enabledComponents);
                    if (wasEnabled != di->mEnabled){
                        // try {
                        // tell the RCD whether it's enabled
                        di->mRcDisplay->SsetEnabled(di->mEnabled);
                        // tell the RCCs about the change for this RCD
                        EnableRemoteControlDisplayForClient_syncRcStack(
                                di->mRcDisplay, di->mEnabled);
                        // when enabling, refresh the information on the display
                        if (di->mEnabled) {
                            SendMsg(mEventHandler, MSG_RCDISPLAY_INIT_INFO, SENDMSG_QUEUE,
                                    di->mArtworkExpectedWidth /*arg1*/,
                                    di->mArtworkExpectedHeight/*arg2*/,
                                    di->mRcDisplay /*obj*/, 0/*delay*/);
                        }
                        // } catch (RemoteException e) {
                        //     Logger::E(TAG, "Error en/disabling RCD: ", e);
                        // }
                    }
                }
            }
        }
    }
}

Boolean MediaFocusControl::IsComponentInStringArray(
    /* [in] */ IComponentName* comp,
    /* [in] */ ArrayOf<String>* enabledArray)
{
    if (enabledArray == NULL || enabledArray->GetLength() == 0) {
        if (DEBUG_RC) { Logger::D(TAG, " > %p is NOT enabled", comp); }
        return FALSE;
    }
    String compString;
    comp->FlattenToString(&compString);
    for (Int32 i = 0; i < enabledArray->GetLength(); i++) {
        if (compString->Equals((*enabledArray)[i])) {
            if (DEBUG_RC) { Logger::D(TAG, " > %s is enabled", compString.string()); }
            return TRUE;
        }
    }
    if (DEBUG_RC) { Logger::D(TAG, " > %s is NOT enabled", compString.string()); }
    return FALSE;
}

void MediaFocusControl::SendMsg(
    /* [in] */ IHandler handler,
    /* [in] */ Int32 msg,
    /* [in] */ Int32 existingMsgPolicy,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 delay)
{
    Boolean b;
    if (existingMsgPolicy == SENDMSG_REPLACE) {
        handler->EemoveMessages(msg);
    }
    else if (existingMsgPolicy == SENDMSG_NOOP && (handler->HasMessages(msg, &b), b)) {
        return;
    }

    AutoPtr<IMessage> msg;
    handler->ObtainMessage(msg, arg1, arg2, obj, (IMessage**)&msg);
    handler->SendMessageDelayed(msg, delay, &b);
}

void MediaFocusControl::NotifyTopOfAudioFocusStack()
{
    Boolean b;
    // notify the top of the stack it gained focus
    if (!(mFocusStack->IsEmpty(&b), b)) {
        if (CanReassignAudioFocus()) {
            AutoPtr<IInterface> obj;
            mFocusStack->Peek((IInterface**)&obj);
            ((FocusRequester*)obj->Get())->HandleFocusGain(IAudioManager::AUDIOFOCUS_GAIN);
        }
    }
}

void MediaFocusControl::PropagateFocusLossFromGain_syncAf(
    /* [in] */ Int32 focusGain)
{
    // going through the audio focus stack to signal new focus, traversing order doesn't
    // matter as all entries respond to the same external focus gain
    AutoPtr<ArrayOf<IInterface*> > array;
    mFocusStack->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IFocusRequester> fs = IFocusRequester::Probe((*array)[i]);
        fs->HandleExternalFocusGain(focusGain);
    }
}

void MediaFocusControl::DumpFocusStack(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println("\nAudio Focus stack entries (last is top of stack):");
    synchronized(mAudioFocusLock) {
        AutoPtr<ArrayOf<IInterface*> > array;
        mFocusStack->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IFocusRequester> fs = IFocusRequester::Probe((*array)[i]);
                fs->Dump(pw);
            }
        }
    }
}

void MediaFocusControl::RemoveFocusStackEntry(
    /* [in] */ const String& clientToRemove,
    /* [in] */ Boolean signal)
{
    Boolean b1, b2;
    mFocusStack-p>IsEmpty(&b1);
    AutoPtr<IInterface> obj;
    mFocusStack->Peek((IInterface**)&obj);
    ((FocusRequester*)obj->Get())->HasSameClient(clientToRemove, &b2);

    // is the current top of the focus stack abandoning focus? (because of request, not death)
    if (!b1 && b2)
    {
        //Log.i(TAG, "   removeFocusStackEntry() removing top of stack");
        AutoPtr<IInterface> obj;
        mFocusStack->Pop((IInterface**)&obj);
        AutoPtr<IFocusRequester> fr = IFocusRequester::Probe(obj);
        fr->ReleaseResources();
        if (signal) {
            // notify the new top of the stack it gained focus
            NotifyTopOfAudioFocusStack();
        }
    }
    else {
        // focus is abandoned by a client that's not at the top of the stack,
        // no need to update focus.
        // (using an iterator on the stack so we can safely remove an entry after having
        //  evaluated it, traversal order doesn't matter here)
        AutoPtr<ArrayOf<IInterface*> > array;
        mFocusStack->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IFocusRequester> fs = IFocusRequester::Probe((*array)[i]);
            Boolean b;
            if((fr->HasSameClient(clientToRemove, &b), b)) {
                Logger::I(TAG, "AudioFocus  removeFocusStackEntry(): removing entry for %s"
                        , clientToRemove.string());
                mFocusStack->Remove(i);
                fr->ReleaseResources();
            }
        }
    }
}

void MediaFocusControl::RemoveFocusStackEntryForClient(
    /* [in] */ IBinder* cb)
{
    // is the owner of the audio focus part of the client to remove?
    AutoPtr<IInterface> obj;
    mFocusStack->Peek((IInterface**)&obj);
    AutoPtr<IFocusRequester> fr = IFocusRequester::Probe(obj);

    Boolean b1, b2;
    mFocusStack->IsEmpty(&b1);
    fr->HasSameBinder(cb, &b2);
    Boolean isTopOfStackForClientToRemove = !b1 && b2;
    // (using an iterator on the stack so we can safely remove an entry after having
    //  evaluated it, traversal order doesn't matter here)
    AutoPtr<ArrayOf<IInterface*> > array;
    mFocusStack->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IFocusRequester> fr = (*array)[i];
        Boolean b;
        if(fr->HasSameBinder(cb, &b), b) {
            Logger::I(TAG, "AudioFocus  removeFocusStackEntry(): removing entry for %p", cb);
            mFocusStack->Remove(i);
            // the client just died, no need to unlink to its death
        }
    }
    if (isTopOfStackForClientToRemove) {
        // we removed an entry at the top of the stack:
        //  notify the new top of the stack it gained focus.
        NotifyTopOfAudioFocusStack();
    }
}

Boolean MediaFocusControl::CanReassignAudioFocus()
{
    // focus requests are rejected during a phone call or when the phone is ringing
    // this is equivalent to IN_VOICE_COMM_FOCUS_ID having the focus
    Boolean b1, b2;
    mFocusStack->IsEmpty(&b1);
    AutoPtr<IInterface> obj;
    mFocusStack->Peek((IInterface**)&obj);
    AutoPtr<IFocusRequester> fr = IFocusRequester::Probe(obj);
    fr->HasSameClient(IN_VOICE_COMM_FOCUS_ID, &b2);
    if (!b1 && b2) {
        return FALSE;
    }
    return TRUE;
}

void MediaFocusControl::FilterMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    // sanity check on the incoming key event
    if (!IsValidMediaKeyEvent(keyEvent)) {
        Logger::E(TAG, "not dispatching invalid media key event %p", keyEvent);
        return;
    }
    // event filtering for telephony
    synchronized(mRingingLock) {
        synchronized(mPRStack) {
            Int32 mode;
            mAudioService->GetMode(&mode);
            if ((mMediaReceiverForCalls != NULL) &&
                    (mIsRinging || (mode == IAudioSystem::MODE_IN_CALL))) {
                DispatchMediaKeyEventForCalls(keyEvent, needWakeLock);
                return;
            }
        }
    }
    // event filtering based on voice-based interactions
    Int32 code;
    keyEvent->GetKeyCode(&code);
    if (IsValidVoiceInputKeyCode(code)) {
        FilterVoiceInputKeyEvent(keyEvent, needWakeLock);
    }
    else {
        DispatchMediaKeyEvent(keyEvent, needWakeLock);
    }
}

void MediaFocusControl::DispatchMediaKeyEventForCalls(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    AutoPtr<IIntent> keyIntent;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, NULL, (IIntent**)&keyIntent);
    keyIntent->PutExtra(IIntent::EXTRA_KEY_EVENT, keyEvent);
    String name;
    mMediaReceiverForCalls->GetPackageName(&name);
    keyIntent->SetPackage(name);
    if (needWakeLock) {
        mMediaEventWakeLock->AcquireLock();
        keyIntent->PutExtra(EXTRA_WAKELOCK_ACQUIRED, WAKELOCK_RELEASE_ON_FINISHED);
    }
    Int64 ident = Binder::ClearCallingIdentity();
    // try {
    mContext->SendOrderedBroadcastAsUser(keyIntent, IUserHandle::ALL,
            NULL, mKeyEventDone, mEventHandler, IActivity::RESULT_OK, NULL, NULL);
    // } finally {
    Binder::RestoreCallingIdentity(ident);
    // }
}

void MediaFocusControl::DispatchMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    if (needWakeLock) {
        mMediaEventWakeLock->AcquireLock();
    }
    AutoPtr<IIntent> keyIntent;
    CIntent::New(IIntent::ACTION_MEDIA_BUTTON, NULL, (IIntent**)&keyIntent);
    keyIntent->PutExtra(IIntent::EXTRA_KEY_EVENT, keyEvent);
    synchronized(mPRStack) {
        Boolean b;
        if (!(mPRStack->IsEmpty(&b), b)) {
            // send the intent that was registered by the client
            // try {
            AutoPtr<IInterface> obj;
            mPRStack->Peek((IInterface**)&obj);
            AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);
            prse->GetMediaButtonIntent()->Send(mContext,
                    needWakeLock ? WAKELOCK_RELEASE_ON_FINISHED : 0 /*code*/,
                    keyIntent, this, mEventHandler);
            // } catch (CanceledException e) {
            //     Logger::E(TAG, "Error sending pending intent " + mPRStack.peek());
            //     e.printStackTrace();
            // }
        }
        else {
            // legacy behavior when nobody registered their media button event receiver
            //    through AudioManager
            if (needWakeLock) {
                keyIntent->PutExtra(EXTRA_WAKELOCK_ACQUIRED, WAKELOCK_RELEASE_ON_FINISHED);
            }
            Int64 ident = Binder::ClearCallingIdentity();
            // try {
            mContext->SendOrderedBroadcastAsUser(keyIntent, IUserHandle::ALL,
                    NULL, mKeyEventDone,
                    mEventHandler, IActivity::RESULT_OK, NULL, NULL);
            // } finally {
            Binder::RestoreCallingIdentity(ident);
            // }
        }
    }
}

void MediaFocusControl::FilterVoiceInputKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [in] */ Boolean needWakeLock)
{
    if (DEBUG_RC) {
        Logger::V(TAG, "voice input key event:%p , needWakeLock=%d", keyEvent, needWakeLock);
    }

    Int32 voiceButtonAction = VOICEBUTTON_ACTION_DISCARD_CURRENT_KEY_PRESS;
    Int32 keyAction;
    keyEvent->GetAction(&keyAction);
    Int32 flag;
    keyEvent->GetFlags(&flag);
    synchronized(mVoiceEventLock) {
        if (keyAction == IKeyEvent::ACTION_DOWN) {
            Int32 count;
            keyEvent->GetRepeatCount(&count);
            if (count == 0) {
                // initial down
                mVoiceButtonDown = TRUE;
                mVoiceButtonHandled = FALSE;
            }
            else if (mVoiceButtonDown && !mVoiceButtonHandled
                    && (flag & IKeyEvent::FLAG_LONG_PRESS) != 0) {
                // long-press, start voice-based interactions
                mVoiceButtonHandled = TRUE;
                voiceButtonAction = VOICEBUTTON_ACTION_START_VOICE_INPUT;
            }
        }
        else if (keyAction == IKeyEvent::ACTION_UP) {
            if (mVoiceButtonDown) {
                // voice button up
                mVoiceButtonDown = FALSE;
                Boolean b;
                if (!mVoiceButtonHandled && !(keyEvent->IsCanceled(&b), b)) {
                    voiceButtonAction = VOICEBUTTON_ACTION_SIMULATE_KEY_PRESS;
                }
            }
        }
    }//synchronized(mVoiceEventLock)

    // take action after media button event filtering for voice-based interactions
    switch (voiceButtonAction) {
        case VOICEBUTTON_ACTION_DISCARD_CURRENT_KEY_PRESS:
            if (DEBUG_RC) Logger::V(TAG, "   ignore key event");
            break;
        case VOICEBUTTON_ACTION_START_VOICE_INPUT:
            if (DEBUG_RC) Logger::V(TAG, "   start voice-based interactions");
            // then start the voice-based interactions
            StartVoiceBasedInteractions(needWakeLock);
            break;
        case VOICEBUTTON_ACTION_SIMULATE_KEY_PRESS:
            if (DEBUG_RC) Logger::V(TAG, "   send simulated key event, wakelock=%d", needWakeLock);
            SendSimulatedMediaButtonEvent(keyEvent, needWakeLock);
            break;
    }
}

void MediaFocusControl::SendSimulatedMediaButtonEvent(
    /* [in] */ IKeyEvent* originalKeyEvent,
    /* [in] */ Boolean needWakeLock)
{
    // send DOWN event
    AutoPtr<IKeyEvent> keyEvent;
    KeyEvent::ChangeAction(originalKeyEvent, IKeyEvent::ACTION_DOWN, (IKeyEvent**)&keyEvent);
    DispatchMediaKeyEvent(keyEvent, needWakeLock);
    // send UP event
    KeyEvent::ChangeAction(originalKeyEvent, IKeyEvent::ACTION_UP, (IKeyEvent**)&keyEvent);
    DispatchMediaKeyEvent(keyEvent, needWakeLock);
}

Boolean MediaFocusControl::IsValidMediaKeyEvent(
    /* [in] */ IKeyEvent* keyEvent)
{
    if (keyEvent == NULL) {
        return FALSE;
    }
    Int32 code;
    keyEvent->GetKeyCode(&code);
    return KeyEvent::IsMediaKey(code);
}

Boolean MediaFocusControl::IsValidVoiceInputKeyCode(
    /* [in] */ Int32 keyCode)
{
    if (keyCode == IKeyEvent::KEYCODE_HEADSETHOOK) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void MediaFocusControl::StartVoiceBasedInteractions(
    /* [in] */ Boolean needWakeLock)
{
    AutoPtr<IIntent> voiceIntent;
    // select which type of search to launch:
    // - screen on and device unlocked: action is ACTION_WEB_SEARCH
    // - device locked or screen off: action is ACTION_VOICE_SEARCH_HANDS_FREE
    //    with EXTRA_SECURE set to TRUE if the device is securely locked
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::POWER_SERVICE, ((IInterface**)service));
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);

    Boolean isLocked;
    mKeyguardManager->IsKeyguardLocked(&isLocked);
    isLocked = mKeyguardManager != NULL && isLocked;

    Boolean b;
    if (!isLocked && (pm->IsScreenOn(&b), b)) {
        CIntent::New(IRecognizerIntent::ACTION_WEB_SEARCH, (IIntent**)&voiceIntent);
        Logger::I(TAG, "voice-based interactions: about to use ACTION_WEB_SEARCH");
    }
    else {
        CIntent::New(IRecognizerIntent::ACTION_VOICE_SEARCH_HANDS_FREE, (IIntent**)&voiceIntent);
        mKeyguardManager->IsKeyguardSecure(&b);
        voiceIntent->PutExtra(IRecognizerIntent::EXTRA_SECURE, isLocked && b);
        Logger::I(TAG, "voice-based interactions: about to use ACTION_VOICE_SEARCH_HANDS_FREE");
    }
    // start the search activity
    if (needWakeLock) {
        mMediaEventWakeLock->AcquireLock();
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    if (voiceIntent != NULL) {
        voiceIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        mContext->StartActivityAsUser(voiceIntent, UserHandle.CURRENT);
    }
    // } catch (ActivityNotFoundException e) {
    //     Logger::W(TAG, "No activity for search: " + e);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    if (needWakeLock) {
        mMediaEventWakeLock->ReleaseLock();
    }
    // }
}

void MediaFocusControl::DumpRCStack(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println("\nRemote Control stack entries (last is top of stack):");
    synchronized(mPRStack) {
        AutoPtr<ArrayOf<IInterface*> > array;
        array = NULL;
        mPRStack->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IPlayerRecord> prse = (*array)[i];
            prse->Dump(pw, TRUE);
        }
    }
}

void MediaFocusControl::DumpRCCStack(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println("\nRemote Control Client stack entries (last is top of stack):");
    synchronized(mPRStack) {
        AutoPtr<ArrayOf<IInterface*> > array;
        array = NULL;
        mPRStack->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IPlayerRecord> prse = (*array)[i];
            prse->Dump(pw, FALSE);
        }
        synchronized(mCurrentRcLock) {
            pw->Println("\nCurrent remote control generation ID = " + mCurrentRcClientGen);
        }
    }
    synchronized(mMainRemote) {
        pw->Println("\nRemote Volume State:");
        pw->Println("  has remote: " + mHasRemotePlayback);
        pw->Println("  is remote active: " + mMainRemoteIsActive);
        pw->Println("  rccId: " + mMainRemote->mRccId);
        pw->Println("  volume handling: "
                + ((mMainRemote->mVolumeHandling == IRemoteControlClient::PLAYBACK_VOLUME_FIXED) ?
                        "PLAYBACK_VOLUME_FIXED(0)" : "PLAYBACK_VOLUME_VARIABLE(1)"));
        pw->Println("  volume: " + mMainRemote->mVolume);
        pw->Println("  volume steps: " + mMainRemote->mVolumeMax);
    }
}

void MediaFocusControl::DumpRCDList(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("\nRemote Control Display list entries:"));
    synchronized(mPRStack) {
        AutoPtr<ArrayOf<IInterface*> > array;
        mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<DisplayInfoForServer> di = (*array)[i];
            pw->Println("  IRCD: " + di->mRcDisplay +
                    "  -- w:" + di->mArtworkExpectedWidth +
                    "  -- h:" + di->mArtworkExpectedHeight +
                    "  -- wantsPosSync:" + di->mWantsPositionSync +
                    "  -- " + (di->mEnabled ? "enabled" : "disabled"));
        }
    }
}

Boolean MediaFocusControl::PushMediaButtonReceiver_syncPrs(
    /* [in] */ IPendingIntent* mediaIntent,
    /* [in] */ IComponentName* target,
    /* [in] */ IBinder* token)
{
    Boolean b;
    if ((mPRStack->IsEmpty(&b), b)) {
        AutoPtr<IPlayerRecord> pr;
        CPlayerRecord::New(mediaIntent, target, token, (IPlayerRecord**)&pr);
        mPRStack->Push(pr);
        return TRUE;
    }
    else if (mPRStack->Peek()->HasMatchingMediaButtonIntent(mediaIntent)) {
        // already at top of stack
        return FALSE;
    }
    if (mAppOps->NoteOp(IAppOpsManager::OP_TAKE_MEDIA_BUTTONS, Binder::GetCallingUid(),
            mediaIntent->GetCreatorPackage()) != IAppOpsManager::MODE_ALLOWED) {
        return FALSE;
    }
    Boolean topChanged = FALSE;
    AutoPtr<IPlayerRecord> prse;
    Int32 size;
    mPRStack->GetSize(&size);
    Int32 lastPlayingIndex = size;
    Int32 inStackIndex = -1;
    AutoPtr<IInterface> obj;
    mPRStack->Get(size-1, (IInterface**)&obj);
    AutoPtr<IPlayerRecord> oldTopPrse = IPlayerRecord::Probe(obj);

    // try {
    // go through the stack from the top to figure out who's playing, and the position
    // of this media button receiver (note that it may not be in the stack)
    for (Int32 index = size-1; index >= 0; index--) {
        obj = NULL;
        mPRStack->Get(index, (IInterface**)&obj);
        prse = IPlayerRecord::Probe(obj);

        if (prse->IsPlaybackActive()) {
            lastPlayingIndex = index;
        }
        if (prse->HasMatchingMediaButtonIntent(mediaIntent)) {
            inStackIndex = index;
        }
    }

    if (inStackIndex == -1) {
        // is not in stack
        CPlayerRecord::New(mediaIntent, target, token, (IPlayerRecord**)&prse);
        // it's new so it's not playing (no RemoteControlClient to give a playstate),
        // therefore it goes after the ones with active playback
        mPRStack->Add(lastPlayingIndex, prse);
    }
    else {
        // is in the stack
        if (size > 1) { // no need to remove and add if stack contains only 1
            obj = NULL;
            mPRStack->Get(inStackIndex, (IInterface**)&obj);
            prse = IPlayerRecord::Probe(obj);
            // remove it from its old location in the stack
            mPRStack->RemoveElementAt(inStackIndex);
            if (prse->IsPlaybackActive()) {
                // and put it at the top
                mPRStack->Push(prse);
            }
            else {
                // and put it after the ones with active playback
                if (inStackIndex > lastPlayingIndex) {
                    mPRStack->Add(lastPlayingIndex, prse);
                }
                else {
                    mPRStack->Add(lastPlayingIndex - 1, prse);
                }
            }
        }
    }

    // } catch (ArrayIndexOutOfBoundsException e) {
    //     // not expected to happen, indicates improper concurrent modification or bad index
    //     Logger::E(TAG, "Wrong index (inStack=" + inStackIndex + " lastPlaying=" + lastPlayingIndex
    //             + " size=" + mPRStack.size()
    //             + " accessing media button stack", e);
    // }

    return (topChanged);
}

void MediaFocusControl::RemoveMediaButtonReceiver_syncPrs(
    /* [in] */ IPendingIntent* pi)
{
    // try {
    Int32 size;
    mPRStack->GetSize(&size);
    for (Int32 index = size-1; index >= 0; index--) {
        AutoPtr<IInterface> obj;
        mPRStack->Get(index, (IInterface**)&obj);
        AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);

        Boolean b;
        if ((prse->HasMatchingMediaButtonIntent(pi, &b), b)) {
            prse->Destroy();
            // ok to remove element while traversing the stack since we're leaving the loop
            mPRStack->Remove(index);
            break;
        }
    }
    // } catch (ArrayIndexOutOfBoundsException e) {
    //     // not expected to happen, indicates improper concurrent modification
    //     Logger::E(TAG, "Wrong index accessing media button stack, lock error? ", e);
    // }
}

Boolean MediaFocusControl::IsCurrentRcController(
    /* [in] */ IPendingIntent* pi)
{
    Boolean b1;
    mPRStack->IsEmpty(&b1);
    AutoPtr<IInterface> obj;
    mPRStack->Peek((IInterface**)&obj);
    AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);
    Boolean b2;
    prse->HasMatchingMediaButtonIntent(pi, &b2);
    if (!b1 && b2) {
        return TRUE;
    }
    return FALSE;
}

void MediaFocusControl::SetNewRcClientOnDisplays_syncRcsCurrc(
    /* [in] */ Int32 newClientGeneration,
    /* [in] */ IPendingIntent* newMediaIntent,
    /* [in] */ Boolean clearing)
{
    synchronized(mPRStack) {
        Int32 size;
        mRcDisplays->GetSize(&size);
        if (size > 0) {
            AutoPtr<ArrayOf<IInterface*> > array;
            mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                AutoPtr<DisplayInfoForServer> di = (*array)[i];
                // try {
                di->mRcDisplay->SetCurrentClientId(
                        newClientGeneration, newMediaIntent, clearing);
                // } catch (RemoteException e) {
                //     Logger::E(TAG, "Dead display in setNewRcClientOnDisplays_syncRcsCurrc()",e);
                //     di.release();
                //     displayIterator.remove();
                // }
            }
        }
    }
}

void MediaFocusControl::SetNewRcClientGenerationOnClients_syncRcsCurrc(
    /* [in] */ Int32 newClientGeneration)
{
    // (using an iterator on the stack so we can safely remove an entry if needed,
    //  traversal order doesn't matter here as we update all entries)
    AutoPtr<ArrayOf<IInterface*> > array;
    array = NULL;
    mPRStack->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IPlayerRecord> se = (*array)[i];
        if ((se != NULL) && (se->GetRcc() != NULL)) {
            // try {
            se->GetRcc()->SetCurrentClientGenerationId(newClientGeneration);
            // } catch (RemoteException e) {
            //     Logger::W(TAG, "Dead client in setNewRcClientGenerationOnClients_syncRcsCurrc()",e);
            //     stackIterator.remove();
            //     se.unlinkToRcClientDeath();
            // }
        }
    }
}

void MediaFocusControl::SetNewRcClient_syncRcsCurrc(
    /* [in] */ Int32 newClientGeneration,
    /* [in] */ IPendingIntent* newMediaIntent,
    /* [in] */ Boolean clearing)
{
    // send the new valid client generation ID to all displays
    SetNewRcClientOnDisplays_syncRcsCurrc(newClientGeneration, newMediaIntent, clearing);
    // send the new valid client generation ID to all clients
    SetNewRcClientGenerationOnClients_syncRcsCurrc(newClientGeneration);
}

void MediaFocusControl::OnRcDisplayClear()
{
    if (DEBUG_RC) Logger::I(TAG, "Clear remote control display");

    synchronized(mPRStack) {
        synchronized(mCurrentRcLock) {
            mCurrentRcClientGen++;
            // synchronously update the displays and clients with the new client generation
            SetNewRcClient_syncRcsCurrc(mCurrentRcClientGen,
                    NULL /*newMediaIntent*/, TRUE /*clearing*/);
        }
    }
}

void MediaFocusControl::OnRcDisplayUpdate(
    /* [in] */ IPlayerRecord* prse,
    /* [in] */ Int32 flags /* USED ?*/)
{
    synchronized(mPRStack) {
        synchronized(mCurrentRcLock) {
            if ((mCurrentRcClient != NULL) && (mCurrentRcClient->Equals(prse->GetRcc()))) {
                if (DEBUG_RC) Logger::I(TAG, "Display/update remote control ");

                mCurrentRcClientGen++;
                // synchronously update the displays and clients with
                //      the new client generation
                SetNewRcClient_syncRcsCurrc(mCurrentRcClientGen,
                        prse->GetMediaButtonIntent() /*newMediaIntent*/,
                        FALSE /*clearing*/);

                // tell the current client that it needs to send info
                // try {
                //TODO change name to informationRequestForAllDisplays()
                mCurrentRcClient->OnInformationRequested(mCurrentRcClientGen, flags);
                // } catch (RemoteException e) {
                //     Logger::E(TAG, "Current valid remote client is dead: "+e);
                //     mCurrentRcClient = NULL;
                // }
            }
            else {
                // the remote control display owner has changed between the
                // the message to update the display was sent, and the time it
                // gets to be processed (now)
            }
        }
    }
}

void MediaFocusControl::OnRcDisplayInitInfo(
    /* [in] */ IIRemoteControlDisplay* newRcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    synchronized(mPRStack) {
        synchronized(mCurrentRcLock) {
            if (mCurrentRcClient != NULL) {
                if (DEBUG_RC) { Logger::I(TAG, "Init RCD with current info"); }
                // try {
                // synchronously update the new RCD with the current client generation
                // and matching PendingIntent
                newRcd->SetCurrentClientId(mCurrentRcClientGen, mCurrentRcClientIntent,
                        FALSE);

                // tell the current RCC that it needs to send info, but only to the new RCD
                // try {
                mCurrentRcClient->InformationRequestForDisplay(newRcd, w, h);
                // } catch (RemoteException e) {
                //     Logger::E(TAG, "Current valid remote client is dead: ", e);
                //     mCurrentRcClient = NULL;
                // }
                // } catch (RemoteException e) {
                //     Logger::E(TAG, "Dead display in onRcDisplayInitInfo()", e);
                // }
            }
        }
    }
}

void MediaFocusControl::ClearRemoteControlDisplay_syncPrs()
{
    synchronized(mCurrentRcLock) {
        mCurrentRcClient = NULL;
    }
    // will cause onRcDisplayClear() to be called in AudioService's handler thread
    AutoPtr<IMessage> msg;
    mEventHandler->ObtainMessage(MSG_RCDISPLAY_CLEAR, (IMessage**)&msg);
    Boolean b;
    mEventHandler->SendMessage(msg, &b);
}

void MediaFocusControl::UpdateRemoteControlDisplay_syncPrs(
    /* [in] */ Int32 infoChangedFlags)
{
    AutoPtr<IInterface> obj;
    mPRStack->Peek((IInterface**)&obj);
    AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);
    Int32 infoFlagsAboutToBeUsed = infoChangedFlags;
    // this is where we enforce opt-in for information display on the remote controls
    //   with the new AudioManager.registerRemoteControlClient() API
    if (prse->GetRcc() == NULL) {
        //Logger::W(TAG, "Can't update remote control display with NULL remote control client");
        ClearRemoteControlDisplay_syncPrs();
        return;
    }
    synchronized(mCurrentRcLock) {
        if (!prse->GetRcc()->Equals(mCurrentRcClient)) {
            // new RC client, assume every type of information shall be queried
            infoFlagsAboutToBeUsed = RC_INFO_ALL;
        }
        mCurrentRcClient = prse->GetRcc();
        mCurrentRcClientIntent = prse->GetMediaButtonIntent();
    }
    // will cause onRcDisplayUpdate() to be called in AudioService's handler thread
    AutoPtr<IMessage> msg;
    mEventHandler->ObtainMessage(MSG_RCDISPLAY_UPDATE,
            infoFlagsAboutToBeUsed /* arg1 */, 0, prse /* obj, != NULL */, (IMessage**)&msg);
    Boolean b;
    mEventHandler->SendMessage(msg, &b);
}

void MediaFocusControl::CheckUpdateRemoteControlDisplay_syncPrs(
    /* [in] */ Int32 infoChangedFlags)
{
    Boolean b;
    // determine whether the remote control display should be refreshed
    // if the player record stack is empty, there is nothing to display, so clear the RC display
    if ((mPRStack->IsEmpty(&b), b)) {
        ClearRemoteControlDisplay_syncPrs();
        return;
    }

    // this is where more rules for refresh go

    // refresh conditions were verified: update the remote controls
    // ok to call: synchronized on mPRStack, mPRStack is not empty
    UpdateRemoteControlDisplay_syncPrs(infoChangedFlags);
}

void MediaFocusControl::PlugRemoteControlDisplaysIntoClient_syncPrs(
    /* [in] */ IIRemoteControlClient* rcc)
{
    AutoPtr<ArrayOf<IInterface*> > array;
    mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<DisplayInfoForServer> di = (*array)[i];

        // try {
        rcc->PlugRemoteControlDisplay(di->mRcDisplay, di->mArtworkExpectedWidth,
                di->mArtworkExpectedHeight);
        if (di->mWantsPositionSync) {
            rcc->SetWantsSyncForDisplay(di->mRcDisplay, TRUE);
        }
        // } catch (RemoteException e) {
        //     Logger::E(TAG, "Error connecting RCD to RCC in RCC registration",e);
        // }
    }
}

void MediaFocusControl::EnableRemoteControlDisplayForClient_syncRcStack(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Boolean enabled)
{
    // let all the remote control clients know whether the given display is enabled
    //   (so the remote control stack traversal order doesn't matter).
    AutoPtr<ArrayOf<IInterface*> > array;
    mPRStack->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IPlayerRecord> prse = (*array)[i];

        if(prse->GetRcc() != NULL) {
            // try {
            prse->GetRcc()->EnableRemoteControlDisplay(rcd, enabled);
            // } catch (RemoteException e) {
            //     Logger::E(TAG, "Error connecting RCD to client: ", e);
            // }
        }
    }
}

Boolean MediaFocusControl::RcDisplayIsPluggedIn_syncRcStack(
    /* [in] */ IIRemoteControlDisplay* rcd)
{
    AutoPtr<ArrayOf<IInterface*> > array;
    mRcDisplays->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<DisplayInfoForServer> di = (*array)[i];

        if (IBinder::Probe(di->mRcDisplay)->Equals(IBinder::Probe(rcd))) {
            return TRUE;
        }
    }
    return FALSE;
}

void MediaFocusControl::RegisterRemoteControlDisplay_int(
    /* [in] */ IIRemoteControlDisplay* rcd,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ IComponentName* listenerComp)
{
    if (DEBUG_RC) Logger::D(TAG, ">>> registerRemoteControlDisplay("+rcd+")");
    synchronized(mAudioFocusLock) {
        synchronized(mPRStack) {
            if ((rcd == NULL) || RcDisplayIsPluggedIn_syncRcStack(rcd)) {
                return;
            }
            AutoPtr<DisplayInfoForServer> di = new DisplayInfoForServer(rcd, w, h, this);
            di->mEnabled = TRUE;
            di->mClientNotifListComp = listenerComp;
            Boolean b;
            if (!(di->Init(&b), b)) {
                if (DEBUG_RC) Logger::E(TAG, " error registering RCD");
                return;
            }
            // add RCD to list of displays
            mRcDisplays->Add(di);

            // let all the remote control clients know there is a new display (so the remote
            //   control stack traversal order doesn't matter).
            AutoPtr<ArrayOf<IInterface*> > array;
            mPRStack->ToArray((ArrayOf<IInterface*>**)&array);
            for (Int32 i = 0; i < array->GetLength(); i++) {
                AutoPtr<IPlayerRecord> prse = (*array)[i];

                if(prse->GetRcc() != NULL) {
                    // try {
                    prse->GetRcc()->PlugRemoteControlDisplay(rcd, w, h);
                    // } catch (RemoteException e) {
                    //     Logger::E(TAG, "Error connecting RCD to client: ", e);
                    // }
                }
            }

            // we have a new display, of which all the clients are now aware: have it be
            // initialized wih the current gen ID and the current client info, do not
            // reset the information for the other (existing) displays
            SendMsg(mEventHandler, MSG_RCDISPLAY_INIT_INFO, SENDMSG_QUEUE,
                    w /*arg1*/, h /*arg2*/,
                    rcd /*obj*/, 0/*delay*/);
        }
    }
}

void MediaFocusControl::OnRegisterVolumeObserverForRcc(
    /* [in] */ Int32 rccId,
    /* [in] */ IIRemoteVolumeObserver* rvo)
{
    synchronized(mPRStack) {
        // The stack traversal order doesn't matter because there is only one stack entry
        //  with this RCC ID, but the matching ID is more likely at the top of the stack, so
        //  start iterating from the top.
        // try {
        Int32 size;
        mPRStack->GetSize(&size);
        for (Int32 index = size - 1; index >= 0; index--) {
            AutoPtr<IInterface> obj;
            mPRStack->Get(index, ((IInterface**)&obj);
            AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);

            if (prse->GetRccId() == rccId) {
                prse->mRemoteVolumeObs = rvo;
                break;
            }
        }
        // } catch (ArrayIndexOutOfBoundsException e) {
        //     // not expected to happen, indicates improper concurrent modification
        //     Logger::E(TAG, "Wrong index accessing media button stack, lock error? ", e);
        // }
    }
}

void MediaFocusControl::SendVolumeUpdateToRemote(
    /* [in] */ Int32 rccId,
    /* [in] */ Int32 direction)
{
    if (DEBUG_VOL) { Logger::D(TAG, "sendVolumeUpdateToRemote(rccId=%d , dir=%d", rccId, direction); }
    if (direction == 0) {
        // only handling discrete events
        return;
    }
    AutoPtr<IIRemoteVolumeObserver> rvo;
    synchronized(mPRStack) {
        // The stack traversal order doesn't matter because there is only one stack entry
        //  with this RCC ID, but the matching ID is more likely at the top of the stack, so
        //  start iterating from the top.
        // try {
        Int32 size;
        mPRStack->GetSize(&size);
        for (Int32 index = size - 1; index >= 0; index--) {
            AutoPtr<IInterface> obj;
            mPRStack->Get(index, ((IInterface**)&obj);
            AutoPtr<IPlayerRecord> prse = IPlayerRecord::Probe(obj);
            //FIXME OPTIMIZE store this info in mMainRemote so we don't have to iterate?
            if (prse->GetRccId() == rccId) {
                rvo = prse->mRemoteVolumeObs;
                break;
            }
        }
        // } catch (ArrayIndexOutOfBoundsException e) {
        //     // not expected to happen, indicates improper concurrent modification
        //     Logger::E(TAG, "Wrong index accessing media button stack, lock error? ", e);
        // }
    }
    if (rvo != NULL) {
        // try {
        rvo->DispatchRemoteVolumeUpdate(direction, -1);
        // } catch (RemoteException e) {
        //     Logger::E(TAG, "Error dispatching relative volume update", e);
        // }
    }
}

void MediaFocusControl::OnReevaluateRemote()
{
    // TODO This was used to notify VolumePanel if there was remote playback
    // in the stack. This is now in MediaSessionService. More code should be
    // removed.
    return;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
