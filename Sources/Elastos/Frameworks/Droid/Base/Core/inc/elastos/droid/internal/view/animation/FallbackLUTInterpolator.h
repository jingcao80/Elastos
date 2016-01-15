
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_FALLBACKLUTINTERPOLATOR_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_FALLBACKLUTINTERPOLATOR_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::ITimeInterpolator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Animation {

/**
 * Interpolator that builds a lookup table to use. This is a fallback for
 * building a native interpolator from a TimeInterpolator that is not marked
 * with {@link HasNativeInterpolator}
 *
 * This implements TimeInterpolator to allow for easier interop with Animators
 */
// @HasNativeInterpolator
class FallbackLUTInterpolator
    : public Object
    , public IFallbackLUTInterpolator
    , public INativeInterpolatorFactory
    , public ITimeInterpolator
{
public:
    /**
     * Used to cache the Float[] LUT for use across multiple native
     * interpolator creation
     */
    CARAPI constructor(
        /* [in] */ ITimeInterpolator* interpolator,
        /* [in] */ Int64 duration);

    // @Override
    CARAPI CreateNativeInterpolator(
        /* [out] */ Int64* result);

    /**
     * Used to create a one-shot Float[] LUT & native interpolator
     */
    static CARAPI_(Int64) CreateNativeInterpolator(
        /* [in] */ ITimeInterpolator* interpolator,
        /* [in] */ Int64 duration);

    // @Override
    CARAPI GetInterpolation(
        /* [in] */ Float input,
        /* [out] */ Float* result);

private:
    static CARAPI_(AutoPtr<ArrayOf<Float> >) CreateLUT(
        /* [in] */ ITimeInterpolator* interpolator,
        /* [in] */ Int64 duration);

private:
    AutoPtr<ITimeInterpolator> mSourceInterpolator;
    AutoPtr<ArrayOf<Float> > mLut;
};

} //namespace Animation
} //namespace View
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_FALLBACKLUTINTERPOLATOR_H__
