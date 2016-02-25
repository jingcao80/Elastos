#ifndef __ELASTOS_DROID_WIDGET_HEADERVIEWLISTADAPTER_H__
#define __ELASTOS_DROID_WIDGET_HEADERVIEWLISTADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/ListView.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Database::IDataSetObserver;

namespace Elastos {
namespace Droid {
namespace Widget {

class HeaderViewListAdapter
    : public Object
    , public IHeaderViewListAdapter
    , public IWrapperListAdapter
    , public IListAdapter
    , public IAdapter
    , public IFilterable
{
public:
    CAR_INTERFACE_DECL();

    HeaderViewListAdapter();

    CARAPI constructor(
        /* [in] */ IArrayList* headerViewInfos,
        /* [in] */ IArrayList* footerViewInfos,
        /* [in] */ IListAdapter* adapter);

    virtual CARAPI GetHeadersCount(
        /* [out] */ Int32* count);

    virtual CARAPI GetFootersCount(
        /* [out] */ Int32* count);

    virtual CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

    CARAPI RemoveHeader(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI RemoveFooter(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* enabled);

    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    CARAPI HasStableIds(
        /* [out] */ Boolean* has);

    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    CARAPI GetFilter(
        /* [out] */ IFilter** filter);

    CARAPI GetWrappedAdapter(
        /* [out] */ IListAdapter** atapter);

private:
    CARAPI_(Boolean) AreAllListInfosSelectable(
        /* [in] */ IArrayList* info);

    static CARAPI_(Boolean) InitStatic();

private:
    AutoPtr<IListAdapter> mAdapter;
    AutoPtr<IArrayList> mHeaderViewInfos;
    AutoPtr<IArrayList> mFooterViewInfos;

    // Used as a placeholder in case the provided info views are indeed null.
    // Currently only used by some CTS tests, which may be removed.
    static Boolean sInit;
    static AutoPtr<IArrayList> EMPTY_INFO_LIST;

    Boolean mAreAllFixedViewsSelectable;
    Boolean mIsFilterable;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_HEADERVIEWLISTADAPTER_H__
