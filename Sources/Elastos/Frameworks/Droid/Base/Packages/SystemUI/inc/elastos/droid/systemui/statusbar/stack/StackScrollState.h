
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSCROLLSTATE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSCROLLSTATE_H__

#include "_SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::Object;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * A state of a {@link com.android.systemui.statusbar.stack.NotificationStackScrollLayout} which
 * can be applied to a viewGroup.
 */
class StackScrollState
    : public Object
    , public IStackScrollState
{
public:
    class ViewState: public Object
    {
    public:
        ViewState();

    public:
        // These are flags such that we can create masks for filtering.
        static const Int32 LOCATION_UNKNOWN;
        static const Int32 LOCATION_FIRST_CARD;
        static const Int32 LOCATION_TOP_STACK_HIDDEN;
        static const Int32 LOCATION_TOP_STACK_PEEKING;
        static const Int32 LOCATION_MAIN_AREA;
        static const Int32 LOCATION_BOTTOM_STACK_PEEKING;
        static const Int32 LOCATION_BOTTOM_STACK_HIDDEN;

        Float mAlpha;
        Float mYTranslation;
        Float mZTranslation;
        Int32 mHeight;
        Boolean mGone;
        Float mScale;
        Boolean mDimmed;
        Boolean mDark;
        Boolean mHideSensitive;
        Boolean mBelowSpeedBump;

        /**
         * The amount which the view should be clipped from the top. This is calculated to
         * perceive consistent shadows.
         */
        Int32 mClipTopAmount;

        /**
         * How much does the child overlap with the previous view on the top? Can be used for
         * a clipping optimization
         */
        Int32 mTopOverLap;

        /**
         * The index of the view, only accounting for views not equal to GONE
         */
        Int32 mNotGoneIndex;

        /**
         * The location this view is currently rendered at.
         *
         * <p>See <code>LOCATION_</code> flags.</p>
         */
        Int32 mLocation;
    };

public:
    CAR_INTERFACE_DECL();

    StackScrollState(
        /* [in] */ IViewGroup* hostView);

    CARAPI GetHostView(
        /* [out] */ IViewGroup** vg);

    CARAPI ResetViewStates();

    CARAPI GetViewStateForView(
        /* [in] */ IView* requestedView,
        /* [out] */ IInterface** vs);

    CARAPI RemoveViewStateForView(
        /* [in] */ IView* child);

    /**
     * Apply the properties saved in {@link #mStateMap} to the children of the {@link #mHostView}.
     * The properties are only applied if they effectively changed.
     */
    CARAPI Apply();

    CARAPI PerformSpeedBumpAnimation(
        /* [in] */ Int32 i,
        /* [in] */ ISpeedBumpView* speedBump,
        /* [in] */ IInterface* state,
        /* [in] */ Int64 delay);

private:
    /**
     * Updates the clipping of a view
     *
     * @param child the view to update
     * @param height the currently applied height of the view
     * @param clipInset how much should this view be clipped from the top
     */
    CARAPI_(void) UpdateChildClip(
        /* [in] */ IView* child,
        /* [in] */ Int32 height,
        /* [in] */ Int32 clipInset);

    CARAPI_(AutoPtr<IView>) GetNextChildNotGone(
        /* [in] */ Int32 childIndex);

private:
    static const String CHILD_NOT_FOUND_TAG;

    AutoPtr<IViewGroup> mHostView;
    AutoPtr<IMap> mStateMap;  /*<ExpandableView, ViewState*/
    AutoPtr<IRect> mClipRect;
    Int32 mClearAllTopPadding;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_STACKSCROLLSTATE_H__
