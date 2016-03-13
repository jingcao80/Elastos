
#include "elastos/droid/launcher2/PagedViewWidgetImageView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(PagedViewWidgetImageView, ImageView, IPagedViewWidgetImageView);

PagedViewWidgetImageView::PagedViewWidgetImageView()
    : mAllowRequestLayout(TRUE)
{
}

ECode PagedViewWidgetImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ImageView::constructor(context, attrs);
}

ECode PagedViewWidgetImageView::RequestLayout()
{
    if (mAllowRequestLayout) {
        return ImageView::RequestLayout();
    }
    return NOERROR;
}

ECode PagedViewWidgetImageView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    canvas->Save();
    Int32 x;
    GetScrollX(&x);
    Int32 pleft;
    GetPaddingLeft(&pleft);
    Int32 y;
    GetScrollY(&y);
    Int32 ptop;
    GetPaddingTop(&ptop);
    Int32 right;
    GetRight(&right);
    Int32 left;
    GetLeft(&left);
    Int32 pright;
    GetPaddingRight(&pright);
    Int32 bottom;
    GetBottom(&bottom);
    Int32 top;
    GetTop(&top);
    Int32 pbottom;
    GetPaddingBottom(&pbottom);
    canvas->ClipRect(x + pleft,
            y + ptop,
            x + right - left - pright,
            y + bottom - top - pbottom);

    super->OnDraw(canvas);
    return canvas->Restore();
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos