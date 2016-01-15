
#include "elastos/droid/widget/RemoteViewsListAdapter.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(RemoteViewsListAdapter, BaseAdapter, IRemoteViewsListAdapter)

RemoteViewsListAdapter::RemoteViewsListAdapter()
    : mViewTypeCount(0)
{
    CArrayList::New((IArrayList**)&mViewTypes);
}

ECode RemoteViewsListAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IArrayList* remoteViews,
    /* [in] */ Int32 viewTypeCount)
{
    mContext = context;
    mRemoteViewsList = remoteViews;
    mViewTypeCount = viewTypeCount;
    Init();
    return NOERROR;
}

ECode RemoteViewsListAdapter::SetViewsList(
    /* [in] */ IArrayList* remoteViews)
{
    mRemoteViewsList = remoteViews;
    Init();
    NotifyDataSetChanged();
    return NOERROR;
}

ECode RemoteViewsListAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    *count = mViewTypeCount;
    return NOERROR;
}

ECode RemoteViewsListAdapter::GetCount(
    /* [out] */ Int32* count)
{
    if (mRemoteViewsList != NULL) {
        return mRemoteViewsList->GetSize(count);
    } else {
        *count = 0;
        return NOERROR;
    }
}

ECode RemoteViewsListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    *item = NULL;
    return NOERROR;
}

ECode RemoteViewsListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    *itemId = position;
    return NOERROR;
}

ECode RemoteViewsListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    Int32 listCount;
    GetCount(&listCount);
    if (position < listCount) {
        AutoPtr<IInterface> tmp;
        mRemoteViewsList->Get(position, (IInterface**)&tmp);
        AutoPtr<IRemoteViews> rv = IRemoteViews::Probe(tmp);
        rv->SetIsWidgetCollectionChild(TRUE);
        AutoPtr<IView> v;
        if (convertView != NULL && rv != NULL)
        {
            Int32 id, layoutId;
            convertView->GetId(&id);
            rv->GetLayoutId(&layoutId);
            if (id == layoutId) {
                *view = convertView;
                REFCOUNT_ADD(*view)
                return rv->Reapply(mContext, convertView);
            }
        }
        return rv->Apply(mContext, parent, view);
    } else {
        *view = NULL;
        return NOERROR;
    }
}

ECode RemoteViewsListAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    Int32 listCount;
    GetCount(&listCount);
    if (position < listCount) {
        AutoPtr<IInterface> tmp;
        mRemoteViewsList->Get(position, (IInterface**)&tmp);
        AutoPtr<IRemoteViews> rv = IRemoteViews::Probe(tmp);
        Int32 layoutId;
        rv->GetLayoutId(&layoutId);

        List<Int32>::Iterator it = mViewTypes.Begin();
        Int32 i = 0;
        for(; it != mViewTypes.End(); it++, i++) {
            if (*it == layoutId)
                break;
        }
        *type = i;
    } else {
        *type = 0;
    }
    return NOERROR;
}

ECode RemoteViewsListAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    *hasStableIds = FALSE;
    return NOERROR;
}

ECode RemoteViewsListAdapter::Init()
{
    if (mRemoteViewsList == NULL) {
        return NOERROR;
    }

    mViewTypes.Clear();
    Int32 size;
    mRemoteViewsList->GetSize(&size);
    for (Int32 i = 0; i != size; i++) {
        AutoPtr<IInterface> tmp;
        mRemoteViewsList->Get(i, (IInterface**)&tmp);
        AutoPtr<IRemoteViews> rv = IRemoteViews::Probe(tmp);
        Int32 layoutId;
        rv->GetLayoutId(&layoutId);
        if (!FindInList(layoutId)) {
            mViewTypes.PushBack(layoutId);
        }
    }

    if (mViewTypes.GetSize() > mViewTypeCount || mViewTypeCount < 1) {
        SLOGGERE("RemoteViewsListAdapter", String("Invalid view type count -- view type count must be >= 1") + 
            "and must be as large as the total number of distinct view types")
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}
Boolean RemoteViewsListAdapter::FindInList(
    /* [in] */ Int32 value)
{
    List<Int32>::Iterator it = mViewTypes.Begin();
    for(; it != mViewTypes.End(); it++) {
        if (*it == value)
            return TRUE;
    }
    return FALSE;
}

} // namespace Elastos
} // namespace Droid
} // namespace Widget

