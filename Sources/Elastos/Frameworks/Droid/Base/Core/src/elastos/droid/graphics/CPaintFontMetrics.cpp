
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CPaintFontMetrics.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPaintFontMetrics);
CAR_INTERFACE_IMPL(CPaintFontMetrics, Object, IPaintFontMetrics);
ECode CPaintFontMetrics::GetTop(
    /* [out] */ Float* top)
{
    VALIDATE_NOT_NULL(top);
    *top = mTop;
    return NOERROR;
}

ECode CPaintFontMetrics::SetTop(
    /* [in] */ Float top)
{
    mTop = top;
    return NOERROR;
}

ECode CPaintFontMetrics::GetAscent(
    /* [out] */ Float* ascent)
{
    VALIDATE_NOT_NULL(ascent);
    *ascent = mAscent;
    return NOERROR;
}

ECode CPaintFontMetrics::SetAscent(
    /* [in] */ Float ascent)
{
    mAscent = ascent;
    return NOERROR;
}

ECode CPaintFontMetrics::GetDescent(
    /* [out] */ Float* descent)
{
    VALIDATE_NOT_NULL(descent);
    *descent = mDescent;
    return NOERROR;
}

ECode CPaintFontMetrics::SetDescent(
    /* [in] */ Float descent)
{
    mDescent = descent;
    return NOERROR;
}

ECode CPaintFontMetrics::GetBottom(
    /* [out] */ Float* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = mBottom;
    return NOERROR;
}

ECode CPaintFontMetrics::SetBottom(
    /* [in] */ Float bottom)
{
    mBottom = bottom;
    return NOERROR;
}

ECode CPaintFontMetrics::GetLeading(
    /* [out] */ Float* leading)
{
    VALIDATE_NOT_NULL(leading);
    *leading = mLeading;
    return NOERROR;
}

ECode CPaintFontMetrics::SetLeading(
    /* [in] */ Float leading)
{
    mLeading = leading;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
