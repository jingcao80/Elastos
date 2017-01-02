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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JSRESULTHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JSRESULTHANDLER_H__

#include <Elastos.CoreLibrary.Core.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/JsResultReceiver.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/JsPromptResultReceiver.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;
// import org.chromium.base.ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {
class AwContentsClientBridge;

class JsResultHandler
    : public Object
    , public JsResultReceiver
    , public JsPromptResultReceiver
{
private:
    class ConfirmRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        ConfirmRunnable(
            /* [in] */ JsResultHandler* owner,
            /* [in] */ const String& promptResult);

        CARAPI Run();

    private:
        JsResultHandler* mOwner;
        String mPromptResult;
    };

    class CancelRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()
        CancelRunnable(
            /* [in] */ JsResultHandler* owner);

        CARAPI Run();

    private:
        JsResultHandler* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    JsResultHandler(
        /* [in] */ AwContentsClientBridge* bridge,
        /* [in] */ Int32 id);

    //@Override
    CARAPI Confirm();

    //@Override
    CARAPI Confirm(
        /* [in] */ const String& promptResult);

    //@Override
    CARAPI Cancel();

private:
    JsResultHandler(const JsResultHandler&);
    JsResultHandler& operator=(const JsResultHandler&);
    AwContentsClientBridge* mBridge;
    const Int32 mId;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_JSRESULTHANDLER_H__
