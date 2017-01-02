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

#ifndef __ELASTOS_DROID_WIDGET_REMOTEVIEWSLISTADAPTER_H__
#define __ELASTOS_DROID_WIDGET_REMOTEVIEWSLISTADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/view/ViewGroup.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Widget {

class RemoteViewsListAdapter
    : public BaseAdapter
    , public IRemoteViewsListAdapter
{
public:
    CAR_INTERFACE_DECL()

    RemoteViewsListAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IArrayList* remoteViews,
        /* [in] */ Int32 viewTypeCount);

    CARAPI SetViewsList(
        /* [in] */ IArrayList* remoteViews);

    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* itemId);

    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* count);

    CARAPI HasStableIds(
        /* [out] */ Boolean* hasStableIds);

private:
    CARAPI Init();

    CARAPI_(Boolean) FindInList(
        /* [in] */ Int32 value);

    AutoPtr<IContext> mContext;
    AutoPtr<IArrayList> mRemoteViewsList;
    List<Int32> mViewTypes;
    Int32 mViewTypeCount;
};

} // namespace Elastos
} // namespace Droid
} // namespace Widget
#endif