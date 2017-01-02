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

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/BaseExpandableListAdapter.h"
#include "elastos/droid/database/CDataSetObservable.h"

using Elastos::Droid::Database::CDataSetObservable;
using Elastos::Droid::Database::IObservable;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(BaseExpandableListAdapter, Object, IBaseExpandableListAdapter);
BaseExpandableListAdapter::BaseExpandableListAdapter()
{
    CDataSetObservable::New((IDataSetObservable**)&mDataSetObservable);
}

ECode BaseExpandableListAdapter::RegisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    IObservable::Probe(mDataSetObservable)->RegisterObserver(observer);
    return NOERROR;
}

ECode BaseExpandableListAdapter::UnregisterDataSetObserver(
    /* [in] */ IDataSetObserver* observer)
{
    return IObservable::Probe(mDataSetObservable)->UnregisterObserver(observer);
}

ECode BaseExpandableListAdapter::NotifyDataSetInvalidated()
{
    return mDataSetObservable->NotifyInvalidated();
}

ECode BaseExpandableListAdapter::NotifyDataSetChanged()
{
    return mDataSetObservable->NotifyChanged();
}

ECode BaseExpandableListAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    *enabled = TRUE;
    return NOERROR;
}

ECode BaseExpandableListAdapter::OnGroupCollapsed(
    /* [in] */ Int32 groupPosition)
{
    return NOERROR;
}

ECode BaseExpandableListAdapter::OnGroupExpanded(
    /* [in] */ Int32 groupPosition)
{
    return NOERROR;
}

ECode BaseExpandableListAdapter::GetCombinedChildId(
    /* [in] */ Int64 groupId,
    /* [in] */ Int64 childId,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = 0x8000000000000000L | ((groupId & 0x7FFFFFFF) << 32) | (childId & 0xFFFFFFFF);
    return NOERROR;
}

ECode BaseExpandableListAdapter::GetCombinedGroupId(
    /* [in] */ Int64 groupId,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = (groupId & 0x7FFFFFFF) << 32;
    return NOERROR;
}

ECode BaseExpandableListAdapter::IsEmpty(
    /* [out] */ Boolean* empty)
{
    VALIDATE_NOT_NULL(empty);
    Int32 count = 0;
    *empty = (GetGroupCount(&count), count) == 0;
    return NOERROR;
}

ECode BaseExpandableListAdapter::GetChildType(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = 0;
    return NOERROR;
}

ECode BaseExpandableListAdapter::GetChildTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 1;
    return NOERROR;
}

ECode BaseExpandableListAdapter::GetGroupType(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = 0;
    return NOERROR;
}

ECode BaseExpandableListAdapter::GetGroupTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 1;
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace Widget
