
#ifndef __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__
#define __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__

#include "Elastos.Droid.Animation.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

class Tweener
    : public Object
{
private:
    class TweenerAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animation);
    };

public:
    Tweener(
        /* [in] */ IObjectAnimator* anim);

    static CARAPI To(
        /* [in] */ IInterface* object,
        /* [in] */ Int64 duration,
        /* [in] */ ArrayOf<IInterface*>* vars,
        /* [out] */ Tweener** result);

    CARAPI From(
        /* [in] */ IInterface* object,
        /* [in] */ Int64 duration,
        /* [in] */ ArrayOf<IInterface*>* vars,
        /* [out] */ Tweener** result);

    static CARAPI_(void) Reset();

    static CARAPI_(void) Replace(
        /* [in] */ IArrayList* props,
        /* [in] */ ArrayOf<IInterface*>* args);

private:
    static CARAPI_(void) Remove(
        /* [in] */ IAnimator* animator);

    static CARAPI_(AutoPtr<IHashMap>) Init_sTweens();

public:
    AutoPtr<IObjectAnimator> mAnimator;

private:
    static const String TAG;
    static const Boolean DEBUG;

    static AutoPtr<IHashMap> sTweens;

    // Listener to watch for completed animations and remove them.
    static AutoPtr<IAnimatorListener> sCleanupListener;
};

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TWEENER_H__
