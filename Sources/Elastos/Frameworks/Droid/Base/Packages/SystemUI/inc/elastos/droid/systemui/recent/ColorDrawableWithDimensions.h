
#ifndef __ELASTOS_DROID_SYSTEMUI_RECENT_COLORDRAWABLEWITHDIMENSIONS_H__
#define __ELASTOS_DROID_SYSTEMUI_RECENT_COLORDRAWABLEWITHDIMENSIONS_H__

#include "_SystemUI.h"
#include "elastos/droid/graphics/drawable/ColorDrawable.h"

using Elastos::Droid::Graphics::Drawable::ColorDrawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

class ColorDrawableWithDimensions
    : public ColorDrawable
{
public:
    ColorDrawableWithDimensions();

    CARAPI constructor(
        /* [in] */ Int32 color,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    // @Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    // @Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

private:
    Int32 mWidth;
    Int32 mHeight;
};

} // namespace Recent
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_RECENT_COLORDRAWABLEWITHDIMENSIONS_H__