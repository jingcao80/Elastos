
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CBoringLayoutMetrics.h"

using Elastos::Droid::Graphics::EIID_IPaintFontMetricsInt;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_2(CBoringLayoutMetrics, Object, IBoringLayoutMetrics, IPaintFontMetricsInt)

CAR_OBJECT_IMPL(CBoringLayoutMetrics)

CBoringLayoutMetrics::CBoringLayoutMetrics()
    : mTop(0)
    , mAscent(0)
    , mDescent(0)
    , mBottom(0)
    , mLeading(0)
    , mWidth(0)
{
}

ECode CBoringLayoutMetrics::constructor()
{
    return NOERROR;
}

ECode CBoringLayoutMetrics::GetTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);

    *top = mTop;
    return NOERROR;
}

ECode CBoringLayoutMetrics::SetTop(
    /* [in] */ Int32 top)
{
    mTop = top;
    return NOERROR;
}

ECode CBoringLayoutMetrics::GetAscent(
    /* [out] */ Int32* ascent)
{
    VALIDATE_NOT_NULL(ascent);

    *ascent = mAscent;
    return NOERROR;
}

ECode CBoringLayoutMetrics::SetAscent(
    /* [in] */ Int32 ascent)
{
    mAscent = ascent;
    return NOERROR;
}

ECode CBoringLayoutMetrics::GetDescent(
    /* [out] */ Int32* descent)
{
    VALIDATE_NOT_NULL(descent);

    *descent = mDescent;
    return NOERROR;
}

ECode CBoringLayoutMetrics::SetDescent(
    /* [in] */ Int32 descent)
{
    mDescent = descent;
    return NOERROR;
}

ECode CBoringLayoutMetrics::GetBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);

    *bottom = mBottom;
    return NOERROR;
}

ECode CBoringLayoutMetrics::SetBottom(
    /* [in] */ Int32 bottom)
{
    mBottom = bottom;
    return NOERROR;
}

ECode CBoringLayoutMetrics::GetLeading(
    /* [out] */ Int32* leading)
{
    VALIDATE_NOT_NULL(leading);

    *leading = mLeading;
    return NOERROR;
}

ECode CBoringLayoutMetrics::SetLeading(
    /* [in] */ Int32 leading)
{
    mLeading = leading;
    return NOERROR;
}

ECode CBoringLayoutMetrics::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mWidth;
    return NOERROR;
}

ECode CBoringLayoutMetrics::SetWidth(
    /* [in] */ Int32 width)
{
    mWidth = width;
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
