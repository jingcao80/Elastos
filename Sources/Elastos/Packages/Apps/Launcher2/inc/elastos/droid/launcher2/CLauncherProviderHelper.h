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

#ifndef __ELASTOS_DROID_LAUNCHER2_CLAUNCHERPROVIDERHELPER_H__
#define __ELASTOS_DROID_LAUNCHER2_CLAUNCHERPROVIDERHELPER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "_Elastos_Droid_Launcher2_CLauncherProviderHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CLauncherProviderHelper)
    , public Singleton
    , public ILauncherProviderHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCONTENT_APPWIDGET_RESET_URI(
        /* [out] */ IUri** uri);

    /**
     * Build a query string that will match any row where the column matches
     * anything in the values list.
     */
    CARAPI BuildOrWhereString(
        /* [in] */ const String& column,
        /* [in] */ ArrayOf<Int32>* values,
        /* [out] */ String* result);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CLAUNCHERPROVIDERHELPER_H__