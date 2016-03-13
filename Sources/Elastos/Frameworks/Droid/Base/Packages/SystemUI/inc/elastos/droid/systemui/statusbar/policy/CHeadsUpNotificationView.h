
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CHEADSUPNOTIFICATIONVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CHEADSUPNOTIFICATIONVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CHeadsUpNotificationView.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/view/ViewOutlineProvider.h>
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IInternalInsetsInfo;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IOnComputeInternalInsetsListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOutlineProvider;
using Elastos::Droid::View::ViewOutlineProvider;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Core::Object;
using Elastos::Droid::SystemUI::StatusBar::Phone::IPhoneStatusBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CHeadsUpNotificationView)
    , public FrameLayout
    , public IHeadsUpNotificationView
    , public ISwipeHelperCallback
    , public IExpandHelperCallback
    , public IOnComputeInternalInsetsListener
{
private:
    class EdgeSwipeHelper
        : public Object
        , public IGefingerpoken
    {
    public:
        CAR_INTERFACE_DECL();

        EdgeSwipeHelper(
            /* [in] */ Float touchSlop,
            /* [in] */ CHeadsUpNotificationView* host);

        // @Override
        CARAPI OnInterceptTouchEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

    private:
        static const Boolean DEBUG_EDGE_SWIPE;
        Float mTouchSlop;
        Boolean mConsuming;
        Float mFirstY;
        Float mFirstX;
        CHeadsUpNotificationView* mHost;
    };

    class _ViewOutlineProvider: public ViewOutlineProvider
    {
    public:
        // @Override
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CHeadsUpNotificationView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI UpdateResources();

    CARAPI SetBar(
        /* [in] */ IPhoneStatusBar* bar);

    CARAPI GetHolder(
        /* [out] */ IViewGroup** holder);

    CARAPI ShowNotification(
        /* [in] */ INotificationDataEntry* headsUp,
        /* [out] */ Boolean* result);

    CARAPI IsShowing(
        /* [in] */ const String& key,
        /* [out] */ Boolean* result);

    /** Discard the Heads Up notification. */
    CARAPI Clear();

    /** Respond to dismissal of the Heads Up window. */
    CARAPI Dismiss();

    /** Push any current Heads Up notification down into the shade. */
    CARAPI ReleaseResources();

    CARAPI ReleaseAndClose();

    CARAPI GetEntry(
        /* [out] */ INotificationDataEntry** entry);

    CARAPI IsClearable(
        /* [out] */ Boolean* result);

    // ViewGroup methods
    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // View methods
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* c);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // ExpandHelper.Callback methods
    // @Override
    CARAPI GetChildAtRawPosition(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [out] */ IExpandableView** view);

    // @Override
    CARAPI GetChildAtPosition(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [out] */ IExpandableView** view);

    // @Override
    CARAPI CanChildBeExpanded(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetUserExpandedChild(
        /* [in] */ IView* v,
        /* [in] */ Boolean userExpanded);

    // @Override
    CARAPI SetUserLockedChild(
        /* [in] */ IView* v,
        /* [in] */ Boolean userLocked);

    // @Override
    CARAPI ExpansionStateChanged(
        /* [in] */ Boolean isExpanding);

    // SwipeHelper.Callback methods
    // @Override
    CARAPI CanChildBeDismissed(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsAntiFalsingNeeded(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetFalsingThresholdFactor(
        /* [out] */ Float* result);

    // @Override
    CARAPI OnChildDismissed(
        /* [in] */ IView* v);

    // @Override
    CARAPI OnBeginDrag(
        /* [in] */ IView* v);

    // @Override
    CARAPI OnDragCancelled(
        /* [in] */ IView* v);

    // @Override
    CARAPI OnChildSnappedBack(
        /* [in] */ IView* animView);

    // @Override
    CARAPI UpdateSwipeProgress(
        /* [in] */ IView* animView,
        /* [in] */ Boolean dismissable,
        /* [in] */ Float swipeProgress,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetChildAtPosition(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ IView** view);

    // @Override
    CARAPI GetChildContentView(
        /* [in] */ IView* v,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnComputeInternalInsets(
        /* [in] */ IInternalInsetsInfo* info);

    CARAPI Escalate();

    CARAPI GetKey(
        /* [out] */ String* key);

protected:
    // @Override
    CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean SPEW;
    static AutoPtr<IViewOutlineProvider> CONTENT_HOLDER_OUTLINE_PROVIDER;

    AutoPtr<IRect> mTmpRect;
    AutoPtr<ArrayOf<Int32> > mTmpTwoArray;

    Int32 mTouchSensitivityDelay;
    Float mMaxAlpha;
    AutoPtr<ISwipeHelper> mSwipeHelper;
    AutoPtr<EdgeSwipeHelper> mEdgeSwipeHelper;

    AutoPtr<IPhoneStatusBar> mBar;
    AutoPtr<IExpandHelper> mExpandHelper;

    Int64 mStartTouchTime;
    AutoPtr<IViewGroup> mContentHolder;

    AutoPtr<INotificationDataEntry> mHeadsUp;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CHEADSUPNOTIFICATIONVIEW_H__
