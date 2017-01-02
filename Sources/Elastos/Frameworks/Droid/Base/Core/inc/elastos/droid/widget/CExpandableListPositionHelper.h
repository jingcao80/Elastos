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

#ifndef __ELASTOS_DROID_WIDGET_CEXPANDABLELISTPOSITIONHELPER_H__
#define __ELASTOS_DROID_WIDGET_CEXPANDABLELISTPOSITIONHELPER_H__

#include "_Elastos_Droid_Widget_CExpandableListPositionHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Widget::IExpandableListPosition;
using Elastos::Droid::Widget::IExpandableListPositionHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CExpandableListPositionHelper)
    , public Singleton
    , public IExpandableListPositionHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI ObtainGroupPosition(
        /* [in] */ Int32 groupPosition,
        /* [out] */ IExpandableListPosition** position);

    CARAPI ObtainChildPosition(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ IExpandableListPosition** position);

    CARAPI ObtainPosition(
        /* [in] */ Int64 packedPosition,
        /* [out] */ IExpandableListPosition** position);

    CARAPI Obtain(
        /* [in] */ Int32 type,
        /* [in] */ Int32 groupPos,
        /* [in] */ Int32 childPos,
        /* [in] */ Int32 flatListPos,
        /* [out] */ IExpandableListPosition** position);
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CEXPANDABLELISTPOSITIONHELPER_H__

