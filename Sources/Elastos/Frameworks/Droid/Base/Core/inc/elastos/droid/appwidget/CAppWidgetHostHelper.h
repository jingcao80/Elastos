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

#ifndef __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTHELPER_H__
#define __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTHELPER_H__

#include "_Elastos_Droid_AppWidget_CAppWidgetHostHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace AppWidget {

CarClass(CAppWidgetHostHelper)
    , public Singleton
    , public IAppWidgetHostHelper
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()
    /**
     * Remove all records about all hosts for your package.
     * <ul>
     *   <li>Call this when initializing your database, as it might be because of a data wipe.</li>
     *   <li>Call this to have the AppWidget manager release all resources associated with your
     *   host.  Any future calls about this host will cause the records to be re-allocated.</li>
     * </ul>
     */
    CARAPI DeleteAllHosts();
};

} // namespace AppWidget
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APPWIDGET_CAPPWIDGETHOSTHELPER_H__
