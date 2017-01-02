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

#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTVIEWHELPER_H__
#define __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTVIEWHELPER_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetHostViewHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetHostViewHelper)
    , public Singleton
    , public IAppWidgetHostViewHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()
    /**
     * As of ICE_CREAM_SANDWICH we are automatically adding padding to widgets targeting
     * ICE_CREAM_SANDWICH and higher. The new widget design guidelines strongly recommend
     * that widget developers do not add extra padding to their widgets. This will help
     * achieve consistency among widgets.
     *
     * Note: this method is only needed by developers of AppWidgetHosts. The method is provided in
     * order for the AppWidgetHost to account for the automatic padding when computing the number
     * of cells to allocate to a particular widget.
     *
     * @param context the current context
     * @param component the component name of the widget
     * @param padding Rect in which to place the output, if null, a new Rect will be allocated and
     *                returned
     * @return default padding for this widget, in pixels
     */
    CARAPI GetDefaultPaddingForWidget(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* component,
        /* [in] */ IRect* padding,
        /* [out] */ IRect** rect);
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTVIEWHELPER_H__
