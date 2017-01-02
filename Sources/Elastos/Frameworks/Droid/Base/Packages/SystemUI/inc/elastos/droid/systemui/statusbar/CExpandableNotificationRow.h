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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CEXPANDABLENOTIFICATIONROW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CEXPANDABLENOTIFICATIONROW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CExpandableNotificationRow.h"
#include "elastos/droid/systemui/statusbar/ActivatableNotificationView.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::View::IViewStub;
using Elastos::Droid::View::IViewStubOnInflateListener;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CExpandableNotificationRow)
    , public ActivatableNotificationView
    , public IExpandableNotificationRow
{
private:
    class InflateListener
        : public Object
        , public IViewStubOnInflateListener
    {
    public:
        CAR_INTERFACE_DECL()

        InflateListener(
            /* [in] */ CExpandableNotificationRow* host);

        // @Override
        CARAPI OnInflate(
            /* [in] */ IViewStub* stub,
            /* [in] */ IView* inflated);

    private:
        CExpandableNotificationRow* mHost;
    };

    class Runnable1: public Runnable
    {
    public:
        Runnable1(
            /* [in] */ IView* view);

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IView> mView;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CExpandableNotificationRow();

    CARAPI SetIconAnimationRunning(
        /* [in] */ Boolean running);

    CARAPI SetStatusBarNotification(
        /* [in] */ IStatusBarNotification* statusBarNotification);

    CARAPI GetStatusBarNotification(
        /* [out] */ IStatusBarNotification** notification);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Resets this view so it can be re-used for an updated notification.
     */
    // @Override
    CARAPI Reset();

    CARAPI ResetHeight();

    // @Override
    CARAPI OnRequestSendAccessibilityEvent(
        /* [in] */ IView* child,
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetDark(
        /* [in] */ Boolean dark,
        /* [in] */ Boolean fade);

    CARAPI SetHeightRange(
        /* [in] */ Int32 rowMinHeight,
        /* [in] */ Int32 rowMaxHeight);

    CARAPI IsExpandable(
        /* [out] */ Boolean* result);

    CARAPI SetExpandable(
        /* [in] */ Boolean expandable);

    /**
     * @return whether the user has changed the expansion state
     */
    CARAPI HasUserChangedExpansion(
        /* [out] */ Boolean* result);

    CARAPI IsUserExpanded(
        /* [out] */ Boolean* result);

    /**
     * Set this notification to be expanded by the user
     *
     * @param userExpanded whether the user wants this notification to be expanded
     */
    CARAPI SetUserExpanded(
        /* [in] */ Boolean userExpanded);

    CARAPI ResetUserExpansion();

    CARAPI IsUserLocked(
        /* [out] */ Boolean* result);

    CARAPI SetUserLocked(
        /* [in] */ Boolean userLocked);

    /**
     * @return has the system set this notification to be expanded
     */
    CARAPI IsSystemExpanded(
        /* [out] */ Boolean* result);

    /**
     * Set this notification to be expanded by the system.
     *
     * @param expand whether the system wants this notification to be expanded.
     */
    CARAPI SetSystemExpanded(
        /* [in] */ Boolean expand);

    /**
     * @param expansionDisabled whether to prevent notification expansion
     */
    CARAPI SetExpansionDisabled(
        /* [in] */ Boolean expansionDisabled);

    /**
     * @return Can the underlying notification be cleared?
     */
    CARAPI IsClearable(
        /* [out] */ Boolean* result);

    /**
     * Set whether the notification can be cleared.
     *
     * @param clearable
     */
    CARAPI SetClearable(
        /* [in] */ Boolean clearable);

    /**
     * Apply an expansion state to the layout.
     */
    CARAPI ApplyExpansionToLayout();

    // @Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    CARAPI SetSensitive(
        /* [in] */ Boolean sensitive);

    CARAPI SetHideSensitiveForIntrinsicHeight(
        /* [in] */ Boolean hideSensitive);

    CARAPI SetHideSensitive(
        /* [in] */ Boolean hideSensitive,
        /* [in] */ Boolean animated,
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration);

    CARAPI GetMaxExpandHeight(
        /* [out] */ Int32* height);

    // @Override
    CARAPI IsContentExpandable(
        /* [out] */ Boolean* result);

    using ExpandableView::SetActualHeight;

    // @Override
    CARAPI SetActualHeight(
        /* [in] */ Int32 height,
        /* [in] */ Boolean notifyListeners);

    // @Override
    CARAPI GetMaxHeight(
        /* [out] */ Int32* height);

    // @Override
    CARAPI GetMinHeight(
        /* [out] */ Int32* height);

    // @Override
    CARAPI SetClipTopAmount(
        /* [in] */ Int32 clipTopAmount);

    CARAPI NotifyContentUpdated();

    CARAPI IsMaxExpandHeightInitialized(
        /* [out] */ Boolean* result);

    CARAPI SetExpansionLogger(
        /* [in] */ IExpansionLogger* logger,
        /* [in] */ const String& key);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

private:
    CARAPI_(void) SetIconAnimationRunning(
        /* [in] */ Boolean running,
        /* [in] */ INotificationContentView* layout);

    CARAPI_(void) SetIconAnimationRunningForChild(
        /* [in] */ Boolean running,
        /* [in] */ IView* child);

    CARAPI_(void) SetIconRunning(
        /* [in] */ IImageView* imageView,
        /* [in] */ Boolean running);

    /**
     * Check whether the view state is currently expanded. This is given by the system in {@link
     * #setSystemExpanded(Boolean)} and can be overridden by user expansion or
     * collapsing in {@link #setUserExpanded(Boolean)}. Note that the visual appearance of this
     * view can differ from this state, if layout params are modified from outside.
     *
     * @return whether the view state is currently expanded.
     */
    CARAPI_(Boolean) IsExpanded();

    CARAPI_(void) UpdateMaxExpandHeight();

    CARAPI_(void) AnimateShowingPublic(
        /* [in] */ Int64 delay,
        /* [in] */ Int64 duration);

    CARAPI_(void) UpdateVetoButton();

    CARAPI_(AutoPtr<INotificationContentView>) GetShowingLayout();

    CARAPI LogExpansionEvent(
        /* [in] */ Boolean userAction,
        /* [in] */ Boolean wasExpanded);

private:
    Int32 mRowMinHeight;
    Int32 mRowMaxHeight;

    /** Does this row contain layouts that can adapt to row expansion */
    Boolean mExpandable;
    /** Has the user actively changed the expansion state of this row */
    Boolean mHasUserChangedExpansion;
    /** If {@link #mHasUserChangedExpansion}, has the user expanded this row */
    Boolean mUserExpanded;
    /** Is the user touching this row */
    Boolean mUserLocked;
    /** Are we showing the "public" version */
    Boolean mShowingPublic;
    Boolean mSensitive;
    Boolean mShowingPublicInitialized;
    Boolean mShowingPublicForIntrinsicHeight;

    /**
     * Is this notification expanded by the system. The expansion state can be overridden by the
     * user expansion.
     */
    Boolean mIsSystemExpanded;

    /**
     * Whether the notification expansion is disabled. This is the case on Keyguard.
     */
    Boolean mExpansionDisabled;

    AutoPtr<INotificationContentView> mPublicLayout;
    AutoPtr<INotificationContentView> mPrivateLayout;
    Int32 mMaxExpandHeight;
    AutoPtr<IView> mVetoButton;
    Boolean mClearable;
    AutoPtr<IExpansionLogger> mLogger;
    String mLoggingKey;
    Boolean mWasReset;
    AutoPtr<INotificationGuts> mGuts;

    AutoPtr<IStatusBarNotification> mStatusBarNotification;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CEXPANDABLENOTIFICATIONROW_H__
