
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ENABLEACCESSIBILITYCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ENABLEACCESSIBILITYCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerBase.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Speech::Tts::ITextToSpeech;
using Elastos::Droid::Speech::Tts::ITextToSpeechOnInitListener;
using Elastos::Droid::View::Accessibility::IIAccessibilityManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IMotionEvent;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {


class EnableAccessibilityController
    : public ElRefBase
{
// Inner class of global variable.
private:
    class SpeakHandler
        : public HandlerBase
    {
    public:
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
        : public ITextToSpeechOnInitListener
        , public ElRefBase
    {
    public:
        TtsShutdownOnInitListener(
            /* [in] */ EnableAccessibilityController* host);

        CAR_INTERFACE_DECL();

        // @Override
        ECode OnInit(
            /* [in] */ Int32 status);

    private:
        EnableAccessibilityController* mHost;
    };

public:
    EnableAccessibilityController(
        /* [in] */ IContext* context);

    CARAPI_(void) OnDestroy();

    CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* event);

public:
    static CARAPI_(Boolean) CanEnableAccessibilityViaGesture(
        /* [in] */ IContext* context);

private:
    CARAPI_(void) Cancel();

    CARAPI_(void) EnableAccessibility();

private:
    static CARAPI_(AutoPtr< List< AutoPtr<IAccessibilityServiceInfo> > >) GetInstalledSpeakingAccessibilityServices(
            /* [in] */ IContext* context);

private:
    static const Int32 SPEAK_WARNING_DELAY_MILLIS;
    static const Int32 ENABLE_ACCESSIBILITY_DELAY_MILLIS;

    static const Int32 MESSAGE_SPEAK_WARNING;
    static const Int32 MESSAGE_SPEAK_ENABLE_CANCELED;
    static const Int32 MESSAGE_ENABLE_ACCESSIBILITY;

private:
    AutoPtr<IHandler> mHandler;

    AutoPtr<IIWindowManager> mWindowManager;

    AutoPtr<IIAccessibilityManager> mAccessibilityManager;

private:
    AutoPtr<IContext> mContext;
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

} // namespace Elastos
} // namespace Droid
} // namespace Internal
} // namespace Policy
} // namespace Impl

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_ENABLEACCESSIBILITYCONTROLLER_H__
