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

#ifndef __ELASTOS_DROID_WIDGET_BASEADAPTER_H__
#define __ELASTOS_DROID_WIDGET_BASEADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Database::IDataSetObservable;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * Common base class of common implementation for an {@link Adapter} that can be
 * used in both {@link ListView} (by implementing the specialized
 * {@link ListAdapter} interface} and {@link Spinner} (by implementing the
 * specialized {@link SpinnerAdapter} interface.
 */
class ECO_PUBLIC BaseAdapter
    : public Object
    , public IBaseAdapter
    , public IListAdapter
    , public ISpinnerAdapter
    , public IAdapter
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI HasStableIds(
        /* [out] */ Boolean* has);

    virtual CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    virtual CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    virtual CARAPI NotifyDataSetChanged();

    virtual CARAPI NotifyDataSetInvalidated();

    virtual CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* enabled);

    virtual CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* enabled);

    virtual CARAPI GetDropDownView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    virtual CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    virtual CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

protected:
    BaseAdapter();

private:
    AutoPtr<IDataSetObservable> mDataSetObservable;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_BASEADAPTER_H__
