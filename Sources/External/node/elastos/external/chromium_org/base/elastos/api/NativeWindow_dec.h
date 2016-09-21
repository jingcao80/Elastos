//./Elastos/LibCore/inc/Elastos/IO/Buffer.h
//put this file at the end of the include list
//so the type definition used in this file will be found
#ifndef ELASTOS_NATIVEWINDOW_CALLBACK_DEC_HH
#define ELASTOS_NATIVEWINDOW_CALLBACK_DEC_HH
#include "elastos/native_window.h"

//the implementation for below functioin refer to frameworks/base/core/jni/android_view_Surface.cpp:android_view_Surface_getSurface
//the implementation should be defined in Elastos/Frameworks/Droid/Base/Core/inc/view/CSurface.h
//should be as below
/*
    ANativeWindow* getFromSurface(IInterface* obj)
    {
        sp<ANativeWindow> win = android::sp<android::Surface> surface = ((CSurface*)obj)->mNativeSurface;
        if (win != NULL) {
            win->incStrong((void*)ANativeWindow_acquire);
        }
        return win.get();
    }
 */
struct ElaNativeWindowCallback
{
    ANativeWindow* (*elastos_NativeWindow_GetFromSurface)(IInterface* obj);
};

#endif //
