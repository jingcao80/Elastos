
#include "elastos/droid/systemui/statusbar/CAlphaImageView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CAlphaImageView);
CAR_INTERFACE_IMPL(CAlphaImageView, ImageView, IAlphaImageView);
ECode CAlphaImageView::constructor(
    /* [in] */ IContext* context)
{
    return ImageView::constructor(context);
}

ECode CAlphaImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ImageView::constructor(context, attrs);
}

ECode CAlphaImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return ImageView::constructor(context, attrs, defStyleAttr);
}

ECode CAlphaImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return ImageView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

ECode CAlphaImageView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
