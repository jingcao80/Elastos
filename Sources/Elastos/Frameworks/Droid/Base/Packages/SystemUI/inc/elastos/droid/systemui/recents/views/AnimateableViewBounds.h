
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_ANIMATEABLEVIEWBOUNDS_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_ANIMATEABLEVIEWBOUNDS_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/droid/view/ViewOutlineProvider.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewOutlineProvider;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/* An outline provider that has a clip and outline that can be animated. */
class AnimateableViewBounds : public ViewOutlineProvider
{
public:
    AnimateableViewBounds(
        /* [in] */ ITaskView* source,
        /* [in] */ Int32 cornerRadius);

    // @Override
    CARAPI GetOutline(
        /* [in] */ IView* view,
        /* [in] */ IOutline* outline);

    /** Sets the view outline alpha. */
    CARAPI_(void) SetAlpha(
        /* [in] */ Float alpha);

    /** Animates the top clip. */
    CARAPI_(void) AnimateClipTop(
        /* [in] */ Int32 top,
        /* [in] */ Int32 duration,
        /* [in] */ IAnimatorUpdateListener* updateListener);

    /** Sets the top clip. */
    CARAPI_(void) SetClipTop(
        /* [in] */ Int32 top);

    /** Returns the top clip. */
    CARAPI_(Int32) GetClipTop();

    /** Animates the right clip. */
    CARAPI_(void) AnimateClipRight(
        /* [in] */ Int32 right,
        /* [in] */ Int32 duration);

    /** Sets the right clip. */
    CARAPI_(void) SetClipRight(
        /* [in] */ Int32 right);

    /** Returns the right clip. */
    CARAPI_(Int32) GetClipRight();

    /** Animates the bottom clip. */
    CARAPI_(void) AnimateClipBottom(
        /* [in] */ Int32 bottom,
        /* [in] */ Int32 duration);

    /** Sets the bottom clip. */
    CARAPI_(void) SetClipBottom(
        /* [in] */ Int32 bottom);

    /** Returns the bottom clip. */
    CARAPI_(Int32) GetClipBottom();

    /** Sets the outline bottom clip. */
    CARAPI_(void) SetOutlineClipBottom(
        /* [in] */ Int32 bottom);

    /** Gets the outline bottom clip. */
    CARAPI_(Int32) GetOutlineClipBottom();

private:
    CARAPI_(void) UpdateClipBounds();

public:
    AutoPtr<RecentsConfiguration> mConfig;

    AutoPtr<IView> mSourceView; // ITaskView
    AutoPtr<IRect> mTmpRect;
    AutoPtr<IRect> mClipRect;
    AutoPtr<IRect> mClipBounds;
    AutoPtr<IRect> mOutlineClipRect;
    Int32 mCornerRadius;
    Float mAlpha;
    Float mMinAlpha;

    AutoPtr<IAnimator> mClipTopAnimator; //IObjectAnimator
    AutoPtr<IAnimator> mClipRightAnimator; //IObjectAnimator
    AutoPtr<IAnimator> mClipBottomAnimator; //IObjectAnimator
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_ANIMATEABLEVIEWBOUNDS_H__
