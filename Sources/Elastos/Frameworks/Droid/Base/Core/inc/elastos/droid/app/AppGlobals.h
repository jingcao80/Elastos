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

#ifndef __ELASTOS_DROID_APP_APPGLOBALS_H__
#define __ELASTOS_DROID_APP_APPGLOBALS_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::App::IApplication;
using Elastos::Droid::Content::Pm::IIPackageManager;

namespace Elastos {
namespace Droid {
namespace App {

class ECO_PUBLIC AppGlobals
{
public:
    /**
     * Return the first Application object made in the process.
     * NOTE: Only works on the main thread.
     */
    static CARAPI_(AutoPtr<IApplication>) GetInitialApplication();

    /**
     * Return the package name of the first .apk loaded into the process.
     * NOTE: Only works on the main thread.
     */
    static CARAPI_(String) GetInitialPackage();

    /**
     * Return the raw interface to the package manager.
     * @return The package manager.
     */
    static CARAPI_(AutoPtr<IIPackageManager>) GetPackageManager();

    /**
     * Gets the value of an integer core setting.
     *
     * @param key The setting key.
     * @param defaultValue The setting default value.
     * @return The core settings.
     */
    static CARAPI_(Int32) GetIntCoreSetting(
        /* [in] */ const String& key,
        /* [in] */ Int32 defaultValue);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_APPGLOBALS_H__
