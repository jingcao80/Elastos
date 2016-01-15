#include "elastos/droid/widget/CStackViewLayoutParams.h"

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEWGROUPLP_METHODS_IMPL(CStackViewLayoutParams, StackViewLayoutParams)

PInterface CStackViewLayoutParams::Probe(
    /* [in] */ REIID riid)
{
    return _CStackViewLayoutParams::Probe(riid);
}

ECode CStackViewLayoutParams::constructor(
    /* [in] */ IStackView* sv,
    /* [in] */ IView* view)
{
    return StackViewLayoutParams::Init(sv, view);
}

ECode CStackViewLayoutParams::constructor(
    /* [in] */ IStackView* sv,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return StackViewLayoutParams::Init(sv, context, attrs);
}

ECode CStackViewLayoutParams::GetInvalidateRect(
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    AutoPtr<IRect> r = StackViewLayoutParams::GetInvalidateRect();
    *rect = r;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode CStackViewLayoutParams::ResetInvalidateRect()
{
    return StackViewLayoutParams::ResetInvalidateRect();
}

ECode CStackViewLayoutParams::SetVerticalOffset(
    /* [in] */ Int32 newVerticalOffset)
{
    return StackViewLayoutParams::SetVerticalOffset(newVerticalOffset);
}

ECode CStackViewLayoutParams::GetVerticalOffset(
    /* [out] */ Int32* verticalOffset)
{
    VALIDATE_NOT_NULL(verticalOffset);
    *verticalOffset = StackViewLayoutParams::GetVerticalOffset();
    return NOERROR;
}

ECode CStackViewLayoutParams::SetHorizontalOffset(
    /* [in] */ Int32 newHorizontalOffset)
{
    return StackViewLayoutParams::SetHorizontalOffset(newHorizontalOffset);
}

ECode CStackViewLayoutParams::GetHorizontalOffset(
    /* [out] */ Int32* horizontalOffset)
{
    VALIDATE_NOT_NULL(horizontalOffset);
    *horizontalOffset = StackViewLayoutParams::GetHorizontalOffset();
    return NOERROR;
}

ECode CStackViewLayoutParams::SetOffsets(
    /* [in] */ Int32 newHorizontalOffset,
    /* [in] */ Int32 newVerticalOffset)
{
    return StackViewLayoutParams::SetOffsets(newHorizontalOffset, newVerticalOffset);
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos