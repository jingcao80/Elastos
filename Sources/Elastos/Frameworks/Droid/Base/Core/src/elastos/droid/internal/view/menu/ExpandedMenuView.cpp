
#include "Elastos.Droid.Content.h"
#include "elastos/droid/internal/view/menu/ExpandedMenuView.h"
#include "elastos/droid/internal/view/menu/MenuBuilder.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Widget::IAdapter;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

//===============================================================================
//                  ExpandedMenuView::OnItemClickListener
//===============================================================================

CAR_INTERFACE_IMPL(ExpandedMenuView::OnItemClickListener, Object, IAdapterViewOnItemClickListener)

ExpandedMenuView::OnItemClickListener::OnItemClickListener(
    /* [in] */ ExpandedMenuView* owner)
    : mOwner(owner)
{}

ECode ExpandedMenuView::OnItemClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mOwner->OnItemClick(parent, v, position, id);
}

//===============================================================================
//                  ExpandedMenuView
//===============================================================================

CAR_INTERFACE_IMPL_3(ExpandedMenuView, ListView, IExpandedMenuView, IMenuBuilderItemInvoker, IMenuView)

ExpandedMenuView::ExpandedMenuView()
    : mAnimations(0)
{}

ECode ExpandedMenuView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(ListView::constructor(context, attrs))

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::MenuView);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, 0, 0, (ITypedArray**)&a);

    a->GetResourceId(R::styleable::MenuView_windowAnimationStyle, 0, &mAnimations);
    a->Recycle();

    AutoPtr<OnItemClickListener> listener = new OnItemClickListener(this);
    return SetOnItemClickListener(listener);
}

ECode ExpandedMenuView::Initialize(
    /* [in] */ IMenuBuilder* menu)
{
    mMenu = menu;
    return NOERROR;
}

ECode ExpandedMenuView::OnDetachedFromWindow()
{
    FAIL_RETURN(ListView::OnDetachedFromWindow())

    // Clear the cached bitmaps of children
    SetChildrenDrawingCacheEnabled(FALSE);
    return NOERROR;
}

ECode ExpandedMenuView::InvokeItem(
    /* [in] */ IMenuItemImpl* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mMenu->PerformItemAction(IMenuItem::Probe(item), 0, result);
}

ECode ExpandedMenuView::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* v,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IAdapter> adapter;
    GetAdapter((IAdapter**)&adapter);
    AutoPtr<IInterface> item;
    adapter->GetItem(position, (IInterface**)&item);
    Boolean res;
    return InvokeItem(IMenuItemImpl::Probe(item), &res);
}

ECode ExpandedMenuView::GetWindowAnimations(
    /* [out] */ Int32* animations)
{
    VALIDATE_NOT_NULL(animations)
    *animations = mAnimations;
    return NOERROR;
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
