#ifndef  __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICLINEARLAYOUT_H__
#define  __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICLINEARLAYOUT_H__

#include "_Elastos_Droid_Launcher2_CHolographicLinearLayout.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/HolographicViewHelper.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Launcher2::HolographicViewHelper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CHolographicLinearLayout)
    , public LinearLayout
    , public IHolographicLinearLayout
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL();

    CHolographicLinearLayout();

    CARAPI constructor();

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
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    AutoPtr<HolographicViewHelper> mHolographicHelper;
    AutoPtr<IImageView> mImageView;
    Int32 mImageViewId;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICLINEARLAYOUT_H__