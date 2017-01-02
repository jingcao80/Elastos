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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ENABLEACCESSIBILITYCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ENABLEACCESSIBILITYCONTROLLER_H__

#include "Elastos.Droid.AccessibilityService.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Speech.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Speech::Tts::ITextToSpeech;
using Elastos::Droid::Speech::Tts::ITextToSpeechOnInitListener;
using Elastos::Droid::View::Accessibility::IIAccessibilityManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {


class EnableAccessibilityController
    : public Object
    , IEnableAccessibilityController
{
// Inner class of global variable.
private:
    class SpeakHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("EnableAccessibilityController::SpeakHandler")

        SpeakHandler(
            /* [in] */ EnableAccessibilityController* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        EnableAccessibilityController* mHost;
    };

// Inner class of local variable.
private:
    class TtsShutdownOnInitListener
        : public Object
        , public ITextToSpeechOnInitListener
    {
    public:
        CAR_INTERFACE_DECL()

        TtsShutdownOnInitListener(
            /* [in] */ EnableAccessibilityController* host);

        // @Override
        ECode OnInit(
            /* [in] */ Int32 status);

    private:
        EnableAccessibilityController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    EnableAccessibilityController();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IRunnable* onAccessibilityEnabledCallback);

    CARAPI OnDestroy();

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    static CARAPI_(Boolean) CanEnableAccessibilityViaGesture(
        /* [in] */ IContext* context);

private:
    CARAPI_(void) Cancel();

    CARAPI_(void) EnableAccessibility();

    static CARAPI_(AutoPtr< List< AutoPtr<IAccessibilityServiceInfo> > >) GetInstalledSpeakingAccessibilityServices(
            /* [in] */ IContext* context);

public:
    static const Int32 MESSAGE_SPEAK_WARNING = 1;
    static const Int32 MESSAGE_SPEAK_ENABLE_CANCELED = 2;
    static const Int32 MESSAGE_ENABLE_ACCESSIBILITY = 3;

private:
    static const Int32 SPEAK_WARNING_DELAY_MILLIS = 2000;
    static const Int32 ENABLE_ACCESSIBILITY_DELAY_MILLIS = 6000;

    AutoPtr<SpeakHandler> mHandler;
    AutoPtr<IIWindowManager> mWindowManager;
    AutoPtr<IIAccessibilityManager> mAccessibilityManager;
    AutoPtr<IContext> mContext;
    AutoPtr<IRunnable> mOnAccessibilityEnabledCallback;
    AutoPtr<IUserManager> mUserManager;
    AutoPtr<ITextToSpeech> mTts;
    AutoPtr<IRingtone> mTone;
    Float mTouchSlop;
    Boolean mDestroyed;
    Boolean mCanceled;
    Float mFirstPointerDownX;
    Float mFirstPointerDownY;
    Float mSecondPointerDownX;
    Float mSecondPointerDownY;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ENABLEACCESSIBILITYCONTROLLER_H__
