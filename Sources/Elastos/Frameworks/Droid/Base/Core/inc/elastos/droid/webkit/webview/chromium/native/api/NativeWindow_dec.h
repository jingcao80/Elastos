//./Elastos/LibCore/inc/Elastos/IO/Buffer.h
//put this file at the end of the include list
//so the type definition used in this file will be found
#ifndef ELASTOS_NATIVEWINDOW_CALLBACK_DEC_HH
#define ELASTOS_NATIVEWINDOW_CALLBACK_DEC_HH
#include <android/native_window.h>

#ifdef __cplusplus
extern "C"
{
#endif
    extern void Elastos_NativeWindow_InitCallback(Handle32 cb);
#ifdef __cplusplus
}
#endif

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

struct ElaNativeWindowCallback
{
    ANativeWindow* (*elastos_NativeWindow_GetFromSurface)(IInterface* obj);
};

void* AwNativeWindow::ElaNativeWindowCallback_Init()
{
    static ElaNativeWindowCallback sElaNativeWindowCallback;

    sElaNativeWindowCallback.elastos_NativeWindow_GetFromSurface = &AwNativeWindow::GetFromSurface;

    Elastos_NativeWindow_InitCallback((Handle32)&sElaNativeWindowCallback);
    return &sElaNativeWindowCallback;
}

static void* sPElaNativeWindowCallback = AwNativeWindow::ElaNativeWindowCallback_Init();

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
#endif //
