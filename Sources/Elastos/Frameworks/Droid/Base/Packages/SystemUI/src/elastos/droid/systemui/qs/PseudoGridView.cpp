
#include "elastos/droid/systemui/qs/PseudoGridView.h"
#include "elastos/droid/systemui/qs/CViewGroupAdapterBridge.h"
#include "../R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::Widget::IAdapter;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

static const String TAG("PseudoGridView");

//===============================================================================
// PseudoGridView::ViewGroupAdapterBridge
//===============================================================================
CAR_OBJECT_IMPL(CViewGroupAdapterBridge)

PseudoGridView::ViewGroupAdapterBridge::ViewGroupAdapterBridge()
    : mReleased(FALSE)
{
}

PseudoGridView::ViewGroupAdapterBridge::~ViewGroupAdapterBridge()
{
    Logger::I(TAG, " >>> destory ViewGroupAdapterBridge %p", this);
}

ECode PseudoGridView::ViewGroupAdapterBridge::constructor(
    /* [in] */ IViewGroup* viewGroup,
    /* [in] */ IBaseAdapter* adapter)
{
    IWeakReferenceSource::Probe(viewGroup)->GetWeakReference((IWeakReference**)&mViewGroup);
    mAdapter = adapter;
    mReleased = FALSE;
    IAdapter::Probe(mAdapter)->RegisterDataSetObserver(this);
    Refresh();
    return NOERROR;
}

void PseudoGridView::ViewGroupAdapterBridge::Link(
    /* [in] */ IViewGroup* viewGroup,
    /* [in] */ IBaseAdapter* adapter)
{
    AutoPtr<IDataSetObserver> dso;
    CViewGroupAdapterBridge::New(viewGroup, adapter, (IDataSetObserver**)&dso);
    Logger::I(TAG, " >>> Link CViewGroupAdapterBridge: %s", TO_CSTR(dso));
}

void PseudoGridView::ViewGroupAdapterBridge::Refresh()
{
    if (mReleased) {
        return;
    }

    AutoPtr<IInterface> obj;
    mViewGroup->Resolve(EIID_IInterface, (IInterface**)&obj);
    AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(obj);
    if (viewGroup == NULL) {
        ReleaseAdapter();
        return;
    }

    IAdapter* adapter = IAdapter::Probe(mAdapter);
    Int32 childCount = 0;
    viewGroup->GetChildCount(&childCount);
    Int32 adapterCount = 0;
    adapter->GetCount(&adapterCount);
    const Int32 N = Elastos::Core::Math::Max(childCount, adapterCount);
    for (Int32 i = 0; i < N; i++) {
        if (i < adapterCount) {
            AutoPtr<IView> oldView;
            if (i < childCount) {
                viewGroup->GetChildAt(i, (IView**)&oldView);
            }
            AutoPtr<IView> newView;
            adapter->GetView(i, oldView, viewGroup, (IView**)&newView);
            if (oldView == NULL) {
                // We ran out of existing views. Add it at the end.
                viewGroup->AddView(newView);
            }
            else if (oldView != newView) {
                // We couldn't rebind the view. Replace it.
                viewGroup->RemoveViewAt(i);
                viewGroup->AddView(newView, i);
            }
        }
        else {
            Int32 lastIndex = 0;
            viewGroup->GetChildCount(&lastIndex);
            lastIndex = lastIndex - 1;
            viewGroup->RemoveViewAt(lastIndex);
        }
    }
}

ECode PseudoGridView::ViewGroupAdapterBridge::OnChanged()
{
    Refresh();
    return NOERROR;
}

ECode PseudoGridView::ViewGroupAdapterBridge::OnInvalidated()
{
    ReleaseAdapter();
    return NOERROR;
}

void PseudoGridView::ViewGroupAdapterBridge::ReleaseAdapter()
{
    if (!mReleased) {
        mReleased = TRUE;
        IAdapter::Probe(mAdapter)->UnregisterDataSetObserver(this);
    }
}

//===============================================================================
// PseudoGridView
//===============================================================================
CAR_INTERFACE_IMPL(PseudoGridView, ViewGroup, IPseudoGridView)

PseudoGridView::PseudoGridView()
    : mNumColumns(3)
    , mVerticalSpacing(0)
    , mHorizontalSpacing(0)
{}

ECode PseudoGridView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::constructor(context, attrs);

    AutoPtr<ITypedArray> a;

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::PseudoGridView),
            ArraySize(R::styleable::PseudoGridView));
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    Int32 N = 0;
    a->GetIndexCount(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 attr = 0;
        a->GetIndex(i, &attr);
        switch (attr) {
            case R::styleable::PseudoGridView_numColumns:
                a->GetInt32(attr, 3, &mNumColumns);
                break;
            case R::styleable::PseudoGridView_verticalSpacing:
                a->GetDimensionPixelSize(attr, 0, &mVerticalSpacing);
                break;
            case R::styleable::PseudoGridView_horizontalSpacing:
                a->GetDimensionPixelSize(attr, 0, &mHorizontalSpacing);
                break;
        }
    }

    a->Recycle();
    return NOERROR;
}

void PseudoGridView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (MeasureSpec::GetMode(widthMeasureSpec) == MeasureSpec::UNSPECIFIED) {
        // throw new UnsupportedOperationException("Needs a maximum width");
        assert(0 && "TODO: Needs a maximum width");
    }
    Int32 width = MeasureSpec::GetSize(widthMeasureSpec);

    Int32 childWidth = (width - (mNumColumns - 1) * mHorizontalSpacing) / mNumColumns;
    Int32 childWidthSpec = MeasureSpec::MakeMeasureSpec(childWidth, MeasureSpec::EXACTLY);
    Int32 childHeightSpec = MeasureSpec::UNSPECIFIED;
    Int32 totalHeight = 0;
    Int32 children = 0, tmp = 0;
    GetChildCount(&children);
    Int32 rows = (children + mNumColumns - 1) / mNumColumns;
    for (Int32 row = 0; row < rows; row++) {
        Int32 startOfRow = row * mNumColumns;
        Int32 endOfRow = Elastos::Core::Math::Min(startOfRow + mNumColumns, children);
        Int32 maxHeight = 0;
        for (Int32 i = startOfRow; i < endOfRow; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            child->Measure(childWidthSpec, childHeightSpec);
            child->GetMeasuredHeight(&tmp);
            maxHeight = Elastos::Core::Math::Max(maxHeight, tmp);
        }
        Int32 maxHeightSpec = MeasureSpec::MakeMeasureSpec(maxHeight, MeasureSpec::EXACTLY);
        for (Int32 i = startOfRow; i < endOfRow; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            if ((child->GetMeasuredHeight(&tmp), tmp) != maxHeight) {
                child->Measure(childWidthSpec, maxHeightSpec);
            }
        }
        totalHeight += maxHeight;
        if (row > 0) {
            totalHeight += mVerticalSpacing;
        }
    }

    GetDefaultSize(totalHeight, heightMeasureSpec);
    SetMeasuredDimension(width, tmp);
}

ECode PseudoGridView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Boolean isRtl = FALSE;
    IsLayoutRtl(&isRtl);
    Int32 children = 0;
    GetChildCount(&children);
    Int32 rows = (children + mNumColumns - 1) / mNumColumns;
    Int32 y = 0, tmp = 0;;
    for (Int32 row = 0; row < rows; row++) {
        Int32 x = isRtl ? (GetWidth(&tmp), tmp) : 0;
        Int32 maxHeight = 0;
        Int32 startOfRow = row * mNumColumns;
        Int32 endOfRow = Elastos::Core::Math::Min(startOfRow + mNumColumns, children);
        for (Int32 i = startOfRow; i < endOfRow; i++) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            Int32 width = 0;
            child->GetMeasuredWidth(&width);
            Int32 height = 0;
            child->GetMeasuredHeight(&height);
            if (isRtl) {
                x -= width;
            }
            child->Layout(x, y, x + width, y + height);
            maxHeight = Elastos::Core::Math::Max(maxHeight, height);
            if (isRtl) {
                x -= mHorizontalSpacing;
            }
            else {
                x += width + mHorizontalSpacing;
            }
        }
        y += maxHeight;
        if (row > 0) {
            y += mVerticalSpacing;
        }
    }
    return NOERROR;
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
