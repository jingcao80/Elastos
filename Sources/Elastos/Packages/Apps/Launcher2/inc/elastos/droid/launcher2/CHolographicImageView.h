#ifndef  __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICIMAGEVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICIMAGEVIEW_H__

#include "_Elastos_Droid_Launcher2_CHolographicImageView.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/HolographicViewHelper.h"
#include "elastos/droid/widget/ImageView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CHolographicImageView)
    , public ImageView
    , public IHolographicImageView
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

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
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    AutoPtr<HolographicViewHelper> mHolographicHelper;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CHOLOGRAPHICIMAGEVIEW_H__