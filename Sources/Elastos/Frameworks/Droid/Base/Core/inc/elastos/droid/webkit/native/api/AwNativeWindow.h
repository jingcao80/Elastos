#ifndef __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
#define __ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <android/native_window.h>

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Api {

class AwNativeWindow
{
public:
    static CARAPI_(void*) ElaNativeWindowCallback_Init();

private:
    static CARAPI_(ANativeWindow*) GetFromSurface(
        /* [in] */ IInterface* obj);
};

} //Api
} //Webkit
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_WEBKIT_NATIVE_API_AWBYTEBUFFER_H__
