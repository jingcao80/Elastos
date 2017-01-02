//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    CAR_INTERFACE_DECL()

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

private:
    AutoPtr<IListAdapter> mAdapter;
    AutoPtr<IArrayList> mHeaderViewInfos;
    AutoPtr<IArrayList> mFooterViewInfos;

    // Used as a placeholder in case the provided info views are indeed null.
    // Currently only used by some CTS tests, which may be removed.
    static AutoPtr<IArrayList> EMPTY_INFO_LIST;

    Boolean mAreAllFixedViewsSelectable;
    Boolean mIsFilterable;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_HEADERVIEWLISTADAPTER_H__
