#ifndef  __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICLINEARLAYOUT_H__
#define  __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICLINEARLAYOUT_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class HolographicLinearLayout
    : public LinearLayout
    , public IHolographicLinearLayout
{
public:
    CAR_INTERFACE_DECL();

    HolographicLinearLayout();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InvalidatePressedFocusedStates();

protected:
    //@Override
    CARAPI DrawableStateChanged();

    //@Override
    CARAPI OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    AutoPtr<HolographicViewHelper> mHolographicHelper;
    AutoPtr<IImageView> mImageView;
    Int32 mImageViewId;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HOLOGRAPHICLINEARLAYOUT_H__