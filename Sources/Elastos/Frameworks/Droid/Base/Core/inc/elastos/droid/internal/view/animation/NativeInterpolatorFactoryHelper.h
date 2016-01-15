
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_NATIVEINTERPOLATORFACTORYHELPER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_NATIVEINTERPOLATORFACTORYHELPER_H__

#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Animation {

/**
 * Static utility class for constructing native interpolators to keep the
 * JNI simpler
 */
class NativeInterpolatorFactoryHelper
{
public:
    static CARAPI_(Int64) CreateAccelerateDecelerateInterpolator();

    static CARAPI_(Int64) CreateAccelerateInterpolator(
        /* [in] */ Float factor);

    static CARAPI_(Int64) CreateAnticipateInterpolator(
        /* [in] */ Float tension);

    static CARAPI_(Int64) CreateAnticipateOvershootInterpolator(
        /* [in] */ Float tension);

    static CARAPI_(Int64) CreateBounceInterpolator();

    static CARAPI_(Int64) CreateCycleInterpolator(
        /* [in] */ Float cycles);

    static CARAPI_(Int64) CreateDecelerateInterpolator(
        /* [in] */ Float factor);

    static CARAPI_(Int64) CreateLinearInterpolator();

    static CARAPI_(Int64) CreateOvershootInterpolator(
        /* [in] */ Float tension);

    static CARAPI_(Int64) CreateLutInterpolator(
        /* [in] */ ArrayOf<Float>* values);

private:
    NativeInterpolatorFactoryHelper() {}
};

}   //namespace Animation
}   //namespace View
}   //namespace Internal
}   //namespace Droid
}   //namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_VIEW_ANIMATION_NATIVEINTERPOLATORFACTORYHELPER_H__
