
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWFOOTER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWFOOTER_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/** The task footer view */
class TaskViewFooter
    : public FrameLayout
    , public ITaskViewFooter
{
public:
    TaskViewFooter();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /** Sets the callbacks for when the footer height changes. */
    CARAPI SetCallbacks(
        /* [in] */ ITaskFooterViewCallbacks* cb);

    /** Sets the footer height. */
    CARAPI SetFooterHeight(
        /* [in] */ Int32 footerHeight);

    /** Gets the footer height. */
    CARAPI GetFooterHeight(
        /* [out] */ Int32* height);

    /** Animates the footer into and out of view. */
    CARAPI AnimateFooterVisibility(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 duration);

public:
    AutoPtr<RecentsConfiguration> mConfig;

    AutoPtr<ITaskFooterViewCallbacks> mCb;
    Int32 mFooterHeight;
    Int32 mMaxFooterHeight;
    AutoPtr<IAnimator> mFooterAnimator; //IObjectAnimator
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_TASKVIEWFOOTER_H__
