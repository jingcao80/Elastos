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

#include "elastos/droid/appwidget/CAppWidgetHostViewHelper.h"
#include "elastos/droid/appwidget/AppWidgetHostView.h"

namespace Elastos {
namespace Droid {
namespace AppWidget {

CAR_SINGLETON_IMPL(CAppWidgetHostViewHelper);

CAR_INTERFACE_IMPL(CAppWidgetHostViewHelper, Singleton, IAppWidgetHostViewHelper);

ECode CAppWidgetHostViewHelper::GetDefaultPaddingForWidget(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* component,
    /* [in] */ IRect* padding,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);
    AutoPtr<IRect> r = AppWidgetHostView::GetDefaultPaddingForWidget(context, component, padding);
    *rect = r;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos
