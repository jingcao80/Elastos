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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHSERVICE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * This class provides java side access to the native PathService.
 */
//@JNINamespace("base::android")
class PathService
    : public Object
{
public:
    // Must match the value of DIR_MODULE in base/base_paths.h!
    static const Int32 DIR_MODULE = 3;

    static CARAPI_(void) Override(
        /* [in] */ Int32 what,
        /* [in] */ const String& path);

private:
    // Prevent instantiation.
    PathService();

    static CARAPI_(void) NativeOverride(
        /* [in] */ Int32 what,
        /* [in] */ const String& path);
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHSERVICE_H__
