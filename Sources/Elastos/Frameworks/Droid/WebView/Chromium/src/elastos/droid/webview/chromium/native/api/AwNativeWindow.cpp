
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
