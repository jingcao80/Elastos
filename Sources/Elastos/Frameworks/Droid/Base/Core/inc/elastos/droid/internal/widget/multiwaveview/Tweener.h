
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_TWEENER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_TWEENER_H__

#include "Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Internal::Widget::Multiwaveview::ITweener;

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

class Tweener
    : public Object
    , public ITweener
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
    CAR_INTERFACE_DECL()

    Tweener(
        /* [in] */ IObjectAnimator* anim);

    static CARAPI To(
        /* [in] */ IInterface* object,
        /* [in] */ Int64 duration,
        /* [in] */ ArrayOf<IInterface*>* vars,
        /* [out] */ ITweener** result);

    CARAPI From(
        /* [in] */ IInterface* object,
        /* [in] */ Int64 duration,
        /* [in] */ ArrayOf<IInterface*>* vars,
        /* [out] */ ITweener** result);

    static CARAPI Reset();

    static CARAPI_(void) Remove(
        /* [in] */ IAnimator* animator);

    static CARAPI_(void) Replace(
        /* [in] */ IArrayList* props,
        /* [in] */ ArrayOf<IInterface*>* args);

public:
    AutoPtr<IObjectAnimator> mAnimator;

private:
    // Listener to watch for completed animations and remove them.
    static AutoPtr<IAnimatorListener> mCleanupListener;

    static const String TAG;
    static const Boolean DEBUG;

    static AutoPtr<IHashMap> sTweens;
};

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_MULTIWAVEVIEW_TWEENER_H__
