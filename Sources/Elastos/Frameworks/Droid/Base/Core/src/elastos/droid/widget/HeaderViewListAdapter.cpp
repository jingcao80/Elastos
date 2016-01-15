#include "elastos/droid/widget/HeaderViewListAdapter.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Widget {

AutoPtr<IArrayList> HeaderViewListAdapter::EMPTY_INFO_LIST;
Boolean HeaderViewListAdapter::sInit = InitStatic();

CAR_INTERFACE_IMPL_5(HeaderViewListAdapter, Object, IHeaderViewListAdapter, IWrapperListAdapter, IListAdapter, IAdapter, IFilterable);
HeaderViewListAdapter::HeaderViewListAdapter()
    : mAreAllFixedViewsSelectable(FALSE)
    , mIsFilterable(FALSE)
{
}

ECode HeaderViewListAdapter::constructor(
    /* [in] */ IArrayList* headerViewInfos,
    /* [in] */ IArrayList* footerViewInfos,
    /* [in] */ IListAdapter* adapter)
{
    mAdapter = adapter;
    if(IFilterable::Probe(adapter)){
        mIsFilterable = TRUE;
    }
    else {
        mIsFilterable = FALSE;
    }

    if (headerViewInfos == NULL) {
        mHeaderViewInfos = EMPTY_INFO_LIST;
    }
    else {
        mHeaderViewInfos = headerViewInfos;
    }

    if (footerViewInfos == NULL) {
        mFooterViewInfos = EMPTY_INFO_LIST;
    }
    else {
        mFooterViewInfos = footerViewInfos;
    }

    mAreAllFixedViewsSelectable = AreAllListInfosSelectable(mHeaderViewInfos)
        && AreAllListInfosSelectable(mFooterViewInfos);
    return NOERROR;
}

ECode HeaderViewListAdapter::GetHeadersCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mHeaderViewInfos->GetSize(count);
}

ECode HeaderViewListAdapter::GetFootersCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    return mFooterViewInfos->GetSize(count);
}

ECode HeaderViewListAdapter::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);
    Boolean res = FALSE;
    *empty =  mAdapter == NULL || (IAdapter::Probe(mAdapter)->IsEmpty(&res), res);
    return NOERROR;
}

ECode HeaderViewListAdapter::RemoveHeader(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size = 0;
    mHeaderViewInfos->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mHeaderViewInfos->Get(i, (IInterface**)&obj);
        AutoPtr<IFixedViewInfo> info = IFixedViewInfo::Probe(obj);
        AutoPtr<IView> view;
        info->GetView((IView**)&view);
        if (view.Get() == v) {
            mHeaderViewInfos->Remove(i);

            mAreAllFixedViewsSelectable =
                    AreAllListInfosSelectable(mHeaderViewInfos)
                    && AreAllListInfosSelectable(mFooterViewInfos);

            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode HeaderViewListAdapter::RemoveFooter(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size = 0;
    mFooterViewInfos->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mFooterViewInfos->Get(i, (IInterface**)&obj);
        AutoPtr<IFixedViewInfo> info = IFixedViewInfo::Probe(obj);
        AutoPtr<IView> view;
        info->GetView((IView**)&view);
        if (view.Get() == v) {
            mFooterViewInfos->Remove(i);

            mAreAllFixedViewsSelectable =
                    AreAllListInfosSelectable(mHeaderViewInfos)
                    && AreAllListInfosSelectable(mFooterViewInfos);

            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode HeaderViewListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    Int32 fc = 0, hc = 0;
    GetFootersCount(&fc);
    GetHeadersCount(&hc);
    if(mAdapter != NULL) {
        Int32 c = 0;
        IAdapter::Probe(mAdapter)->GetCount(&c);
        *count = fc + hc + c;
        return NOERROR;
    }

    *count = fc + hc;
    return NOERROR;
}

ECode HeaderViewListAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    if(mAdapter != NULL) {
        Boolean res = FALSE;
        mAdapter->AreAllItemsEnabled(&res);
        *enabled = mAreAllFixedViewsSelectable && res;
        return NOERROR;
    }
    *enabled = TRUE;
    return NOERROR;
}

ECode HeaderViewListAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    // Header (negative positions will throw an IndexOutOfBoundsException)
    Int32 numHeaders = 0;
    GetHeadersCount(&numHeaders);
    if (position < numHeaders) {
        AutoPtr<IInterface> obj;
        mHeaderViewInfos->Get(position, (IInterface**)&obj);
        AutoPtr<IFixedViewInfo> info = IFixedViewInfo::Probe(obj);
        return info->GetSelectable(enabled);
    }

    // Adapter
    const Int32 adjPosition = position - numHeaders;
    Int32 adapterCount = 0;
    if (mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->GetCount(&adapterCount);
        if (adjPosition < adapterCount) {
            return mAdapter->IsEnabled(adjPosition, enabled);
        }
    }

    // Footer (off-limits positions will throw an IndexOutOfBoundsException)
    AutoPtr<IInterface> obj;
    mFooterViewInfos->Get(adjPosition - adapterCount, (IInterface**)&obj);
    return IFixedViewInfo::Probe(obj)->GetSelectable(enabled);
}

ECode HeaderViewListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    // Header (negative positions will throw an IndexOutOfBoundsException)
    Int32 numHeaders = 0;
    GetHeadersCount(&numHeaders);
    if(position < numHeaders) {
        AutoPtr<IInterface> obj;
        mHeaderViewInfos->Get(position, (IInterface**)&obj);
        return IFixedViewInfo::Probe(obj)->GetData(item);
    }

    // Adapter
    Int32 adjPosition = position - numHeaders;
    Int32 adapterCount = 0;
    if(mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->GetCount(&adapterCount);
        if(adjPosition < adapterCount) {
            return IAdapter::Probe(mAdapter)->GetItem(adjPosition, item);
        }
    }
    Int32 c = 0;
    if (adjPosition - adapterCount >= (GetFootersCount(&c), c)) {
        *item = NULL;
        return NOERROR;
    }

    // Footer (off-limits positions will throw an IndexOutOfBoundsException)
    AutoPtr<IInterface> obj;
    mFooterViewInfos->Get(adjPosition - adapterCount, (IInterface**)&obj);
    return IFixedViewInfo::Probe(obj)->GetData(item);
}

ECode HeaderViewListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    Int32 numHeaders = 0;
    GetHeadersCount(&numHeaders);
    if(mAdapter != NULL && position >= numHeaders) {
        Int32 adjPosition = position - numHeaders;
        Int32 adapterCount = 0;
        IAdapter::Probe(mAdapter)->GetCount(&adapterCount);
        if(adjPosition < adapterCount) {
            return IAdapter::Probe(mAdapter)->GetItemId(adjPosition, id);
        }
    }
    *id = -1;
    return NOERROR;
}

ECode HeaderViewListAdapter::HasStableIds(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    if(mAdapter != NULL) {
        return IAdapter::Probe(mAdapter)->HasStableIds(has);
    }
    *has = FALSE;
    return NOERROR;
}

ECode HeaderViewListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    // Header (negative positions will throw an IndexOutOfBoundsException)
    Int32 numHeaders = 0;
    GetHeadersCount(&numHeaders);
    if(position < numHeaders) {
        AutoPtr<IInterface> obj;
        mHeaderViewInfos->Get(position, (IInterface**)&obj);
        return IFixedViewInfo::Probe(obj)->GetView(view);
    }

    // Adapter
    Int32 adjPosition = position - numHeaders;
    Int32 adapterCount = 0;
    if(mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->GetCount(&adapterCount);
        if(adjPosition < adapterCount) {
            return IAdapter::Probe(mAdapter)->GetView(adjPosition, convertView, parent, view);
        }
    }

    Int32 count = 0;
    if (adjPosition - adapterCount >= (GetFootersCount(&count), count)) {
        *view = NULL;
        return NOERROR;
    }

    // Footer (off-limits positions will throw an IndexOutOfBoundsException)
    AutoPtr<IInterface> obj;
    mFooterViewInfos->Get(adjPosition - adapterCount, (IInterface**)&obj);
    return IFixedViewInfo::Probe(obj)->GetView(view);
}

ECode HeaderViewListAdapter::GetItemViewType(
    /* [in] */Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    Int32 numHeaders = 0;
    GetHeadersCount(&numHeaders);
    if(mAdapter != NULL && position >= numHeaders) {
        Int32 adjPosition = position - numHeaders;
        Int32 adapterCount = 0;
        IAdapter::Probe(mAdapter)->GetCount(&adapterCount);
        if(adjPosition < adapterCount) {
            Int32 res = 0;
            IAdapter::Probe(mAdapter)->GetItemViewType(adjPosition, &res);
        }
    }
    *type = IAdapterView::ITEM_VIEW_TYPE_HEADER_OR_FOOTER;
    return NOERROR;
}

ECode HeaderViewListAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    if(mAdapter != NULL) {
        return IAdapter::Probe(mAdapter)->GetViewTypeCount(count);
    }
    *count = 1;
    return NOERROR;
}

ECode HeaderViewListAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    if(mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->RegisterDataSetObserver(observer);
    }
    return NOERROR;
}

ECode HeaderViewListAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    if(mAdapter != NULL) {
        IAdapter::Probe(mAdapter)->UnregisterDataSetObserver(observer);
    }
    return NOERROR;
}

ECode HeaderViewListAdapter::GetFilter(
    /* [out] */ IFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    if(mIsFilterable) {
        AutoPtr<IFilterable> filterable = IFilterable::Probe(mAdapter);
        return filterable->GetFilter(filter);
    }
    *filter = NULL;
    return NOERROR;
}

ECode HeaderViewListAdapter::GetWrappedAdapter(
    /* [out] */ IListAdapter** atapter)
{
    VALIDATE_NOT_NULL(atapter);
    *atapter = mAdapter;
    REFCOUNT_ADD(*atapter);
    return NOERROR;
}

Boolean HeaderViewListAdapter::AreAllListInfosSelectable(
    /* [in] */ IArrayList* infos)
{
    if (infos != NULL) {
        Int32 size = 0;
        infos->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            infos->Get(i, (IInterface**)&obj);
            AutoPtr<IFixedViewInfo> info = IFixedViewInfo::Probe(obj);
            Boolean isSelectable = FALSE;
            info->GetSelectable(&isSelectable);
            if (!isSelectable) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

Boolean HeaderViewListAdapter::InitStatic()
{
    CArrayList::New((IArrayList**)&EMPTY_INFO_LIST);
    return TRUE;
}

}// amespace Widget
}// namespace Droid
}// namespace Elastos
