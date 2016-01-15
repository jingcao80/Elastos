#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_SCROLLACCESSIBILITYHELPER_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_SCROLLACCESSIBILITYHELPER_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * Helper used to post the VIEW_SCROLLED accessibility event.
 *
 * TODO(mkosiba): Investigate whether this is behavior we want to share with the chrome/ layer.
 * TODO(mkosiba): We currently don't handle JS-initiated scrolling for layers other than the root
 * layer.
 */
class ScrollAccessibilityHelper
  :public Object
{
public:
    class HandlerCallback
        : public Object
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL();
        static const Int32 MSG_VIEW_SCROLLED = 1;

        HandlerCallback(
            /* [in] */ ScrollAccessibilityHelper* owner,
            /* [in] */ IView* eventSender);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        ScrollAccessibilityHelper* mOwner;
        AutoPtr<IView> mEventSender;
    };

public:
    ScrollAccessibilityHelper(
        /* [in] */ IView* eventSender);

    /**
     * Post a callback to send a {@link AccessibilityEvent#TYPE_VIEW_SCROLLED} event.
     * This event is sent at most once every
     * {@link android.view.ViewConfiguration#getSendRecurringAccessibilityEventsInterval()}
     */
    CARAPI_(void) PostViewScrolledAccessibilityEventCallback();

    CARAPI_(void) RemovePostedViewScrolledAccessibilityEventCallback();

    CARAPI_(void) RemovePostedCallbacks();

private:
    // This is copied straight out of android.view.ViewConfiguration.
    static const Int64 SEND_RECURRING_ACCESSIBILITY_EVENTS_INTERVAL_MILLIS = 100;
    AutoPtr<IHandler> mHandler;
    Boolean mMsgViewScrolledQueued;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_SCROLLACCESSIBILITYHELPER_H__
