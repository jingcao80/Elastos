#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_ACCESSIBILITYINPUTFILTER_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_ACCESSIBILITYINPUTFILTER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/server/accessibility/TouchExplorer.h"
#include "elastos/droid/server/accessibility/ScreenMagnifier.h"
#include "elastos/droid/view/InputFilter.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Utility::Pools;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IIAccessibilityManager;
using Elastos::Droid::View::InputFilter;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IChoreographer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class MotionEventHolder
    : public Object
{
public:
    MotionEventHolder();

    ~MotionEventHolder();

    static CARAPI_(AutoPtr<MotionEventHolder>) Obtain(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI Recycle();

public:
    Int32 mPolicyFlags;
    AutoPtr<IMotionEvent> mEvent;
    AutoPtr<MotionEventHolder> mNext;
    AutoPtr<MotionEventHolder> mPrevious;

private:
    static const Int32 MAX_POOL_SIZE;
    static AutoPtr< Pools::SimplePool<MotionEventHolder> > sPool;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Accessibility::MotionEventHolder, IInterface);

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class AccessibilityManagerService;

/**
 * This class is an input filter for implementing accessibility features such
 * as display magnification and explore by touch.
 *
 * NOTE: This class has to be created and poked only from the main thread.
 */
class AccessibilityInputFilter
    : public InputFilter
    , public IEventStreamTransformation
{
    friend class AccessibilityManagerService;
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ AccessibilityInputFilter* host);

        ~MyRunnable();

        // @Override
        CARAPI Run();

    private:
        AccessibilityInputFilter* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("AccessibilityInputFilter")

    AccessibilityInputFilter();

    ~AccessibilityInputFilter();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IIAccessibilityManager* service);

    // @Override
    CARAPI OnInstalled();

    // @Override
    CARAPI OnUninstalled();

    // @Override
    CARAPI OnInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

    // @Override
    CARAPI OnMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ IMotionEvent* rawEvent,
        /* [in] */ Int32 policyFlags);

    // @Override
    CARAPI OnAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI SetNext(
        /* [in] */ IEventStreamTransformation* next);

    // @Override
    CARAPI Clear();

    // @Override
    CARAPI OnDestroy();

protected:
    CARAPI SetEnabledFeatures(
        /* [in] */ Int32 enabledFeatures);

    CARAPI NotifyAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI DisableFeatures();

private:
    CARAPI_(void) OnMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) OnKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) ScheduleProcessBatchedEvents();

    CARAPI_(void) BatchMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) ProcessBatchedEvents(
        /* [in] */ Int64 frameNanos);

    CARAPI_(void) HandleMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI_(void) EnableFeatures();

public:
    /**
     * Flag for enabling the screen magnification feature.
     *
     * @see #setEnabledFeatures(int)
     */
    static const Int32 FLAG_FEATURE_SCREEN_MAGNIFIER;

    /**
     * Flag for enabling the touch exploration feature.
     *
     * @see #setEnabledFeatures(int)
     */
    static const Int32 FLAG_FEATURE_TOUCH_EXPLORATION;

    /**
     * Flag for enabling the filtering key events feature.
     *
     * @see #setEnabledFeatures(int)
     */
    static const Int32 FLAG_FEATURE_FILTER_KEY_EVENTS;

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<Runnable> mProcessBatchedEventsRunnable;

    AutoPtr<IContext> mContext;

    AutoPtr<IPowerManager> mPm;

    AccessibilityManagerService* mAms;

    AutoPtr<IChoreographer> mChoreographer;

    Int32 mCurrentTouchDeviceId;

    Boolean mInstalled;

    Int32 mEnabledFeatures;

    AutoPtr<TouchExplorer> mTouchExplorer;

    AutoPtr<ScreenMagnifier> mScreenMagnifier;

    AutoPtr<IEventStreamTransformation> mEventHandler;

    AutoPtr<MotionEventHolder> mEventQueue;

    Boolean mMotionEventSequenceStarted;

    Boolean mHoverEventSequenceStarted;

    Boolean mKeyEventSequenceStarted;

    Boolean mFilterKeyEvents;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_ACCESSIBILITYINPUTFILTER_H__
