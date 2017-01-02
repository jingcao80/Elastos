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

#include <Elastos.Droid.View.h>
#include "elastos/droid/webkit/webview/chromium/native/api/AwNativeWindow.h"
#include "elastos/droid/webkit/webview/chromium/native/api/NativeWindow_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::ISurface;
//TODO: using Elastos::Droid::View::Surface;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

ANativeWindow* AwNativeWindow::GetFromSurface(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ISurface> iSurface = ISurface::Probe(obj);
    if (NULL == iSurface)
    {
        Logger::E("AwNativeWindow", "AwNativeWindow::GetFromSurface, iSurface is NULL");
        return NULL;
    }
    //CSurface define a static function return ANativeWindow from mNativeObject;= reinterpret_cast<Surface *>(mNativeObject);
    //ANativeWindow_fromSurface  ->>  base/native/android/native_window.cpp
    //    |
    //android_view_Surface_getNativeWindow  ->> base/core/jni/android_view_Surface.cpp
    //TODO: Surface*  surface = (Surface*)iSurface.Get();
    ANativeWindow* result = NULL;// TODO: surface->GetSurface();
    return result;
}

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
