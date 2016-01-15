
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/internal/policy/impl/PhoneFallbackEventHandler.h"
#include "elastos/droid/internal/policy/impl/CPhoneWindowManager.h"
#include "elastos/droid/media/session/CMediaSessionLegacyHelperHelper.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Media::IIAudioService;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelper;
using Elastos::Droid::Media::Session::IMediaSessionLegacyHelperHelper;
using Elastos::Droid::Media::Session::CMediaSessionLegacyHelperHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::View::EIID_IFallbackEventHandler;
using Elastos::Droid::View::IDispatcherState;
using Elastos::Droid::View::IHapticFeedbackConstants;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//                      PhoneFallbackEventHandler
//=====================================================================
CAR_INTERFACE_IMPL_2(PhoneFallbackEventHandler, Object, IPhoneFallbackEventHandler, IFallbackEventHandler)

String PhoneFallbackEventHandler::TAG("PhoneFallbackEventHandler");
const Boolean PhoneFallbackEventHandler::DEBUG = FALSE;

PhoneFallbackEventHandler::PhoneFallbackEventHandler()
{
}

ECode PhoneFallbackEventHandler::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode PhoneFallbackEventHandler::SetView(
    /* [in] */ IView *v)
{
    mView = v;
    return NOERROR;
}

ECode PhoneFallbackEventHandler::PreDispatchKeyEvent(
    /* [in] */ IKeyEvent *event)
{
    AutoPtr<IAudioManager> am;
    GetAudioManager((IAudioManager**)&am);
    return am->PreDispatchKeyEvent(event, IAudioManager::USE_DEFAULT_STREAM_TYPE);
}

ECode PhoneFallbackEventHandler::DispatchKeyEvent(
    /* [in] */ IKeyEvent *event,
    /* [out] */ Boolean *res)
{
    VALIDATE_NOT_NULL(res)
    Int32 action, keyCode;
    event->GetAction(&action);
    event->GetKeyCode(&keyCode);

    if (IKeyEvent::ACTION_DOWN == action) {
        return OnKeyDown(keyCode, event, res);
    }
    else {
        return OnKeyUp(keyCode, event, res);
    }
}

ECode PhoneFallbackEventHandler::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent *event,
    /* [out] */ Boolean *res)
{
    /* ****************************************************************************
     * HOW TO DECIDE WHERE YOUR KEY HANDLING GOES.
     * See the comment in PhoneWindow.onKeyDown
     * ****************************************************************************/
    VALIDATE_NOT_NULL(res)
    AutoPtr<IDispatcherState> dispatcher;
    mView->GetKeyDispatcherState((IDispatcherState**)&dispatcher);

    switch (keyCode) {
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_VOLUME_MUTE:
        {
            AutoPtr<IMediaSessionLegacyHelperHelper> mslHelperHelper;
            CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mslHelperHelper);
            AutoPtr<IMediaSessionLegacyHelper> mslHelper;
            mslHelperHelper->GetHelper(mContext, (IMediaSessionLegacyHelper**)&mslHelper);
            mslHelper->SendVolumeKeyEvent(event, FALSE);
            *res = TRUE;
            return NOERROR;
        }

        case IKeyEvent::KEYCODE_MEDIA_PLAY:
        case IKeyEvent::KEYCODE_MEDIA_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
            /* Suppress PLAY/PAUSE toggle when phone is ringing or in-call
             * to avoid music playback */
        {
            AutoPtr<ITelephonyManager> tm;
            GetTelephonyManager((ITelephonyManager**)&tm);
            if (tm) {
                Int32 state = 0;
                assert(state);//TODO remove
                //TODO begin ITelephoneManager is not ok
                //tm->GetCallState(&state);
                //if (ITelephonyManager::CALL_STATE_IDLE == state) {
                //    *res = TRUE;  // suppress key event
                //    return NOERROR;
                //}
                //TODO end
            }
        }

        case IKeyEvent::KEYCODE_MUTE:
        case IKeyEvent::KEYCODE_HEADSETHOOK:
        case IKeyEvent::KEYCODE_MEDIA_STOP:
        case IKeyEvent::KEYCODE_MEDIA_NEXT:
        case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
        case IKeyEvent::KEYCODE_MEDIA_REWIND:
        case IKeyEvent::KEYCODE_MEDIA_RECORD:
        case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
        case IKeyEvent::KEYCODE_MEDIA_AUDIO_TRACK:
        {
            HandleMediaKeyEvent(event);
            *res = TRUE;
            return NOERROR;
        }

        case IKeyEvent::KEYCODE_CALL:
        {
            AutoPtr<IKeyguardManager> km;
            GetKeyguardManager((IKeyguardManager**)&km);
            Boolean isInputMode;
            km->InKeyguardRestrictedInputMode(&isInputMode);
            if (isInputMode || dispatcher == NULL) {
                break;
            }

            Int32 rc;
            event->GetRepeatCount(&rc);
            if (0 == rc) {
                dispatcher->StartTracking(event, THIS_PROBE(IInterface));
            }
            else {
                Boolean isLongPress, isTracking;
                event->IsLongPress(&isLongPress);
                dispatcher->IsTracking(event, &isTracking);
                if (isLongPress && isTracking) {
                    dispatcher->PerformedLongPress(event);
                    Boolean result;
                    mView->PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &result);
                    // launch the VoiceDialer
                    AutoPtr<IIntent> intent;
                    CIntent::New(IIntent::ACTION_VOICE_COMMAND, (IIntent**)&intent);
                    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
                    ECode ec;
                    FAIL_GOTO(ec = SendCloseSystemWindows(), ERROR_PROCESS)
                    FAIL_GOTO(ec = mContext->StartActivity(intent), ERROR_PROCESS)
ERROR_PROCESS:
                    if (FAILED(ec)) {
                        if (E_ACTIVITY_NOT_FOUND_EXCEPTION == (UInt32)ec) {
                            StartCallActivity();
                        }
                        else {
                            return ec;
                        }
                    }
                }
            }
            *res = TRUE;
            return NOERROR;
        }

        case IKeyEvent::KEYCODE_CAMERA:
        {
            AutoPtr<IKeyguardManager> km;
            GetKeyguardManager((IKeyguardManager**)&km);
            Boolean isInputMode;
            km->InKeyguardRestrictedInputMode(&isInputMode);
            if (isInputMode || dispatcher == NULL) {
                break;
            }
            Int32 rc;
            event->GetRepeatCount(&rc);
            if (0 == rc) {
                dispatcher->StartTracking(event, THIS_PROBE(IInterface));
            }
            else {
                Boolean isLongPress, isTracking;
                event->IsLongPress(&isLongPress);
                dispatcher->IsTracking(event, &isTracking);
                if (isLongPress && isTracking) {
                    dispatcher->PerformedLongPress(event);
                    Boolean result;
                    mView->PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &result);
                    SendCloseSystemWindows();
                    // Broadcast an intent that the Camera button was longpressed
                    AutoPtr<IIntent> intent;
                    CIntent::New(IIntent::ACTION_CAMERA_BUTTON, NULL, (IIntent**)&intent);
                    intent->PutExtra(IIntent::EXTRA_KEY_EVENT, IParcelable::Probe(event));
                    mContext->SendOrderedBroadcastAsUser(intent, UserHandle::CURRENT_OR_SELF,
                        String(), NULL, NULL, 0, String(), NULL);
                }
            }
            *res = TRUE;
            return NOERROR;
        }

        case IKeyEvent::KEYCODE_SEARCH:
        {
            AutoPtr<IKeyguardManager> km;
            GetKeyguardManager((IKeyguardManager**)&km);
            Boolean isInputMode;
            km->InKeyguardRestrictedInputMode(&isInputMode);
            if (isInputMode || dispatcher == NULL) {
                break;
            }
            Int32 rc;
            event->GetRepeatCount(&rc);
            if (0 == rc) {
                dispatcher->StartTracking(event, THIS_PROBE(IInterface));
            }
            else {
                Boolean isLongPress, isTracking;
                event->IsLongPress(&isLongPress);
                dispatcher->IsTracking(event, &isTracking);
                if (isLongPress && isTracking) {
                    AutoPtr<IConfiguration> config;
                    AutoPtr<IResources> rs;
                    mContext->GetResources((IResources**)&rs);
                    rs->GetConfiguration((IConfiguration**)&config);
                    Int32 keyBoard, hardKeyboardHidden;
                    config->GetKeyboard(&keyBoard);
                    config->GetHardKeyboardHidden(&hardKeyboardHidden);
                    if (IConfiguration::KEYBOARD_NOKEYS == keyBoard ||
                        IConfiguration::HARDKEYBOARDHIDDEN_YES == hardKeyboardHidden) {
                        // launch the search activity
                        AutoPtr<IIntent> intent;
                        CIntent::New(IIntent::ACTION_SEARCH_LONG_PRESS, (IIntent**)&intent);
                        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
                        Boolean result;
                        mView->PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, &result);
                        SendCloseSystemWindows();
                        AutoPtr<ISearchManager> sm;
                        GetSearchManager((ISearchManager**)&sm);
                        sm->StopSearch();
                        ECode ec;
                        FAIL_GOTO(ec = mContext->StartActivity(intent), ERROR_PROCESS_EX)
                        // Only clear this if we successfully start the
                        // activity; otherwise we will allow the normal short
                        // press action to be performed.
                        dispatcher->PerformedLongPress(event);
                        *res = TRUE;
                        return NOERROR;
ERROR_PROCESS_EX:
                        if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
                            // Ignore
                        }
                        else {
                            return ec;
                        }
                    }
                }
            }
            break;
        }
    } // end switch
    *res = FALSE;
    return NOERROR;
}

ECode PhoneFallbackEventHandler::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent *event,
    /* [out] */ Boolean *res)
{
    VALIDATE_NOT_NULL(res)
    if (DEBUG) {
        String str("up ");
        str.AppendFormat("%d", keyCode);
        Slogger::D(TAG, str.string());
    }
    AutoPtr<IDispatcherState> dispatcher;
    mView->GetKeyDispatcherState((IDispatcherState**)&dispatcher);
    if (dispatcher != NULL) {
        dispatcher->HandleUpEvent(event);
    }

    switch (keyCode) {
        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_VOLUME_MUTE:
        {
            Boolean isCanceled;
            event->IsCanceled(&isCanceled);
            if (!isCanceled) {
                AutoPtr<IMediaSessionLegacyHelperHelper> mslHelperHelper;
                CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mslHelperHelper);
                AutoPtr<IMediaSessionLegacyHelper> mslHelper;
                mslHelperHelper->GetHelper(mContext, (IMediaSessionLegacyHelper**)&mslHelper);
                mslHelper->SendVolumeKeyEvent(event, FALSE);
            }
            *res = TRUE;
            return NOERROR;
        }

        case IKeyEvent::KEYCODE_HEADSETHOOK:
        case IKeyEvent::KEYCODE_MUTE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY:
        case IKeyEvent::KEYCODE_MEDIA_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE:
        case IKeyEvent::KEYCODE_MEDIA_STOP:
        case IKeyEvent::KEYCODE_MEDIA_NEXT:
        case IKeyEvent::KEYCODE_MEDIA_PREVIOUS:
        case IKeyEvent::KEYCODE_MEDIA_REWIND:
        case IKeyEvent::KEYCODE_MEDIA_RECORD:
        case IKeyEvent::KEYCODE_MEDIA_FAST_FORWARD:
        case IKeyEvent::KEYCODE_MEDIA_AUDIO_TRACK:
        {
            HandleMediaKeyEvent(event);
            *res = TRUE;
            return NOERROR;
        }

        case IKeyEvent::KEYCODE_CAMERA:
        {
            AutoPtr<IKeyguardManager> km;
            GetKeyguardManager((IKeyguardManager**)&km);
            Boolean isInputMode;
            km->InKeyguardRestrictedInputMode(&isInputMode);
            if (isInputMode) {
                break;
            }

            Boolean isTracking, isCanceled;
            event->IsTracking(&isTracking);
            event->IsCanceled(&isCanceled);
            if (isTracking && !isCanceled) {
                // Add short press behavior here if desired
            }
            *res = TRUE;
            return NOERROR;
        }

        case IKeyEvent::KEYCODE_CALL:
        {
            AutoPtr<IKeyguardManager> km;
            GetKeyguardManager((IKeyguardManager**)&km);
            Boolean isInputMode;
            km->InKeyguardRestrictedInputMode(&isInputMode);

            if (isInputMode) {
                break;
            }
            Boolean isTracking, isCanceled;
            event->IsTracking(&isTracking);
            event->IsCanceled(&isCanceled);
            if (isTracking && !isCanceled) {
                StartCallActivity();
            }
            *res = TRUE;
            return NOERROR;
        }
    } // end switch
    *res = FALSE;
    return NOERROR;
}

ECode PhoneFallbackEventHandler::StartCallActivity()
{
    SendCloseSystemWindows();
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_CALL_BUTTON, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    ECode ec;
    FAIL_GOTO(ec = mContext->StartActivity(intent), ERROR_PROCESS)
ERROR_PROCESS:
    if (FAILED(ec)) {
        if (E_ACTIVITY_NOT_FOUND_EXCEPTION == (UInt32)ec)
        {
            Slogger::W(TAG, "No activity found for android.intent.action.CALL_BUTTON.");
            ec = NOERROR;
        }
    }
    return ec;
}

ECode PhoneFallbackEventHandler::GetSearchManager(
    /* [out] */ ISearchManager **sm)

{
    VALIDATE_NOT_NULL(sm)
    if (mSearchManager == NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::SEARCH_SERVICE, (IInterface**)&service);
        mSearchManager = ISearchManager::Probe(service);
    }
    *sm = mSearchManager;
    REFCOUNT_ADD(*sm)
    return NOERROR;
}

ECode PhoneFallbackEventHandler::GetTelephonyManager(
    /* [out] */ ITelephonyManager **tm)
{
    VALIDATE_NOT_NULL(tm)
    if (mTelephonyManager == NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
        mTelephonyManager = ITelephonyManager::Probe(service);
    }
    *tm = mTelephonyManager;
    REFCOUNT_ADD(*tm)
    return NOERROR;
}

ECode PhoneFallbackEventHandler::GetKeyguardManager(
    /* [out] */ IKeyguardManager **km)
{
    VALIDATE_NOT_NULL(km)
    if (mKeyguardManager == NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&service);
        mKeyguardManager = IKeyguardManager::Probe(service);
    }
    *km = mKeyguardManager;
    REFCOUNT_ADD(*km)
    return NOERROR;
}

ECode PhoneFallbackEventHandler::GetAudioManager(
    /* [out] */ IAudioManager **am)
{
    VALIDATE_NOT_NULL(am)
    if (mAudioManager == NULL) {
        AutoPtr<IInterface> service;
        mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&service);
        mAudioManager = IAudioManager::Probe(service);
    }
    *am = mAudioManager;
    REFCOUNT_ADD(*am)
    return NOERROR;
}

ECode PhoneFallbackEventHandler::SendCloseSystemWindows()
{
    CPhoneWindowManager::SendCloseSystemWindows(mContext, String());
    return NOERROR;
}

ECode PhoneFallbackEventHandler::HandleMediaKeyEvent(
        /* [in] */ IKeyEvent *keyEvent)
{
    AutoPtr<IMediaSessionLegacyHelperHelper> mslHelperHelper;
    CMediaSessionLegacyHelperHelper::AcquireSingleton((IMediaSessionLegacyHelperHelper**)&mslHelperHelper);
    AutoPtr<IMediaSessionLegacyHelper> mslHelper;
    mslHelperHelper->GetHelper(mContext, (IMediaSessionLegacyHelper**)&mslHelper);
    mslHelper->SendMediaButtonEvent(keyEvent, FALSE);
    return NOERROR;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

