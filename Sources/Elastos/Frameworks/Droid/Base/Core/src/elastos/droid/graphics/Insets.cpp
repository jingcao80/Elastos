
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/graphics/CRect.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Graphics {

const AutoPtr<Insets> Insets::NONE = new Insets(0, 0, 0, 0);

CAR_INTERFACE_IMPL(Insets, Object, IInsets);
Insets::Insets(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
    : mLeft(left)
    , mTop(top)
    , mRight(right)
    , mBottom(bottom)
{}

AutoPtr<Insets> Insets::Of(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (left == 0 && top == 0 && right == 0 && bottom == 0) {
        return NONE;
    }
    return new Insets(left, top, right, bottom);
}

AutoPtr<Insets> Insets::Of(
    /* [in] */ IRect* r)
{
    if (r == NULL) return NONE;

    CRect* rObj = (CRect*)r;
    return Of(rObj->mLeft, rObj->mTop, rObj->mRight, rObj->mBottom);
}

ECode Insets::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    *e = FALSE;
    if (THIS_PROBE(IInterface) == IInterface::Probe(o)) {
        *e = TRUE;
        return NOERROR;
    }

    ClassID id1, id2;
    GetClassID(&id1);
    if (o == NULL || id1 != (IObject::Probe(o)->GetClassID(&id2), id2)) {
        return NOERROR;
    }

    Insets* insets = (Insets*)IInsets::Probe(o);

    if (mBottom != insets->mBottom) return NOERROR;
    if (mLeft != insets->mLeft) return NOERROR;
    if (mRight != insets->mRight) return NOERROR;
    if (mTop != insets->mTop) return NOERROR;

    *e = TRUE;
    return NOERROR;
}

ECode Insets::GetHashCode(
    /* [out] */ Int32* hc)
{
    VALIDATE_NOT_NULL(hc);
    Int32 result = mLeft;
    result = 31 * result + mTop;
    result = 31 * result + mRight;
    result = 31 * result + mBottom;
    *hc = result;
    return NOERROR;
}

ECode Insets::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("Insets{left=");
    sb.Append(mLeft);
    sb.Append(", top=");
    sb.Append(mTop);
    sb.Append(", right=");
    sb.Append(mRight);
    sb.Append(", bottom=");
    sb.Append(mBottom);
    sb.AppendChar('}');
    return sb.ToString(str);
}

ECode Insets::GetLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left);
    *left = mLeft;
    return NOERROR;
}

ECode Insets::GetTop(
    /* [out] */ Int32* top)
{
    VALIDATE_NOT_NULL(top);
    *top = mTop;
    return NOERROR;
}

ECode Insets::GetRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right);
    *right = mRight;
    return NOERROR;
}

ECode Insets::GetBottom(
    /* [out] */ Int32* bottom)
{
    VALIDATE_NOT_NULL(bottom);
    *bottom = mBottom;
    return NOERROR;
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
