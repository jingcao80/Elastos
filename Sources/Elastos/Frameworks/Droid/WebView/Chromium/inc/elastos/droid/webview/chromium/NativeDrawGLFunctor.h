#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_DRAWGL_FUNCTOR_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_DRAWGL_FUNCTOR_H_

#include "elastos/droid/ext/frameworkext.h"
#include "android_webview/public/browser/draw_gl.h"

#include <cstdlib>
#include <utils/Log.h>
//#include <errno.h>
#include <private/hwui/DrawGlInfo.h>
#include <string.h>
#include <sys/resource.h>
//#include <sys/time.h>
#include <utils/Functor.h>
#include <utils/Log.h>

#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

class NativeDrawGLFunctor
    : public android::Functor {
public:
     NativeDrawGLFunctor(
        /* [in] */ Int64 view_context);
     virtual ~NativeDrawGLFunctor();

     virtual CARAPI_(android::status_t) operator()(
        /* [in] */ Int32 what,
        /* [in] */ void* data);
     static AwDrawGLFunction* g_aw_drawgl_function;
private:
     Int64 view_context_;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_DRAWGL_FUNCTOR_H_

