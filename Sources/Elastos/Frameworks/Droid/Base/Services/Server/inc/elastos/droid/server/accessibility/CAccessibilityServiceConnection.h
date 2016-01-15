
#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Accessibility_CAccessibilityServiceConnection.h"
#include "accessibility/CAccessibilityManagerService.h"
#include "elastos/droid/os/HandlerBase.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::AccessibilityService::IIAccessibilityServiceClient;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionConnectionCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

/**
 * This class represents an accessibility service. It stores all per service
 * data required for the service management, provides API for starting/stopping the
 * service and is responsible for adding/removing the service in the data structures
 * for service management. The class also exposes configuration interface that is
 * passed to the service it represents as soon it is bound. It also serves as the
 * connection for the service.
 */
CarClass(CAccessibilityServiceConnection)
{
private:
    class MyHandler : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CAccessibilityServiceConnection* service)
            : HandlerBase(looper)
            , mHost(service)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CAccessibilityServiceConnection* mHost;
    };

public:
    CAccessibilityServiceConnection();

    CARAPI constructor(
        /* [in] */ ILooper* looper,
        /* [in] */ Int32 userId,
        /* [in] */ IComponentName* componentName,
        /* [in] */ IAccessibilityServiceInfo* accessibilityServiceInfo,
        /* [in] */ Boolean isAutomation,
        /* [in] */ IIAccessibilityManager* host);

    CARAPI_(void) SetDynamicallyConfigurableProperties(
        /* [in] */ IAccessibilityServiceInfo* info);

    /**
     * Binds to the accessibility service.
     *
     * @return True if binding is successful.
     */
    CARAPI_(Boolean) Bind();

    /**
     * Unbinds form the accessibility service and removes it from the data
     * structures for service management.
     *
     * @return True if unbinding is successful.
     */
    CARAPI_(Boolean) Unbind();

    CARAPI_(Boolean) CanReceiveEvents();

    CARAPI GetServiceInfo(
        /* [out] */ IAccessibilityServiceInfo** serviceInfo);

    CARAPI SetServiceInfo(
        /* [in] */ IAccessibilityServiceInfo* info);

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    CARAPI FindAccessibilityNodeInfoByViewId(
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 viewId,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int64 threadId,
        /* [out] */ Float* scale);

    CARAPI FindAccessibilityNodeInfosByText(
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ const String& text,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int64 threadId,
        /* [out] */ Float* scale);

    CARAPI FindAccessibilityNodeInfoByAccessibilityId(
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int32 flags,
        /* [in] */ Int64 threadId,
        /* [out] */ Float* scale);

    CARAPI FindFocus(
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 focusType,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int64 threadId,
        /* [out] */ Float* scale);

    CARAPI FocusSearch(
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int64 threadId,
        /* [out] */ Float* scale);

    CARAPI PerformAccessibilityAction(
        /* [in] */ Int32 accessibilityWindowId,
        /* [in] */ Int64 accessibilityNodeId,
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments,
        /* [in] */ Int32 interactionId,
        /* [in] */ IAccessibilityInteractionConnectionCallback* cb,
        /* [in] */ Int64 threadId,
        /* [out] */ Boolean* result);

    CARAPI PerformGlobalAction(
        /* [in] */ Int32 action,
        /* [out] */ Boolean* result);

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

    CARAPI LinkToOwnDeath();

    CARAPI UnlinkToOwnDeath();

    CARAPI Dispose();

    CARAPI ProxyDied();

    /**
     * Performs a notification for an {@link AccessibilityEvent}.
     *
     * @param event The event.
     */
    CARAPI_(void) NotifyAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI_(void) NotifyGesture(
        /* [in] */ Int32 gestureId);

private:
    /**
     * Notifies an accessibility service client for a scheduled event given the event type.
     *
     * @param eventType The type of the event to dispatch.
     */
    CARAPI_(void) NotifyAccessibilityEventInternal(
        /* [in] */ Int32 eventType);

    CARAPI_(void) NotifyGestureInternal(
        /* [in] */ Int32 gestureId);

    CARAPI_(void) SendDownAndUpKeyEvents(
        /* [in] */ Int32 keyCode);

    CARAPI_(void) ExpandNotifications();

    CARAPI_(void) ExpandQuickSettings();

    CARAPI_(void) OpenRecents();

    CARAPI_(AutoPtr<IAccessibilityInteractionConnection>) GetConnectionLocked(
        /* [in] */ Int32 windowId);

    CARAPI_(Int32) ResolveAccessibilityWindowIdLocked(
        /* [in] */ Int32 accessibilityWindowId);

    CARAPI_(Float) GetCompatibilityScale(
        /* [in] */ Int32 windowId);

    CARAPI_(void) HandleOnGesture(
        /* [in] */ Int32 gestureId);

    CARAPI_(void) HandleDefault(
        /* [in] */ Int32 eventType);

public:
    Int32 mUserId;

    Int32 mId;

    AutoPtr<IAccessibilityServiceInfo> mAccessibilityServiceInfo;

    AutoPtr<IBinder> mService;

    AutoPtr<IIAccessibilityServiceClient> mServiceInterface;

    Int32 mEventTypes;

    Int32 mFeedbackType;

    HashSet<String> mPackageNames;

    Boolean mIsDefault;

    Boolean mRequestTouchExplorationMode;

    Boolean mIncludeNotImportantViews;

    Int64 mNotificationTimeout;

    AutoPtr<IComponentName> mComponentName;

    AutoPtr<IIntent> mIntent;

    Boolean mCanRetrieveScreenContent;

    Boolean mIsAutomation;

    AutoPtr<IRect> mTempBounds;

    AutoPtr<IResolveInfo> mResolveInfo;

    // the events pending events to be dispatched to this service
    HashMap<Int32, AutoPtr<IAccessibilityEvent> > mPendingEvents;

    /**
     * Handler for delayed event dispatch.
     */
    AutoPtr<IHandler> mHandler;

private:
    static const Boolean DEBUG;

    // We pick the MSB to avoid collision since accessibility event types are
    // used as message types allowing us to remove messages per event type.
    static const Int32 MSG_ON_GESTURE;

    AutoPtr<IIAccessibilityManager> mAccessibilityManager;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYSERVICECONNECTION_H__
