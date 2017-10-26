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

#include "elastos/droid/contacts/common/list/PinnedHeaderListAdapter.h"
#include "elastos/droid/contacts/common/list/CPinnedHeaderListView.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Contacts::Common::List::EIID_IPinnedHeaderListAdapter;
using Elastos::Droid::Contacts::Common::List::EIID_IPinnedHeaderAdapter;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

CAR_INTERFACE_IMPL_2(PinnedHeaderListAdapter, CompositeCursorAdapter, IPinnedHeaderListAdapter, IPinnedHeaderAdapter)

ECode PinnedHeaderListAdapter::constructor(
    /* [in] */ IContext* context)
{
    return CompositeCursorAdapter::constructor(context);
}

ECode PinnedHeaderListAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 initialCapacity)
{
    return CompositeCursorAdapter::constructor(context, initialCapacity);
}

ECode PinnedHeaderListAdapter::GetPinnedPartitionHeadersEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPinnedPartitionHeadersEnabled;
    return NOERROR;
}

ECode PinnedHeaderListAdapter::SetPinnedPartitionHeadersEnabled(
    /* [in] */ Boolean flag)
{
    mPinnedPartitionHeadersEnabled = flag;
    return NOERROR;
}

ECode PinnedHeaderListAdapter::GetPinnedHeaderCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (mPinnedPartitionHeadersEnabled) {
        return GetPartitionCount(count);
    }
    else {
        *count = 0;
    }
    return NOERROR;
}

Boolean PinnedHeaderListAdapter::IsPinnedPartitionHeaderVisible(
    /* [in] */ Int32 partition)
{
    Boolean enabled, hasHeader, isEmpty;
    return (GetPinnedPartitionHeadersEnabled(&enabled), enabled) &&
            (HasHeader(partition, &hasHeader), hasHeader) && (IsPartitionEmpty(partition, &isEmpty), !isEmpty);
}

ECode PinnedHeaderListAdapter::GetPinnedHeaderView(
    /* [in] */ Int32 partition,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** _view)
{
    VALIDATE_NOT_NULL(_view)
    Boolean hasHeader;
    if (HasHeader(partition, &hasHeader), hasHeader) {
        AutoPtr<IView> view;
        if (convertView != NULL) {
            AutoPtr<IInterface> tag;
            convertView->GetTag((IInterface**)&tag);
            AutoPtr<IInteger32> headerType = IInteger32::Probe(tag);
            if (headerType != NULL && CoreUtils::Unbox(headerType) == PARTITION_HEADER_TYPE) {
                view = convertView;
            }
        }
        if (view == NULL) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            view = NewHeaderView(ctx, partition, NULL, parent);

            view->SetTag(CoreUtils::Convert(PARTITION_HEADER_TYPE));
            view->SetFocusable(FALSE);
            view->SetEnabled(FALSE);
        }
        AutoPtr<ICursor> cursor;
        GetCursor(partition, (ICursor**)&cursor);
        BindHeaderView(view, partition, cursor);
        Int32 direction;
        IView::Probe(parent)->GetLayoutDirection(&direction);
        view->SetLayoutDirection(direction);
        *_view = view;
        REFCOUNT_ADD(*_view)
    }
    else {
        *_view = NULL;
    }
    return NOERROR;
}

ECode PinnedHeaderListAdapter::ConfigurePinnedHeaders(
    /* [in] */ IPinnedHeaderListView* _listView)
{
    AutoPtr<CPinnedHeaderListView> listView = (CPinnedHeaderListView*)_listView;
    Boolean enabled;
    if (GetPinnedPartitionHeadersEnabled(&enabled), !enabled) {
        return NOERROR;
    }

    Int32 size;
    GetPartitionCount(&size);

    // Cache visibility bits, because we will need them several times later on
    if (mHeaderVisibility == NULL || mHeaderVisibility->GetLength() != size) {
        mHeaderVisibility = ArrayOf<Boolean>::Alloc(size);
    }
    for (Int32 i = 0; i < size; i++) {
        Boolean visible = IsPinnedPartitionHeaderVisible(i);
        (*mHeaderVisibility)[i] = visible;
        if (!visible) {
            listView->SetHeaderInvisible(i, TRUE);
        }
    }

    Int32 headerViewsCount = listView->GetHeaderViewsCount();

    // Starting at the top, find and pin headers for partitions preceding the visible one(s)
    Int32 maxTopHeader = -1;
    Int32 topHeaderHeight = 0;
    for (Int32 i = 0; i < size; i++) {
        if ((*mHeaderVisibility)[i]) {
            Int32 pos;
            listView->GetPositionAt(topHeaderHeight, &pos);
            Int32 position = pos - headerViewsCount;
            Int32 partition;
            GetPartitionForPosition(position, &partition);
            if (i > partition) {
                break;
            }

            listView->SetHeaderPinnedAtTop(i, topHeaderHeight, FALSE);
            Int32 height;
            listView->GetPinnedHeaderHeight(i, &height);
            topHeaderHeight += height;
            maxTopHeader = i;
        }
    }

    // Starting at the bottom, find and pin headers for partitions following the visible one(s)
    Int32 maxBottomHeader = size;
    Int32 bottomHeaderHeight = 0;
    Int32 listHeight;
    IView::Probe(listView)->GetHeight(&listHeight);
    for (Int32 i = size; --i > maxTopHeader;) {
        if ((*mHeaderVisibility)[i]) {
            Int32 pos;
            listView->GetPositionAt(listHeight - bottomHeaderHeight, &pos);
            Int32 position = pos - headerViewsCount;
            if (position < 0) {
                break;
            }

            Int32 partition;
            GetPartitionForPosition(position - 1, &partition);
            if (partition == -1 || i <= partition) {
                break;
            }

            Int32 height;
            listView->GetPinnedHeaderHeight(i, &height);
            bottomHeaderHeight += height;

            listView->SetHeaderPinnedAtBottom(i, listHeight - bottomHeaderHeight, FALSE);
            maxBottomHeader = i;
        }
    }

    // Headers in between the top-pinned and bottom-pinned should be hidden
    for (Int32 i = maxTopHeader + 1; i < maxBottomHeader; i++) {
        if ((*mHeaderVisibility)[i]) {
            Boolean isEmpty;
            IsPartitionEmpty(i, &isEmpty);
            listView->SetHeaderInvisible(i, isEmpty);
        }
    }
    return NOERROR;
}

ECode PinnedHeaderListAdapter::GetScrollPositionForHeader(
    /* [in] */ Int32 viewIndex,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position)
    return GetPositionForPartition(viewIndex, position);
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
