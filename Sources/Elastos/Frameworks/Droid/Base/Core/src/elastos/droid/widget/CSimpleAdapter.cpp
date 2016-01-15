
#include "elastos/droid/widget/CSimpleAdapter.h"

namespace Elastos {x
namespace Droid {
namespace Widget {


ECode CSimpleAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IObjectContainer* data,
    /* [in] */ Int32 resource,
    /* [in] */ ArrayOf<String>* from,
    /* [in] */ ArrayOf<Int32>* to)
{
    return SimpleAdapter::Init(context, data, resource, from, to);
}

ECode CSimpleAdapter::SetDropDownViewResource(
    /* [in] */ Int32 resource)
{
    return SimpleAdapter::SetDropDownViewResource(resource);
}

ECode CSimpleAdapter::GetViewBinder(
    /* [out] */ ISimpleAdapterViewBinder** viewBinder)
{
    VALIDATE_NOT_NULL(viewBinder);
    AutoPtr<ISimpleAdapterViewBinder> temp = SimpleAdapter::GetViewBinder();
    *viewBinder = temp;
    REFCOUNT_ADD(*viewBinder);
    return NOERROR;
}

ECode CSimpleAdapter::SetViewBinder(
    /* [in] */ ISimpleAdapterViewBinder* viewBinder)
{
    return SimpleAdapter::SetViewBinder(viewBinder);
}

ECode CSimpleAdapter::SetViewImage(
    /* [in] */ IImageView* v,
    /* [in] */ Int32 value)
{
    return SimpleAdapter::SetViewImage(v, value);
}

ECode CSimpleAdapter::SetViewImage(
    /* [in] */ IImageView* v,
    /* [in] */ const String& value)
{
    return SimpleAdapter::SetViewImage(v, value);
}

ECode CSimpleAdapter::SetViewText(
    /* [in] */ ITextView* v,
    /* [in] */ const String& text)
{
    return SimpleAdapter::SetViewText(v, text);
}

ECode CSimpleAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return SimpleAdapter::RegisterDataSetObserver(observer);
}

ECode CSimpleAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return SimpleAdapter::UnregisterDataSetObserver(observer);
}

ECode CSimpleAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = SimpleAdapter::GetCount();
    return NOERROR;
}

ECode CSimpleAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item);
    AutoPtr<IInterface> temp = SimpleAdapter::GetItem(position);
    *item = temp;
    REFCOUNT_ADD(*item);
    return NOERROR;
}

ECode CSimpleAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId);
    *itemId = SimpleAdapter::GetItemId(position);
    return NOERROR;
}

ECode CSimpleAdapter::HasStableIds(
    /* [out] */ Boolean* hasStableIds)
{
    VALIDATE_NOT_NULL(hasStableIds);
    *hasStableIds = SimpleAdapter::HasStableIds();
    return NOERROR;
}

ECode CSimpleAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = SimpleAdapter::GetView(position, convertView, parent);
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CSimpleAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* viewType)
{
    VALIDATE_NOT_NULL(viewType);
    *viewType = SimpleAdapter::GetItemViewType(position);
    return NOERROR;
}

ECode CSimpleAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = SimpleAdapter::GetViewTypeCount();
    return NOERROR;
}

ECode CSimpleAdapter::IsEmpty(
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty);
    *isEmpty = SimpleAdapter::IsEmpty();
    return NOERROR;
}

ECode CSimpleAdapter::NotifyDataSetChanged()
{
    return SimpleAdapter::NotifyDataSetChanged();
}

ECode CSimpleAdapter::NotifyDataSetInvalidated()
{
    return SimpleAdapter::NotifyDataSetInvalidated();
}

ECode CSimpleAdapter::GetDropDownView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> temp = SimpleAdapter::GetDropDownView(position, convertView, parent);
    *view = temp;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CSimpleAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enable)
{
    VALIDATE_NOT_NULL(enable);
    *enable = SimpleAdapter::AreAllItemsEnabled();
    return NOERROR;
}

ECode CSimpleAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [in] */ Boolean* enable)
{
    VALIDATE_NOT_NULL(enable);
    *enable = SimpleAdapter::IsEnabled(position);
    return NOERROR;
}

ECode CSimpleAdapter::GetFilter(
    /* [out] */ IFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    AutoPtr<IFilter> temp;
    temp = SimpleAdapter::GetFilter();
    *filter = temp;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
