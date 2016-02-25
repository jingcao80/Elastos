
#include "elastos/droid/systemui/recents/views/FixedSizeImageView.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

CAR_INTERFACE_IMPL(FixedSizeImageView, ImageView, IFixedSizeImageView)

FixedSizeImageView::FixedSizeImageView()
    : mAllowRelayout(TRUE)
    , mAllowInvalidate(TRUE)
{}

ECode FixedSizeImageView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode FixedSizeImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode FixedSizeImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode FixedSizeImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return ImageView::constructor(context, attrs, defStyleAttr, defStyleRes);
}

// @Override
ECode FixedSizeImageView::RequestLayout()
{
    if (mAllowRelayout) {
        ImageView::RequestLayout();
    }
    return NOERROR;
}

// @Override
ECode FixedSizeImageView::Invalidate()
{
    if (mAllowInvalidate) {
        ImageView::Invalidate();
    }
    return NOERROR;
}

// @Override
ECode FixedSizeImageView::SetImageDrawable(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IBitmap> bitmap;
    Boolean isNullBitmapDrawable = IBitmapDrawable::Probe(drawable) != NULL &&
        (IBitmapDrawable::Probe(drawable)->GetBitmap((IBitmap**)&bitmap), bitmap == NULL);
    if (drawable == NULL || isNullBitmapDrawable) {
        mAllowRelayout = FALSE;
        mAllowInvalidate = FALSE;
    }
    ImageView::SetImageDrawable(drawable);
    mAllowRelayout = TRUE;
    mAllowInvalidate = TRUE;
    return NOERROR;
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
