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

#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETMANAGERHELPER_H__
#define __ELASTOS_DROID_APPWIDGET_CAPPWIDGETMANAGERHELPER_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetManagerHelper)
    , public Singleton
    , public IAppWidgetManagerHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    /**
     * Get the AppWidgetManager instance to use for the supplied {@link android.content.Context
     * Context} object.
     */
    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IAppWidgetManager** appWidgetManager);
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APPWIDGET_CAPPWIDGETMANAGERHELPER_H__
