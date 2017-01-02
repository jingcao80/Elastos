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

#include "elastos/droid/widget/CExpandableListPositionHelper.h"
#include "elastos/droid/widget/ExpandableListPosition.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_SINGLETON_IMPL(CExpandableListPositionHelper)

CAR_INTERFACE_IMPL(CExpandableListPositionHelper, Singleton, IExpandableListPositionHelper)

ECode CExpandableListPositionHelper::ObtainGroupPosition(
    /* [in] */ Int32 groupPosition,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    AutoPtr<IExpandableListPosition> temp = ExpandableListPosition::ObtainGroupPosition(groupPosition);
    *position = temp;
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode CExpandableListPositionHelper::ObtainChildPosition(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    AutoPtr<IExpandableListPosition> temp = ExpandableListPosition::ObtainChildPosition(groupPosition, childPosition);
    *position = temp;
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode CExpandableListPositionHelper::ObtainPosition(
    /* [in] */ Int64 packedPosition,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    AutoPtr<IExpandableListPosition> temp = ExpandableListPosition::ObtainPosition(packedPosition);
    *position = temp;
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode CExpandableListPositionHelper::Obtain(
    /* [in] */ Int32 type,
    /* [in] */ Int32 groupPos,
    /* [in] */ Int32 childPos,
    /* [in] */ Int32 flatListPos,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    AutoPtr<IExpandableListPosition> temp = ExpandableListPosition::Obtain(type, groupPos, childPos, flatListPos);
    *position = temp;
    REFCOUNT_ADD(*position);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
