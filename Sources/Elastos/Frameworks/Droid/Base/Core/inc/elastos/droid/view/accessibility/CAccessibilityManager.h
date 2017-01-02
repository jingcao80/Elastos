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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_View_Accessibility_CAccessibilityManager.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::View::IIWindow;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::ICopyOnWriteArrayList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityManager)
    , public Object
    , public IAccessibilityManager
{
    friend class CAccessibilityManagerClient;
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CAccessibilityManager::MyHandler")

        MyHandler(
            /* [in] */ CAccessibilityManager* host,
            /* [in] */ ILooper* looper);

        ~MyHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    public:
        static const Int32 MSG_NOTIFY_ACCESSIBILITY_STATE_CHANGED;
        static const Int32 MSG_NOTIFY_EXPLORATION_STATE_CHANGED;
        static const Int32 MSG_NOTIFY_HIGH_TEXT_CONTRAST_STATE_CHANGED;
        static const Int32 MSG_SET_STATE;
    private:
        CAccessibilityManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityManager();

    ~CAccessibilityManager();

    /**
     * Create an instance.
     *
     * @param context A {@link Context}.
     * @param service An interface to the backing service.
     * @param userId User id under which to run.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIAccessibilityManager* service,
        /* [in] */ Int32 userId);

    /**
     * Get an AccessibilityManager instance (create one if necessary).
     *
     * @param context Context in which this manager operates.
     *
     * @hide
     */
    static CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IAccessibilityManager** manager);

    /**
     * @hide
     */
    CARAPI GetClient(
        /* [out] */ IIAccessibilityManagerClient** client);

    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    /**
     * Returns if the touch exploration in the system is enabled.
     *
     * @return True if touch exploration is enabled, false otherwise.
     */
    CARAPI IsTouchExplorationEnabled(
        /* [out] */ Boolean* result);

    /**
     * Returns if the high text contrast in the system is enabled.
     * <p>
     * <strong>Note:</strong> You need to query this only if you application is
     * doing its own rendering and does not rely on the platform rendering pipeline.
     * </p>
     *
     * @return True if high text contrast is enabled, false otherwise.
     *
     * @hide
     */
    CARAPI IsHighTextContrastEnabled(
        /* [out] */ Boolean* result);

    /**
     * Sends an {@link AccessibilityEvent}.
     *
     * @param event The event to send.
     *
     * @throws IllegalStateException if accessibility is not enabled.
     *
     * <strong>Note:</strong> The preferred mechanism for sending custom accessibility
     * events is through calling
     * {@link android.view.ViewParent#requestSendAccessibilityEvent(View, AccessibilityEvent)}
     * instead of this method to allow predecessors to augment/filter events sent by
     * their descendants.
     */
    CARAPI SendAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    /**
     * Requests feedback interruption from all accessibility services.
     */
    CARAPI Interrupt();

    /**
     * Returns the {@link ServiceInfo}s of the installed accessibility services.
     *
     * @return An unmodifiable list with {@link ServiceInfo}s.
     *
     * @deprecated Use {@link #getInstalledAccessibilityServiceList()}
     */
    // @Deprecated
    CARAPI GetAccessibilityServiceList(
        /* [out] */ IList** serviceList);

    /**
     * Returns the {@link AccessibilityServiceInfo}s of the installed accessibility services.
     *
     * @return An unmodifiable list with {@link AccessibilityServiceInfo}s.
     */
    CARAPI GetInstalledAccessibilityServiceList(
        /* [out] */ IList** serviceList);

    /**
     * Returns the {@link AccessibilityServiceInfo}s of the enabled accessibility services
     * for a given feedback type.
     *
     * @param feedbackTypeFlags The feedback type flags.
     * @return An unmodifiable list with {@link AccessibilityServiceInfo}s.
     *
     * @see AccessibilityServiceInfo#FEEDBACK_AUDIBLE
     * @see AccessibilityServiceInfo#FEEDBACK_GENERIC
     * @see AccessibilityServiceInfo#FEEDBACK_HAPTIC
     * @see AccessibilityServiceInfo#FEEDBACK_SPOKEN
     * @see AccessibilityServiceInfo#FEEDBACK_VISUAL
     * @see AccessibilityServiceInfo#FEEDBACK_BRAILLE
     */
    CARAPI GetEnabledAccessibilityServiceList(
        /* [in] */ Int32 feedbackTypeFlags,
        /* [out] */ IList** serviceList);

    /**
     * Registers an {@link AccessibilityStateChangeListener} for changes in
     * the global accessibility state of the system.
     *
     * @param listener The listener.
     * @return True if successfully registered.
     */
    CARAPI AddAccessibilityStateChangeListener(
        /* [in] */ IAccessibilityManagerAccessibilityStateChangeListener* listener,
        /* [out] */ Boolean* result);

    /**
     * Unregisters an {@link AccessibilityStateChangeListener}.
     *
     * @param listener The listener.
     * @return True if successfully unregistered.
     */
    CARAPI RemoveAccessibilityStateChangeListener(
        /* [in] */ IAccessibilityManagerAccessibilityStateChangeListener* listener,
        /* [out] */ Boolean* result);

    /**
     * Registers a {@link TouchExplorationStateChangeListener} for changes in
     * the global touch exploration state of the system.
     *
     * @param listener The listener.
     * @return True if successfully registered.
     */
    CARAPI AddTouchExplorationStateChangeListener(
        /* [in] */ IAccessibilityManagerTouchExplorationStateChangeListener* listener,
        /* [out] */ Boolean* result);

    /**
     * Unregisters a {@link TouchExplorationStateChangeListener}.
     *
     * @param listener The listener.
     * @return True if successfully unregistered.
     */
    CARAPI RemoveTouchExplorationStateChangeListener(
        /* [in] */ IAccessibilityManagerTouchExplorationStateChangeListener* listener,
        /* [out] */ Boolean* result);

    /**
     * Registers a {@link HighTextContrastChangeListener} for changes in
     * the global high text contrast state of the system.
     *
     * @param listener The listener.
     * @return True if successfully registered.
     *
     * @hide
     */
    CARAPI AddHighTextContrastStateChangeListener(
        /* [in] */ IAccessibilityManagerHighTextContrastChangeListener* listener,
        /* [out] */ Boolean* result);

    /**
     * Unregisters a {@link HighTextContrastChangeListener}.
     *
     * @param listener The listener.
     * @return True if successfully unregistered.
     *
     * @hide
     */
    CARAPI RemoveHighTextContrastStateChangeListener(
        /* [in] */ IAccessibilityManagerHighTextContrastChangeListener* listener,
        /* [out] */ Boolean* result);

    /**
     * Adds an accessibility interaction connection interface for a given window.
     * @param windowToken The window token to which a connection is added.
     * @param connection The connection.
     *
     * @hide
     */
    CARAPI AddAccessibilityInteractionConnection(
        /* [in] */ IIWindow* windowToken,
        /* [in] */ IIAccessibilityInteractionConnection* connection,
        /* [out] */ Int32* add);

    /**
     * Removed an accessibility interaction connection interface for a given window.
     * @param windowToken The window token to which a connection is removed.
     *
     * @hide
     */
    CARAPI RemoveAccessibilityInteractionConnection(
        /* [in] */ IIWindow* windowToken);

private:
    /**
     * Sets the current state and notifies listeners, if necessary.
     *
     * @param stateFlags The state flags.
     */
    CARAPI_(void) SetStateLocked(
        /* [in] */ Int32 stateFlags);

    CARAPI_(AutoPtr<IIAccessibilityManager>) GetServiceLocked();

    CARAPI_(void) TryConnectToServiceLocked();

    /**
     * Notifies the registered {@link AccessibilityStateChangeListener}s.
     */
    CARAPI_(void) HandleNotifyAccessibilityStateChanged();

    /**
     * Notifies the registered {@link TouchExplorationStateChangeListener}s.
     */
    CARAPI_(void) HandleNotifyTouchExplorationStateChanged();

    /**
     * Notifies the registered {@link HighTextContrastChangeListener}s.
     */
    CARAPI_(void) HandleNotifyHighTextContrastStateChanged();

public:
    Int32 mUserId;

    AutoPtr<MyHandler> mHandler;

    Boolean mIsEnabled;

    Boolean mIsTouchExplorationEnabled;

    Boolean mIsHighTextContrastEnabled;

private:
    static const Boolean DEBUG;

    static const String localLOG_TAG;

    static Object sInstanceSync;

    static AutoPtr<IAccessibilityManager> sInstance;

    Object mLock;

    AutoPtr<IIAccessibilityManager> mService;

    AutoPtr<ICopyOnWriteArrayList> mAccessibilityStateChangeListeners;

    AutoPtr<ICopyOnWriteArrayList> mTouchExplorationStateChangeListeners;

    AutoPtr<ICopyOnWriteArrayList> mHighTextContrastStateChangeListeners;

    AutoPtr<IIAccessibilityManagerClient> mClient;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGER_H__
