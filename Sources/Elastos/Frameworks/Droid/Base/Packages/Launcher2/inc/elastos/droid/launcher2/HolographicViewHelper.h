#ifndef  __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICVIEWHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICVIEWHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * The ListAdapter which presents all the valid widgets that can be created for a given drop.
 */
class HolographicViewHelper
    : public Object
{
public:
    HolographicViewHelper(
        /* [in] */ IContext* context);

    /**
     * Generate the pressed/focused states if necessary.
     */
    CARAPI_(void) GeneratePressedFocusedStates(
        /* [in] */ IImageView* v);

    /**
     * Invalidates the pressed/focused states.
     */
    CARAPI_(void) InvalidatePressedFocusedStates(
        /* [in] */ IImageView* v);

    /**
     * Creates a copy of the original image.
     */
private:
    CARAPI_(AutoPtr<IBitmap>) CreateOriginalImage(
        /* [in] */ IImageView* v,
        /* [in] */ ICanvas* canvas);

    /**
     * Creates a new press state image which is the old image with a blue overlay.
     * Responsibility for the bitmap is transferred to the caller.
     */
    CARAPI_(AutoPtr<IBitmap>) CreatePressImage(
        /* [in] */ IImageView* v,
        /* [in] */ ICanvas* canvas);

private:
    AutoPre<ICanvas> mTempCanvas = new Canvas();

    Boolean mStatesUpdated;
    Int32 mHighlightColor;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICVIEWHELPER_H__