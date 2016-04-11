
#include "Elastos.Droid.Content.h"
#include "elastos/droid/view/ViewStub.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(ViewStub, View, IViewStub);

ViewStub::ViewStub()
    : mLayoutResource(0)
    , mInflatedId(0)
{}

ViewStub::~ViewStub()
{}

ECode ViewStub::constructor(
    /* [in] */ IContext* context)
{
    Initialize(context);
    return NOERROR;
}

ECode ViewStub::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 layoutResource)
{
    mLayoutResource = layoutResource;
    Initialize(context);
    return NOERROR;
}

ECode ViewStub::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode ViewStub::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode ViewStub::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AutoPtr< ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::ViewStub), ArraySize(R::styleable::ViewStub));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    a->GetResourceId(R::styleable::ViewStub_inflatedId, IView::NO_ID, &mInflatedId);
    a->GetResourceId(R::styleable::ViewStub_layout, 0, &mLayoutResource);

    a->Recycle();

    a = NULL;
    attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::View), ArraySize(R::styleable::View));
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    a->GetResourceId(R::styleable::View_id, IView::NO_ID, &mID);
    a->Recycle();

    Initialize(context);
    return NOERROR;
}

void ViewStub::Initialize(
    /* [in] */ IContext* context)
{
    mContext = context;
    // see CContextThemeWrapper::New in LayoutInflater::CreateViewFromTag
    if (IContextThemeWrapperInLayoutInflater::Probe(context) != NULL) {
        mContext->AddRef();
    }
    SetVisibility(IView::GONE);
    SetWillNotDraw(TRUE);
}

ECode ViewStub::GetInflatedId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mInflatedId;
    return NOERROR;
}

ECode ViewStub::SetInflatedId(
    /* [in] */ Int32 inflatedId)
{
    mInflatedId = inflatedId;
    return NOERROR;
}

ECode ViewStub::GetLayoutResource(
    /* [out] */ Int32* resource)
{
    VALIDATE_NOT_NULL(resource);
    *resource = mLayoutResource;
    return NOERROR;
}

ECode ViewStub::SetLayoutResource(
    /* [in] */ Int32 layoutResource)
{
    mLayoutResource = layoutResource;
    return NOERROR;
}

ECode ViewStub::SetLayoutInflater(
    /* [in] */ ILayoutInflater* inflater)
{
    mInflater = inflater;
    return NOERROR;
}

ECode ViewStub::GetLayoutInflater(
    /* [out] */ ILayoutInflater** inflater)
{
    VALIDATE_NOT_NULL(inflater);
    *inflater = mInflater;
    REFCOUNT_ADD(*inflater);
    return NOERROR;
}

void ViewStub::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    SetMeasuredDimension(0, 0);
}

ECode ViewStub::Draw(
    /* [in] */ ICanvas* canvas)
{
    return NOERROR;
}

ECode ViewStub::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    return NOERROR;
}

ECode ViewStub::SetVisibility(
    /* [in] */ Int32 visibility)
{
    if (mInflatedViewRef != NULL) {
        AutoPtr<IView> view;
        mInflatedViewRef->Resolve(EIID_IView, (IInterface**)&view);
        if (view != NULL) {
            view->SetVisibility(visibility);
        }
        else {
            Slogger::E("ViewStub", "setVisibility called on un-referenced view");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    else {
        View::SetVisibility(visibility);
        if (visibility == IView::VISIBLE || visibility == IView::INVISIBLE) {
            AutoPtr<IView> tmp;
            Inflate((IView**)&tmp);
        }
    }

    return NOERROR;
}

ECode ViewStub::Inflate(
    /* [out] */ IView** retView)
{
    VALIDATE_NOT_NULL(retView);
    AutoPtr<IViewParent> viewParent;
    GetParent((IViewParent**)&viewParent);

    IViewGroup* parent = IViewGroup::Probe(viewParent);
    if (parent != NULL) {
        if (mLayoutResource != 0) {
            AutoPtr<ILayoutInflater> factory;
            if (mInflater != NULL) {
                factory = mInflater;
            }
            else {
                LayoutInflater::From(mContext, (ILayoutInflater**)&factory);
            }

            AutoPtr<IView> view;
            factory->Inflate(mLayoutResource, parent, FALSE, (IView**)&view);

            if (mInflatedId != IView::NO_ID) {
                view->SetId(mInflatedId);
            }

            Int32 index = 0;
            parent->IndexOfChild(this, &index);
            parent->RemoveViewInLayout(this);

            AutoPtr<IViewGroupLayoutParams> layoutParams;
            GetLayoutParams((IViewGroupLayoutParams**)&layoutParams);
            if (layoutParams != NULL) {
                parent->AddView(view, index, layoutParams);
            }
            else {
                parent->AddView(view, index);
            }

            IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(view);
            assert(wrs != NULL);
            mInflatedViewRef = NULL;
            wrs->GetWeakReference((IWeakReference**)&mInflatedViewRef);

            if (mInflateListener != NULL) {
                mInflateListener->OnInflate(this, view);
            }

            *retView = view;
            REFCOUNT_ADD(*retView);
            return NOERROR;
        }
        else {
            Slogger::E("ViewStub", "ViewStub must have a valid layoutResource");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    else {
        Slogger::E("ViewStub", "ViewStub must have a non-null ViewGroup viewParent");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode ViewStub::SetOnInflateListener(
    /* [in] */ IViewStubOnInflateListener* inflateListener)
{
    mInflateListener = inflateListener;
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
