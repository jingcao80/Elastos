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

