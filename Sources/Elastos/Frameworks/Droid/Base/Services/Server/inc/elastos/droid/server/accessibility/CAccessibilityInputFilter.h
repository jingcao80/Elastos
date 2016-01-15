#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYINPUTFILTER_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYINPUTFILTER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_Accessibility_CAccessibilityInputFilter.h"
#include "elastos/droid/view/InputFilter.h"
#include "accessibility/CAccessibilityManagerService.h"
#include "accessibility/TouchExplorer.h"
#include "accessibility/ScreenMagnifier.h"

using Elastos::Droid::View::IIInputFilter;
using Elastos::Droid::View::InputFilter;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputFilterHost;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IIAccessibilityManager;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

CarClass(CAccessibilityInputFilter), public InputFilter
{
public:
    CAccessibilityInputFilter();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IIAccessibilityManager* service);

    CARAPI OnInstalled();

    CARAPI OnUninstalled();

    CARAPI OnInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI OnMotionEvent(
        /* [in] */ IMotionEvent* event,
        /* [in] */ IMotionEvent* rawEvent,
        /* [in] */ Int32 policyFlags);

    CARAPI OnAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI SetNext(
        /* [in] */ IEventStreamTransformation* next);

    CARAPI Clear();

    CARAPI SetEnabledFeatures(
        /* [in] */ Int32 enabledFeatures);

    CARAPI NotifyAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnDestroy();

    CARAPI Install(
        /* [in] */ IInputFilterHost* host);

    CARAPI Uninstall();

    CARAPI FilterInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

    CARAPI SendInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Int32 policyFlags);

private:
    CARAPI_(void) EnableFeatures();

    CARAPI_(void) DisableFeatures();

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

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 UNDEFINED_DEVICE_ID;

    AutoPtr<IContext> mContext;

    AutoPtr<IPowerManager> mPm;

    AutoPtr<CAccessibilityManagerService> mAms;

    Int32 mCurrentDeviceId;

    Boolean mInstalled;

    Int32 mEnabledFeatures;

    AutoPtr<TouchExplorer> mTouchExplorer;
    AutoPtr<ScreenMagnifier> mScreenMagnifier;
    AutoPtr<IEventStreamTransformation> mEventHandler;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYINPUTFILTER_H__
