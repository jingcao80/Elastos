
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_AMBIENTSTATE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_AMBIENTSTATE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Stack {

/**
 * A global state to track all input states for the algorithm.
 */
class AmbientState
    : public Object
    , public IAmbientState
{
public:
    CAR_INTERFACE_DECL()

    AmbientState();

    CARAPI GetScrollY(
        /* [out] */ Int32* y);

    CARAPI SetScrollY(
        /* [in] */ Int32 scrollY);

    CARAPI OnBeginDrag(
        /* [in] */ IView* view);

    CARAPI OnDragFinished(
        /* [in] */ IView* view);

    CARAPI GetDraggedViews(
        /* [out] */ IArrayList** list);

    /**
     * @param dimmed Whether we are in a dimmed state (on the lockscreen), where the backgrounds are
     *               translucent and everything is scaled back a bit.
     */
    CARAPI SetDimmed(
        /* [in] */ Boolean dimmed);

    /** In dark mode, we draw as little as possible, assuming a black background */
    CARAPI SetDark(
        /* [in] */ Boolean dark);

    CARAPI SetHideSensitive(
        /* [in] */ Boolean hideSensitive);

    /**
     * In dimmed mode, a child can be activated, which happens on the first tap of the double-tap
     * interaction. This child is then scaled normally and its background is fully opaque.
     */
    CARAPI SetActivatedChild(
        /* [in] */ IActivatableNotificationView* activatedChild);

    CARAPI IsDimmed(
        /* [out] */ Boolean* result);

    CARAPI IsDark(
        /* [out] */ Boolean* result);

    CARAPI IsHideSensitive(
        /* [out] */ Boolean* result);

    CARAPI GetActivatedChild(
        /* [out] */ IActivatableNotificationView** view);

    CARAPI SetOverScrollAmount(
        /* [in] */ Float amount,
        /* [in] */ Boolean onTop);

    CARAPI GetOverScrollAmount(
        /* [in] */ Boolean top,
        /* [out] */ Float* result);

    CARAPI GetSpeedBumpIndex(
        /* [out] */ Int32* index);

    CARAPI SetSpeedBumpIndex(
        /* [in] */ Int32 speedBumpIndex);

private:
    AutoPtr<IArrayList> mDraggedViews;  /*<View*/
    Int32 mScrollY;
    Boolean mDimmed;
    AutoPtr<IActivatableNotificationView> mActivatedChild;
    Float mOverScrollTopAmount;
    Float mOverScrollBottomAmount;
    Int32 mSpeedBumpIndex;
    Boolean mDark;
    Boolean mHideSensitive;
};

} // namespace Stack
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_STACK_AMBIENTSTATE_H__
