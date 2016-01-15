
#ifndef __ELASTOS_DROID_TRANSITION_CTRANSITIONUTILS_H__
#define __ELASTOS_DROID_TRANSITION_CTRANSITIONUTILS_H__

#include "Elastos.Droid.Animation.h"
#include "_Elastos_Droid_Transition_CTransitionUtils.h"
#include <elastos/core/Object.h>
#include <elastos/core/Singleton.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * Static utility methods for Transitions.
 *
 * @hide
 */
CarClass(CTransitionUtils)
    , public Singleton
    , public ITransitionUtils
{
public:
    class MatrixEvaluator
        : public Object
        , public ITypeEvaluator
    {
    public:
        CAR_INTERFACE_DECL()

        MatrixEvaluator();

        CARAPI Evaluate(
            /* [in] */ Float fraction,
            /* [in] */ IInterface* startValue,
            /* [in] */ IInterface* endValue,
            /* [out] */ IInterface** result);

    public:
        AutoPtr<ArrayOf<Float> > mTempStartValues;

        AutoPtr<ArrayOf<Float> > mTempEndValues;

        AutoPtr<IMatrix> mTempMatrix;
    };

public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI MergeAnimators(
        /* [in] */ IAnimator* animator1,
        /* [in] */ IAnimator* animator2,
        /* [out] */ IAnimator** result);

    CARAPI MergeTransitions(
        /* [in] */ ArrayOf<ITransition*>* transitions,
        /* [out] */ ITransition** result);

    CARAPI CopyViewImage(
        /* [in] */ IViewGroup* sceneRoot,
        /* [in] */ IView* view,
        /* [in] */ IView* parent,
        /* [out] */ IView** result);

    CARAPI CreateViewBitmap(
        /* [in] */ IView* view,
        /* [in] */ IMatrix* matrix,
        /* [in] */ IRectF* bounds,
        /* [out] */ IBitmap** result);

private:
    static Int32 MAX_IMAGE_SIZE;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CTRANSITIONUTILS_H__
