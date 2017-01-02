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

#include "elastos/droid/launcher2/CPagedViewWidgetHelper.h"
#include "elastos/droid/launcher2/PagedViewWidget.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CPagedViewWidgetHelper)

CAR_INTERFACE_IMPL(CPagedViewWidgetHelper, Singleton, IPagedViewWidgetHelper)

ECode CPagedViewWidgetHelper::SetDeletePreviewsWhenDetachedFromWindow(
    /* [in] */ Boolean value)
{
    return PagedViewWidget::SetDeletePreviewsWhenDetachedFromWindow(value);
}

ECode CPagedViewWidgetHelper::SetRecyclePreviewsWhenDetachedFromWindow(
    /* [in] */ Boolean value)
{
    return PagedViewWidget::SetRecyclePreviewsWhenDetachedFromWindow(value);
}

ECode CPagedViewWidgetHelper::ResetShortPressTarget()
{
    return PagedViewWidget::ResetShortPressTarget();
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos