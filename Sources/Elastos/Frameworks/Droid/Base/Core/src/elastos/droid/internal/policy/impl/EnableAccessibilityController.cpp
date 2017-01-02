//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/policy/impl/EnableAccessibilityController.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/CActivityManager.h"
#include "elastos/droid/content/CComponentName.h"
//TODO #include "elastos/droid/media/CRingtoneManager.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/provider/CSettingsSystem.h"
//TODO #include "elastos/droid/speech/tts/CTextToSpeech.h"
#include "elastos/droid/view/accessibility/CAccessibilityManagerHelper.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/core/Math.h"

using Elastos::Droid::App::CActivityManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::IResources;
//TODO using Elastos::Droid::Media::CRingtoneManager;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettingsSystem;
//TODO using Elastos::Droid::Speech::Tts::CTextToSpeech;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Speech::Tts::EIID_ITextToSpeechOnInitListener;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

//=====================================================================
//             EnableAccessibilityController::SpeakHandler
//=====================================================================

EnableAccessibilityController::SpeakHandler::SpeakHandler(
    /* [in] */ EnableAccessibilityController* host)
    : mHost(host)
{
}

ECode EnableAccessibilityController::SpeakHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case EnableAccessibilityController::MESSAGE_SPEAK_WARNING:
            {
                String text;
                mHost->mContext->GetString(R::string::continue_to_enable_accessibility, &text);
                Int32 result;
                mHost->mTts->Speak(text, ITextToSpeech::QUEUE_FLUSH, NULL, &result);
            }
            break;
        case EnableAccessibilityController::MESSAGE_SPEAK_ENABLE_CANCELED:
            {
                String text;
                mHost->mContext->GetString(R::string::enable_accessibility_canceled, &text);
                Int32 result;
                mHost->mTts->Speak(text, ITextToSpeech::QUEUE_FLUSH, NULL, &result);
            }
            break;
        case EnableAccessibilityController::MESSAGE_ENABLE_ACCESSIBILITY:
            {
                mHost->EnableAccessibility();
                mHost->mTone->Play();
                String text;
                mHost->mContext->GetString(R::string::accessibility_enabled, &text);
                Int32 result;
                mHost->mTts->Speak(text, ITextToSpeech::QUEUE_FLUSH, NULL, &result);
            }
            break;
    }
    return NOERROR;
}

//=====================================================================
//   EnableAccessibilityController::TtsShutdownOnInitListener
//=====================================================================
CAR_INTERFACE_IMPL(EnableAccessibilityController::TtsShutdownOnInitListener, Object, ITextToSpeechOnInitListener)

EnableAccessibilityController::TtsShutdownOnInitListener::TtsShutdownOnInitListener(
    /* [in] */ EnableAccessibilityController* host)
    : mHost(host)
{
}

ECode EnableAccessibilityController::TtsShutdownOnInitListener::OnInit(
    /* [in] */ Int32 status)
{
    if (mHost->mDestroyed) {
        mHost->mTts->Shutdown();
    }
    return NOERROR;
}
//=====================================================================
//                    EnableAccessibilityController
//=====================================================================
CAR_INTERFACE_IMPL(EnableAccessibilityController, Object, IEnableAccessibilityController)

const Int32 EnableAccessibilityController::SPEAK_WARNING_DELAY_MILLIS;
const Int32 EnableAccessibilityController::ENABLE_ACCESSIBILITY_DELAY_MILLIS;

const Int32 EnableAccessibilityController::MESSAGE_SPEAK_WARNING;
const Int32 EnableAccessibilityController::MESSAGE_SPEAK_ENABLE_CANCELED;
const Int32 EnableAccessibilityController::MESSAGE_ENABLE_ACCESSIBILITY;

EnableAccessibilityController::EnableAccessibilityController()
{
}

ECode EnableAccessibilityController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IRunnable* onAccessibilityEnabledCallback)
{
    mContext = context;
    mOnAccessibilityEnabledCallback = onAccessibilityEnabledCallback;
    mDestroyed = FALSE;
    mCanceled = FALSE;
    mFirstPointerDownX = 0.0;
    mFirstPointerDownY = 0.0;
    mSecondPointerDownX = 0.0;
    mSecondPointerDownY = 0.0;

    // Init global variable.
    mHandler = new SpeakHandler(this);
    mHandler->constructor();

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> service;
    serviceManager->GetService(String("window"), (IInterface**)&service);
    mWindowManager = IIWindowManager::Probe(service);

    service = NULL;
    serviceManager->GetService(String("accessibility"), (IInterface**)&service);
    mAccessibilityManager = IIAccessibilityManager::Probe(service);

    service = NULL;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&service);
    mUserManager = IUserManager::Probe(service);
    AutoPtr<ITextToSpeechOnInitListener> ttsShutdownOnInitListener =
            new TtsShutdownOnInitListener(this);
    //TODO CTextToSpeech::New(context, ttsShutdownOnInitListener, (ITextToSpeech**)&mTts);

    AutoPtr<ISettingsSystem> settingsSystem;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&settingsSystem);
    AutoPtr<IUri> uri;
    settingsSystem->GetDEFAULT_NOTIFICATION_URI((IUri**)&uri);
    //TODO CRingtoneManager::GetRingtone(context, uri, (IRingtone**)&mTone);
    mTone->SetStreamType(IAudioManager::STREAM_MUSIC);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Int32 touchSlop = 0;
    resources->GetDimensionPixelSize(
            R::dimen::accessibility_touch_slop, &touchSlop);
    mTouchSlop = (Float)touchSlop;
    return NOERROR;
}

Boolean EnableAccessibilityController::CanEnableAccessibilityViaGesture(
        /* [in] */ IContext* context)
{
    AutoPtr<IAccessibilityManagerHelper> helper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
    AutoPtr<IAccessibilityManager> accessibilityManager;
    helper->GetInstance(context, (IAccessibilityManager**)&accessibilityManager);
    // Accessibility is enabled and there is an enabled speaking
    // accessibility service, then we have nothing to do.
    AutoPtr<IList> serviceList;
    accessibilityManager->GetEnabledAccessibilityServiceList(
                    IAccessibilityServiceInfo::FEEDBACK_SPOKEN, (IList**)&serviceList);
    Boolean isEnabled = FALSE;
    Int32 size = 0;
    if ((accessibilityManager->IsEnabled(&isEnabled), isEnabled)
            && (serviceList->GetSize(&size), size) > 0) {
        return FALSE;
    }
    // If the global gesture is enabled and there is a speaking service
    // installed we are good to go, otherwise there is nothing to do.
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Int32 value = 0;
    settingsGlobal->GetInt32(contentResolver,
            ISettingsGlobal::ENABLE_ACCESSIBILITY_GLOBAL_GESTURE_ENABLED, 0, &value);
    return value == 1 && !GetInstalledSpeakingAccessibilityServices(context)->IsEmpty();
}

AutoPtr< List< AutoPtr<IAccessibilityServiceInfo> > > EnableAccessibilityController::GetInstalledSpeakingAccessibilityServices(
        /* [in] */ IContext* context)
{
    AutoPtr< List< AutoPtr<IAccessibilityServiceInfo> > > services = new List< AutoPtr<IAccessibilityServiceInfo> >();

    AutoPtr<IAccessibilityManagerHelper> helper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
    AutoPtr<IAccessibilityManager> accessibilityManager;
    helper->GetInstance(context, (IAccessibilityManager**)&accessibilityManager);

    AutoPtr<IList> serviceList;
    accessibilityManager->GetInstalledAccessibilityServiceList((IList**)&serviceList);

    AutoPtr<IIterator> iterator;
    serviceList->GetIterator((IIterator**)&iterator);
    Boolean has = FALSE;
    while (iterator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        AutoPtr<IAccessibilityServiceInfo> service = IAccessibilityServiceInfo::Probe(obj);
        Int32 feedbackType = 0;
        service->GetFeedbackType(&feedbackType);
        if ((feedbackType & IAccessibilityServiceInfo::FEEDBACK_SPOKEN) != 0) {
            services->PushBack(service);
        }
    }

    return services;
}

ECode EnableAccessibilityController::OnDestroy()
{
    mDestroyed = TRUE;
    return NOERROR;
}

ECode EnableAccessibilityController::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 eventType = 0;
    Int32 count = 0;
    if ((event->GetActionMasked(&eventType), eventType) == IMotionEvent::ACTION_POINTER_DOWN
            && (event->GetPointerCount(&count), count) == 2) {
        event->GetX(0, &mFirstPointerDownX);
        event->GetY(0, &mFirstPointerDownY);
        event->GetX(1, &mSecondPointerDownX);
        event->GetY(1, &mSecondPointerDownY);

        AutoPtr<IMessage> messageWarning;
        mHandler->ObtainMessage(MESSAGE_SPEAK_WARNING, (IMessage**)&messageWarning);
        Boolean isSuccess = FALSE;
        mHandler->SendMessageDelayed(messageWarning, SPEAK_WARNING_DELAY_MILLIS, &isSuccess);
        AutoPtr<IMessage> messageAccessibility;
        mHandler->ObtainMessage(MESSAGE_ENABLE_ACCESSIBILITY, (IMessage**)&messageAccessibility);
        mHandler->SendMessageDelayed(messageAccessibility,
               ENABLE_ACCESSIBILITY_DELAY_MILLIS, &isSuccess);
        *result = TRUE;
    }
    *result = FALSE;
    return NOERROR;
}

ECode EnableAccessibilityController::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 pointerCount = 0;
    event->GetPointerCount(&pointerCount);
    Int32 action = 0;
    event->GetActionMasked(&action);
    if (mCanceled) {
        if (action == IMotionEvent::ACTION_UP) {
            mCanceled = FALSE;
        }
        *result = TRUE;
    }
    switch (action) {
        case IMotionEvent::ACTION_POINTER_DOWN:
            {
                if (pointerCount > 2) {
                    Cancel();
                }
            }
            break;
        case IMotionEvent::ACTION_MOVE:
            {
                Float x0, y0;
                event->GetX(0, &x0);
                event->GetY(0, &y0);
                Float firstPointerMove = MathUtils::Dist(x0,
                        y0, mFirstPointerDownX, mFirstPointerDownY);
                if (Elastos::Core::Math::Abs(firstPointerMove) > mTouchSlop) {
                    Cancel();
                }
                Float x1, y1;
                event->GetX(1, &x1);
                event->GetY(1, &y1);
                Float secondPointerMove = MathUtils::Dist(x1,
                        y1, mSecondPointerDownX, mSecondPointerDownY);
                if (Elastos::Core::Math::Abs(secondPointerMove) > mTouchSlop) {
                    Cancel();
                }
            }
            break;
        case IMotionEvent::ACTION_POINTER_UP:
        case IMotionEvent::ACTION_CANCEL:
            {
                Cancel();
            }
            break;
    }
    *result = TRUE;
    return NOERROR;
}

void EnableAccessibilityController::Cancel()
{
    mCanceled = TRUE;
    Boolean has = FALSE;
    if (mHandler->HasMessages(MESSAGE_SPEAK_WARNING, &has), has) {
        mHandler->RemoveMessages(MESSAGE_SPEAK_WARNING);
    }
    else if (mHandler->HasMessages(MESSAGE_ENABLE_ACCESSIBILITY, &has), has) {
        AutoPtr<IMessage> message;
        mHandler->ObtainMessage(MESSAGE_SPEAK_ENABLE_CANCELED, (IMessage**)&message);
        Boolean isSuccess = FALSE;
        mHandler->SendMessage(message, &isSuccess);
    }
    mHandler->RemoveMessages(MESSAGE_ENABLE_ACCESSIBILITY);
}

void EnableAccessibilityController::EnableAccessibility()
{
    AutoPtr< List< AutoPtr<IAccessibilityServiceInfo> > > services = GetInstalledSpeakingAccessibilityServices(
            mContext);
    if (services->IsEmpty()) {
        return;
    }
    Boolean keyguardLocked = FALSE;
    // try {
    mWindowManager->IsKeyguardLocked(&keyguardLocked);
    // } catch (RemoteException re) {
        /* ignore */
    // }

    AutoPtr<IList> users;
    mUserManager->GetUsers((IList**)&users);
    Int32 size = 0;
    users->GetSize(&size);
    Boolean hasMoreThanOneUser = size > 1;

    AutoPtr<IAccessibilityServiceInfo> service = *(services->Begin());
    Int32 flags = 0;
    Boolean enableTouchExploration = ((service->GetFlags(&flags), flags)
            & IAccessibilityServiceInfo::FLAG_REQUEST_TOUCH_EXPLORATION_MODE) != 0;
    // Try to find a service supporting explore by touch.
    if (!enableTouchExploration) {
        List< AutoPtr<IAccessibilityServiceInfo> >::Iterator it = services->Begin();
        for (; it != services->End(); ++it) {
            AutoPtr<IAccessibilityServiceInfo> candidate = *it;
            Int32 candidateFlags = 0;
            if (((candidate->GetFlags(&candidateFlags), candidateFlags)
                    & IAccessibilityServiceInfo::FLAG_REQUEST_TOUCH_EXPLORATION_MODE) != 0) {
                enableTouchExploration = TRUE;
                service = candidate;
                break;
            }
        }
    }

    AutoPtr<IResolveInfo> resolveInfo;
    service->GetResolveInfo((IResolveInfo**)&resolveInfo);
    AutoPtr<IServiceInfo> serviceInfo;
    resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
    String packageName;
    AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(serviceInfo);
    pii->GetPackageName(&packageName);
    String name;
    pii->GetName(&name);
    AutoPtr<IComponentName> componentName;
    CComponentName::New(packageName, name, (IComponentName**)&componentName);
    if (!keyguardLocked || !hasMoreThanOneUser) {
        Int32 userId = CActivityManager::GetCurrentUser();
        String enabledServiceString;
        componentName->FlattenToString(&enabledServiceString);
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        // Enable one speaking accessibility service.
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        Boolean result = FALSE;
        settingsSecure->PutStringForUser(resolver,
                ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
                enabledServiceString, userId, &result);
        // Allow the services we just enabled to toggle touch exploration.
        settingsSecure->PutStringForUser(resolver,
                ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
                enabledServiceString, userId, &result);
        // Enable touch exploration.
        if (enableTouchExploration) {
            settingsSecure->PutInt32ForUser(resolver, ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
                    1, userId, &result);
        }
        // Enable accessibility script injection (AndroidVox) for web content.
        settingsSecure->PutInt32ForUser(resolver, ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION,
                1, userId, &result);
        // Turn on accessibility mode last.
        settingsSecure->PutInt32ForUser(resolver, ISettingsSecure::ACCESSIBILITY_ENABLED,
                1, userId, &result);
    } else if (keyguardLocked) {
        // try {
        mAccessibilityManager->TemporaryEnableAccessibilityStateUntilKeyguardRemoved(
                componentName, enableTouchExploration);
        // } catch (RemoteException re) {
            /* ignore */
        // }
    }

    mOnAccessibilityEnabledCallback->Run();
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
