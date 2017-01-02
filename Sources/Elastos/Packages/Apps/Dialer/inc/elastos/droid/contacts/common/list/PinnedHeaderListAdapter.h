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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/common/widget/CompositeCursorAdapter.h"

using Elastos::Droid::Common::Widget::CompositeCursorAdapter;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A subclass of {@link CompositeCursorAdapter} that manages pinned partition headers.
 */
class PinnedHeaderListAdapter
    : public CompositeCursorAdapter
    , public IPinnedHeaderListAdapter
    , public IPinnedHeaderAdapter
{
public:
    PinnedHeaderListAdapter()
        : mPinnedPartitionHeadersEnabled(FALSE)
    {}

    CAR_INTERFACE_DECL()

    virtual ~PinnedHeaderListAdapter() {}

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 initialCapacity);

    CARAPI GetPinnedPartitionHeadersEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetPinnedPartitionHeadersEnabled(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI GetPinnedHeaderCount(
        /* [out] */ Int32* count);

    /**
     * The default implementation creates the same type of view as a normal
     * partition header.
     */
    // @Override
    CARAPI GetPinnedHeaderView(
        /* [in] */ Int32 viewIndex,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    // @Override
    CARAPI ConfigurePinnedHeaders(
        /* [in] */ IPinnedHeaderListView* listView);

    // @Override
    CARAPI GetScrollPositionForHeader(
        /* [in] */ Int32 viewIndex,
        /* [out] */ Int32* position);

protected:
    CARAPI_(Boolean) IsPinnedPartitionHeaderVisible(
        /* [in] */ Int32 partition);

private:
    Boolean mPinnedPartitionHeadersEnabled;
    AutoPtr<ArrayOf<Boolean> > mHeaderVisibility;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__
