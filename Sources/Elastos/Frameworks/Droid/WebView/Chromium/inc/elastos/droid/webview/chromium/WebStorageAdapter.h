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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBSTORAGEADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBSTORAGEADAPTER_H_

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwQuotaManagerBridge.h"

using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwQuotaManagerBridge;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Droid::Webkit::IWebStorage;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

/**
  * Chromium implementation of WebStorage -- forwards calls to the
  * chromium internal implementation.
  */
class WebStorageAdapter
    : public Object
    , public IWebStorage
{
private:
    class InnerValueCallback
        : public Object
        , public IValueCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerValueCallback(
            /* [in] */ WebStorageAdapter* owner,
            /* [in] */ IValueCallback* callback);

        // @Override
        CARAPI OnReceiveValue(
            /* [in] */ IInterface* origins);

    private:
        WebStorageAdapter* mOwner;
        IValueCallback* mCallback;
    };

public:
    CAR_INTERFACE_DECL()

    WebStorageAdapter(
        /* [in] */ AwQuotaManagerBridge* quotaManagerBridge);

    // @Override
    CARAPI GetOrigins(
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI GetUsageForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI GetQuotaForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ IValueCallback* callback);

    // @Override
    CARAPI SetQuotaForOrigin(
        /* [in] */ const String& origin,
        /* [in] */ Int64 quota);

    // @Override
    CARAPI DeleteOrigin(
        /* [in] */ const String& origin);

    // @Override
    CARAPI DeleteAllData();

private:
    /*const*/ AutoPtr<AwQuotaManagerBridge> mQuotaManagerBridge;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_WEBSTORAGEADAPTER_H_

