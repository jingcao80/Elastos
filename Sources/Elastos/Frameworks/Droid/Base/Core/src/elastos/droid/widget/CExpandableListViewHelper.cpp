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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CExpandableListViewHelper.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(CExpandableListViewHelper, Singleton, IExpandableListViewHelper);

CAR_SINGLETON_IMPL(CExpandableListViewHelper);

ECode CExpandableListViewHelper::GetPackedPositionType(
    /* [in] */ Int64 packedPosition,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = ExpandableListView::GetPackedPositionType(packedPosition);
    return NOERROR;
}

ECode CExpandableListViewHelper::GetPackedPositionGroup(
    /* [in] */ Int64 packedPosition,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListView::GetPackedPositionGroup(packedPosition);
    return NOERROR;
}

ECode CExpandableListViewHelper::GetPackedPositionChild(
    /* [in] */ Int64 packedPosition,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListView::GetPackedPositionChild(packedPosition);
    return NOERROR;
}

ECode CExpandableListViewHelper::GetPackedPositionForChild(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Int64* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListView::GetPackedPositionForChild(groupPosition, childPosition);
    return NOERROR;
}

ECode CExpandableListViewHelper::GetPackedPositionForGroup(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int64* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListView::GetPackedPositionForGroup(groupPosition);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
