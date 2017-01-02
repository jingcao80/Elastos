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

#include "ViewUtil.h"
#include "CViewUtil.h"

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CAR_INTERFACE_IMPL(CViewUtil, Singleton, IViewUtil)

CAR_SINGLETON_IMPL(CViewUtil)

ECode CViewUtil::GetConstantPreLayoutWidth(
    /* [in] */ IView* view,
    /* [out] */ Int32* width)
{
    return ViewUtil::GetConstantPreLayoutWidth(view, width);
}

ECode CViewUtil::IsViewLayoutRtl(
    /* [in] */ IView* view,
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);
    *result = ViewUtil::IsViewLayoutRtl(view);
    return NOERROR;
}

ECode CViewUtil::AddRectangularOutlineProvider(
    /* [in] */ IView* view,
    /* [in] */ IResources* res)
{
    ViewUtil::AddRectangularOutlineProvider(view, res);
    return NOERROR;
}

ECode CViewUtil::SetupFloatingActionButton(
    /* [in] */ IView* view,
    /* [in] */ IResources* res)
{
    ViewUtil::SetupFloatingActionButton(view, res);
    return NOERROR;
}


ECode CViewUtil::AddBottomPaddingToListViewForFab(
    /* [in] */ IListView* listView,
    /* [in] */ IResources* res)
{
    ViewUtil::AddBottomPaddingToListViewForFab(listView, res);
    return NOERROR;
}

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos
