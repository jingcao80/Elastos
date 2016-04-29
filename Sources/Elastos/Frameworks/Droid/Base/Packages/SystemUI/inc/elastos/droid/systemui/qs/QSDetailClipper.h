
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_QSDETAILCLIPPER_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_QSDETAILCLIPPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Graphics::Drawable::ITransitionDrawable;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/** Helper for quick settings detail panel clip animations. **/
class QSDetailClipper: public Object
{
private:
    class ReverseBackground: public Runnable
    {
    public:
        ReverseBackground(
            /* [in] */ QSDetailClipper* host);

        // @Override
        CARAPI Run();

    private:
        QSDetailClipper* mHost;
    };

    class VisibleOnStart: public AnimatorListenerAdapter
    {
    public:
        VisibleOnStart(
            /* [in] */ QSDetailClipper* host);

        // @Override
        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        QSDetailClipper* mHost;
    };

    class GoneOnEnd: public AnimatorListenerAdapter
    {
    public:
        GoneOnEnd(
            /* [in] */ QSDetailClipper* host);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        QSDetailClipper* mHost;
    };

public:
    QSDetailClipper(
        /* [in] */ IView* detail);

    CARAPI_(void) AnimateCircularClip(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Boolean in,
        /* [in] */ IAnimatorListener* listener);

private:
    AutoPtr<IView> mDetail;
    AutoPtr<ITransitionDrawable> mBackground;

    AutoPtr<IAnimator> mAnimator;

    AutoPtr<IRunnable> mReverseBackground;
    AutoPtr<VisibleOnStart> mVisibleOnStart;
    AutoPtr<GoneOnEnd> mGoneOnEnd;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_QSDETAILCLIPPER_H__
