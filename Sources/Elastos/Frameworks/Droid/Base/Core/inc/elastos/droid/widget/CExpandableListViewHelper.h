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

#ifndef __ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWHELPER_H__
#define __ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWHELPER_H__

#include "_Elastos_Droid_Widget_CExpandableListViewHelper.h"
#include "elastos/droid/widget/ExpandableListView.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CExpandableListViewHelper)
    , public Singleton
    , public IExpandableListViewHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetPackedPositionType(
        /* [in] */ Int64 packedPosition,
        /* [out] */ Int32* type);

    CARAPI GetPackedPositionGroup(
        /* [in] */ Int64 packedPosition,
        /* [out] */ Int32* position);

    CARAPI GetPackedPositionChild(
        /* [in] */ Int64 packedPosition,
        /* [out] */ Int32* position);

    CARAPI GetPackedPositionForChild(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ Int64* position);

    CARAPI GetPackedPositionForGroup(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Int64* position);
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CEXPANDABLELISTVIEWHELPER_H__
