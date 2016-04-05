
#include "elastos/droid/launcher2/CDrawableStateProxyView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_OBJECT_IMPL(CDrawableStateProxyView);

CDrawableStateProxyView::CDrawableStateProxyView()
    : mViewId(0)
{
}

ECode CDrawableStateProxyView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CDrawableStateProxyView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CDrawableStateProxyView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(Elastos::Droid::Launcher2::R::styleable::DrawableStateProxyView),
            ArraySize(Elastos::Droid::Launcher2::R::styleable::DrawableStateProxyView));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);
    a->GetResourceId(
            Elastos::Droid::Launcher2::R::styleable::DrawableStateProxyView_sourceViewId,
            -1, &mViewId);
    a->Recycle();

    return SetFocusable(FALSE);
}

ECode CDrawableStateProxyView::DrawableStateChanged()
{
    LinearLayout::DrawableStateChanged();

    if (mView == NULL) {
        AutoPtr<IViewParent> res;
        GetParent((IViewParent**)&res);
        AutoPtr<IView> parent = IView::Probe(res);
        parent->FindViewById(mViewId, (IView**)&mView);
    }
    Boolean res;
    IsPressed(&res);
    mView->SetPressed(res);
    IsHovered(&res);
    return mView->SetHovered(res);
}

ECode CDrawableStateProxyView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos