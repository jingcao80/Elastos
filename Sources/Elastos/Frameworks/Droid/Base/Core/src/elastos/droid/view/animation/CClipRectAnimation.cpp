
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CClipRectAnimation.h"
#include "elastos/droid/graphics/CRect.h"

using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_OBJECT_IMPL(CClipRectAnimation);

CClipRectAnimation::CClipRectAnimation()
{
    CRect::New((IRect**)&mFromRect);
    CRect::New((IRect**)&mToRect);
}

ECode CClipRectAnimation::constructor(
    /* [in] */ IRect* fromClip,
    /* [in] */ IRect* toClip)
{
    if (fromClip == NULL || toClip == NULL) {
        // throw new RuntimeException("Expected non-null animation clip rects");
        return E_RUNTIME_EXCEPTION;
    }
    mFromRect->Set(fromClip);
    return mToRect->Set(toClip);
}

void CClipRectAnimation::ApplyTransformation(
    /* [in] */ Float it,
    /* [in] */ ITransformation* tr)
{
    Int32 v1 = 0, v2 = 0;
    mFromRect->GetLeft(&v1);
    mToRect->GetLeft(&v2);
    Int32 l = v1 + (Int32) ((v2 - v1) * it);

    mFromRect->GetTop(&v1);
    mToRect->GetTop(&v2);
    Int32 t = v1 + (Int32) ((v2 - v1) * it);

    mFromRect->GetRight(&v1);
    mToRect->GetRight(&v2);
    Int32 r = v1 + (Int32) ((v2 - v1) * it);

    mFromRect->GetBottom(&v1);
    mToRect->GetBottom(&v2);
    Int32 b = v1 + (Int32) ((v2 - v1) * it);
    tr->SetClipRect(l, t, r, b);
}

ECode CClipRectAnimation::WillChangeTransformationMatrix(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CClipRectAnimation::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<CClipRectAnimation> obj;
    CClipRectAnimation::NewByFriend(NULL, NULL, (CClipRectAnimation**)&obj);
    CClipRectAnimation::CloneImpl(obj);
    *object = (IInterface*)IAnimation::Probe(obj);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CClipRectAnimation::CloneImpl(
    /* [in] */ CClipRectAnimation* object)
{
    Animation::CloneImpl(IAnimation::Probe(object));

    object->mFromRect = mFromRect;
    object->mToRect = mToRect;

    return NOERROR;
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
